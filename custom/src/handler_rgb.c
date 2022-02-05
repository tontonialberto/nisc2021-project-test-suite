#include "handler_rgb.h"
#include "hal.h"
#include <math.h>
#include "types.h"
#include "utils.h"
#include "constants.h"

extern uint8_t service;
extern uint8_t ledMode;
extern uint8_t ledColor;
extern uint8_t ledStaticMode;
extern uint32_t ledDynamicMs;
extern RGBPercentage rgbStateSR;
extern Point rawJoystickSR;
extern PWMDriver *pwmpRgb;
extern PWMConfig pwmcfg;

static void setStaticRGBState(RGBPercentage *rgbp, uint8_t color, uint8_t mode);

static THD_WORKING_AREA(waThreadRGBSoftware, 128);
static THD_FUNCTION(ThreadRGBSoftware, arg) {
  (void)arg;

  float x, y, polarRadius, thetaDegrees;

  pwmStart(pwmpRgb, &pwmcfg);

  pwmEnableChannel(pwmpRgb, CHANNEL_PWM_R, PWM_PERCENTAGE_TO_WIDTH(pwmpRgb, 0));
  pwmEnableChannel(pwmpRgb, CHANNEL_PWM_G, PWM_PERCENTAGE_TO_WIDTH(pwmpRgb, 0));
  pwmEnableChannel(pwmpRgb, CHANNEL_PWM_B, PWM_PERCENTAGE_TO_WIDTH(pwmpRgb, 0));

  while(true) {

    if(service == SERVICE_LED) {
      if (ledMode == LED_MODE_STATIC) {
        // Static. Turn off/on one component of rgb led.
        chSysLock();
        setStaticRGBState(&rgbStateSR, ledColor, ledStaticMode);
        chSysUnlock();
      }
      else if(ledMode == LED_MODE_DYNAMIC) {
        // Dynamic. Blink a channel of rgb led with a given period.
        chSysLock();
        setStaticRGBState(&rgbStateSR, ledColor, 1);
        chSysUnlock();

        chThdSleepMilliseconds(ledDynamicMs);

        chSysLock();
        setStaticRGBState(&rgbStateSR, ledColor, 0);
        chSysUnlock();

        chThdSleepMilliseconds(ledDynamicMs);
      }
    }
    else if (service == SERVICE_DEMO) {
      // DEMO service. Set rgb state according to joystick polar coords.

      chSysLock();
      // map joystick square domain to a circle
      x = mapSquareToCircleX(rawJoystickSR.x, rawJoystickSR.y);
      y = mapSquareToCircleY(rawJoystickSR.x, rawJoystickSR.y);
      x *= JOYSTICK_CARTESIAN_MAX_VALUE;
      y *= JOYSTICK_CARTESIAN_MAX_VALUE;
      chSysUnlock();

      polarRadius = getPolarRadius(x, y);
      thetaDegrees = getPolarDegrees(x, y);

      // state of rgb led is a shared variable, so we need a critical section
      // in order to read from it safely.
      chSysLock();
      rgbStateSR = degreesToRGB(thetaDegrees, polarRadius, JOYSTICK_CARTESIAN_MAX_VALUE);
      chSysUnlock();
    }

    chThdSleepMilliseconds(5);
  }
}

static THD_WORKING_AREA(waThreadRGBHardware, 128);
static THD_FUNCTION(ThreadRGBHardware, arg) {

  (void)arg;
  RGBPercentage previousRGB;

  while(true) {

    // Update PWM channel only if the rgb state was changed via software.
    chSysLock();
    if(previousRGB.red != rgbStateSR.red || previousRGB.green != rgbStateSR.green || previousRGB.blue != rgbStateSR.blue) {
      pwmEnableChannelI(pwmpRgb, CHANNEL_PWM_R, PWM_PERCENTAGE_TO_WIDTH(pwmpRgb, rgbStateSR.red));
      pwmEnableChannelI(pwmpRgb, CHANNEL_PWM_G, PWM_PERCENTAGE_TO_WIDTH(pwmpRgb, rgbStateSR.green));
      pwmEnableChannelI(pwmpRgb, CHANNEL_PWM_B, PWM_PERCENTAGE_TO_WIDTH(pwmpRgb, rgbStateSR.blue));

      previousRGB.red = rgbStateSR.red;
      previousRGB.green = rgbStateSR.green;
      previousRGB.blue = rgbStateSR.blue;
    }
    chSysUnlock();

    chThdSleepMilliseconds(5);
  }
}

thread_t *CreateThread_RGBSoftware(tprio_t prio, void *arg) {
  return chThdCreateStatic(waThreadRGBSoftware, sizeof(waThreadRGBSoftware), prio, ThreadRGBSoftware, arg);
}

thread_t *CreateThread_RGBHardware(tprio_t prio, void *arg) {
  return chThdCreateStatic(waThreadRGBHardware, sizeof(waThreadRGBHardware), prio, ThreadRGBHardware, arg);
}

static void setStaticRGBState(RGBPercentage *rgbp, uint8_t color, uint8_t mode )
{
  switch (color) {
  case 0:
    rgbp->red = (mode == 0) ? 0 : 10000;
    break;
  case 1:
    rgbp->green = (mode == 0) ? 0 : 10000;
    break;
  case 2:
    rgbp->blue = (mode == 0) ? 0 : 10000;
    break;
  }
}
