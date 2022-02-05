#include "handler_oled.h"
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "ssd1306.h"
#include "types.h"
#include "utils.h"
#include "constants.h"

// Here i am referring to global variables defined in other source files.
extern RGBPercentage rgbStateSR;
extern Point rawJoystickSR;
extern uint8_t joyMode;
extern uint8_t oledMode;
extern uint8_t service;
extern SSD1306Driver SSD1306D1;
extern const SSD1306Config ssd1306cfg;

// Used by whoever wants to write on OLED from within this source file.
static char screenBuffer[50];

// Draws value a and b in this way: "aLabel: a, bLabel: b".
// OLED cursor is moved at x,y coordinates.
// Note: this function does not fill nor update the screen before/after drawing the values.
static void oledDrawPair(int x, int y, int a, int b, const char *aLabel, const char *bLabel, _Bool newLine);

// Draws "label: value" of three elements, each one separated by a line.
// OLED cursor is moved at x,y coordinates.
// Note: this function does not fill nor update the screen before/after drawing the values.
static void oledDrawTriplet(int x, int y, int a, int b, int c, const char *aLabel, const char *bLabel, const char *cLabel);

static THD_WORKING_AREA(waThreadOled, 512);
static THD_FUNCTION(ThreadOled, arg) {
  (void)arg;
  float x, y, r, thetaDegrees;
  uint32_t red, green, blue;

  chRegSetThreadName("ThreadOled");

  ssd1306ObjectInit(&SSD1306D1);
  ssd1306Start(&SSD1306D1, &ssd1306cfg);

  while(true) {

    chSysLock();
    x = getCartesian(rawJoystickSR.x, JOYSTICK_CARTESIAN_MAX_VALUE);
    y = getCartesian(rawJoystickSR.y, JOYSTICK_CARTESIAN_MAX_VALUE);
    red = rgbStateSR.red;
    green = rgbStateSR.green;
    blue = rgbStateSR.blue;
    chSysUnlock();

    r = getPolarRadius(x, y);
    thetaDegrees = getPolarDegrees(x, y);

    ssd1306FillScreen(&SSD1306D1, SSD1306_COLOR_BLACK);

    if(service == SERVICE_JOY) {
      if (joyMode == JOY_CHOICE_POLAR) {
        oledDrawPair(5, 0, r, thetaDegrees, "R", "Theta", true);
      }
      else if (joyMode == JOY_CHOICE_CARTESIAN) {
        oledDrawPair(5, 0, x, y, "X", "Y", true);
      }
    }
    else if(service == SERVICE_OLED) {
      if(oledMode == OLED_MODE_JOY) {
        oledDrawPair(5, 0, x, y, "X", "Y", true);
      }
      else if(oledMode == OLED_MODE_LED) {
        // Print RGB components
        oledDrawTriplet(5, 0,
                        red / 100,
                        green / 100,
                        blue / 100,
                        "R", "G", "B");
      }
    }
    else if(service == SERVICE_DEMO) {
      // Print RGB components
      oledDrawTriplet(5, 0,
                      red / 100,
                      green / 100,
                      blue / 100,
                      "R", "G", "B");
    }
    else {
      ssd1306GotoXy(&SSD1306D1, 10, 32);
      ssd1306Puts(&SSD1306D1, "OLED IDLE", &ssd1306_font_11x18, SSD1306_COLOR_WHITE);
    }

    ssd1306UpdateScreen(&SSD1306D1);

    chThdSleepMilliseconds(50);
  }
}

thread_t *CreateThread_Oled(tprio_t prio, void *arg) {
  return chThdCreateStatic(waThreadOled, sizeof(waThreadOled), prio, ThreadOled, arg);
}

static void oledDrawPair(int x, int y, int a, int b, const char *aLabel, const char *bLabel, _Bool newLine) {
  if(newLine) {
    chsnprintf(screenBuffer, sizeof(screenBuffer), "%s: %d", aLabel, a);
    ssd1306GotoXy(&SSD1306D1, x, y);
    ssd1306Puts(&SSD1306D1, screenBuffer, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);

    chsnprintf(screenBuffer, sizeof(screenBuffer), "%s: %d", bLabel, b);
    ssd1306GotoXy(&SSD1306D1, x, y + 18);
    ssd1306Puts(&SSD1306D1, screenBuffer, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);
  }
  else {
    chsnprintf(screenBuffer, sizeof(screenBuffer), "%s: %d, %s: %d", aLabel, a, bLabel, b);
    ssd1306GotoXy(&SSD1306D1, x, y);
    ssd1306Puts(&SSD1306D1, screenBuffer, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);
  }
}

static void oledDrawTriplet(int x, int y, int a, int b, int c, const char *aLabel, const char *bLabel, const char *cLabel) {
  chsnprintf(screenBuffer, sizeof(screenBuffer), "%s: %d", aLabel, a);
  ssd1306GotoXy(&SSD1306D1, x, y);
  ssd1306Puts(&SSD1306D1, screenBuffer, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);

  chsnprintf(screenBuffer, sizeof(screenBuffer), "%s: %d", bLabel, b);
  ssd1306GotoXy(&SSD1306D1, x, y + 18);
  ssd1306Puts(&SSD1306D1, screenBuffer, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);

  chsnprintf(screenBuffer, sizeof(screenBuffer), "%s: %d", cLabel, c);
  ssd1306GotoXy(&SSD1306D1, x, y + 36);
  ssd1306Puts(&SSD1306D1, screenBuffer, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);
}
