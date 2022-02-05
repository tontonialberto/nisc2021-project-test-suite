#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "ssd1306.h"
#include "shell.h"
#include "stdio.h"
#include "types.h"
#include "constants.h"
#include "handler_buzzer.h"
#include "handler_joystick.h"
#include "handler_oled.h"
#include "handler_rgb.h"
#include "handler_encoder.h"
#include "shell_commands.h"

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

char buffer[20];

// Used to avoid changing tons of code if different drivers of the same peripheral are needed
PWMDriver *pwmpRgb = &PWMD1;
ICUDriver *icuDriver = &ICUD5;

// The following variables are used to coordinate system operation.
// Those ones which have SR postfix require mutual exclusion to be accessed (mainly critical sections).

// RGB led state (based on the image design pattern)
volatile RGBPercentage rgbStateSR = { .red = 0, .green = 0, .blue = 0 };

volatile uint8_t service = SERVICE_NONE;
volatile uint8_t joyMode;
volatile uint8_t oledMode;
volatile uint8_t buzzerMode;
volatile uint16_t buzzerSeconds;
volatile uint8_t ledMode;
volatile uint8_t ledColor;
volatile uint8_t ledStaticMode;
volatile uint8_t dimmerMode = DIMMER_MODE_OFF;
volatile uint8_t dimmerColor;
volatile uint32_t ledDynamicMs;

static const I2CConfig i2ccfg = {
                                 OPMODE_I2C,
                                 400000,
                                 FAST_DUTY_CYCLE_2,
};

const SSD1306Config ssd1306cfg = {
                                  &I2CD1,
                                  &i2ccfg,
                                  SSD1306_SAD_0X78,
};

SSD1306Driver SSD1306D1;

const PWMConfig pwmcfg = {
                          .frequency = 10000,
                          .period = 100,
                          .callback = NULL,
                          {
                           {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                           {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                           {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                           {PWM_OUTPUT_DISABLED, NULL}
                          }
};

static ICUConfig icucfg = {
                           ICU_INPUT_ACTIVE_HIGH,                                                        // ICU is configured on active level
                           10000,                                                                         // Frequency is 200hz
                           cbIcuFallingEdge_RotaryEncoder,                                                                   // This callback is called when input signal pass from the high level to low level
                           NULL,                                                                  // This callback is called when input signal pass from the low level to high level
                           NULL,                                                                         // There is no callback when the counter goes in overflow
                           ICU_CHANNEL_1,                                                                // It configures the first channel of ICU Driver
};

const ShellCommand commands[] = { {"LED", cmd_led},
                                  {"JOY", cmd_joy},
                                  {"OLED", cmd_oled},
                                  {"BUZZ", cmd_buzz},
                                  {"DIMMER", cmd_dimmer},
                                  {"DEMO", cmd_demo},
                                  {NULL, NULL}
};

static const ShellConfig shell_cfg = {(BaseSequentialStream*)&SD2, commands};

//%--------------------------------------------------------


int main(void) {

  halInit();
  chSysInit();

  shellInit();

  sdStart(&SD2, NULL);

  /* Configuring I2C related PINs */
  palSetLineMode(LINE_ARD_D15, PAL_MODE_ALTERNATE(4) |
                 PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                 PAL_STM32_PUPDR_PULLUP);
  palSetLineMode(LINE_ARD_D14, PAL_MODE_ALTERNATE(4) |
                 PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                 PAL_STM32_PUPDR_PULLUP);

  // Associate R,G,B led lines with TIM1 CH1,2,3
  palSetLineMode(LINE_EXT_R, MODE_PWM_R);
  palSetLineMode(LINE_EXT_G, MODE_PWM_G);
  palSetLineMode(LINE_EXT_B, MODE_PWM_B);

  // Set buzzer line in output mode
  palSetLineMode(LINE_BUZZER, PAL_MODE_OUTPUT_PUSHPULL);

  // Set DT line in ICU mode, CH1.
  palSetLineMode(LINE_ENC_DT, PAL_MODE_ALTERNATE(2));

  // Starts the ICU driver
  icuStart(icuDriver, &icucfg);

  // Starts the capture of the signal on Channel 1
  icuStartCapture(icuDriver);

  // Enables ICU callbacks
  icuEnableNotifications(icuDriver);

  CreateThread_Buzzer(NORMALPRIO + 1, NULL);
  CreateThread_JoystickADC(NORMALPRIO + 1, NULL);
  CreateThread_Oled(NORMALPRIO + 1, NULL);
  CreateThread_RGBSoftware(NORMALPRIO + 1, NULL);
  CreateThread_RGBHardware(NORMALPRIO + 1, NULL);

  /*Infinite loop*/
  while (true) {
    thread_t *shelltp = chThdCreateFromHeap(NULL, SHELL_WA_SIZE, "shell", NORMALPRIO + 1, shellThread, (void*)&shell_cfg);
    chThdWait(shelltp);
  }
}


