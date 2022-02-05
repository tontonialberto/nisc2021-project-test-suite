#include "handler_encoder.h"
#include "ch.h"
#include "types.h"
#include "constants.h"

extern uint8_t service;
extern uint8_t dimmerMode;
extern uint8_t dimmerColor;
extern RGBPercentage rgbStateSR;

void setRGBWithinBounds(RGBPercentage *rgbp, uint8_t color, int16_t variation) {
  switch(color) {
  case 0:
    rgbp->red += variation;
    if(rgbp->red < 0) rgbp->red = 0;
    if(rgbp->red > 10000) rgbp->red = 10000;
    break;

  case 1:
    rgbp->green += variation;
    if(rgbp->green < 0) rgbp->green = 0;
    if(rgbp->green > 10000) rgbp->green = 10000;
    break;

  case 2:
    rgbp->blue += variation;
    if(rgbp->blue < 0) rgbp->blue = 0;
    if(rgbp->blue > 10000) rgbp->blue = 10000;
    break;
  }
}

void cbIcuFallingEdge_RotaryEncoder(ICUDriver *icup) {
  (void)icup;

  _Bool clkHigh = palReadLine(LINE_ENC_CLK);
  int16_t variation = 0; // -100 counterclockwise, +100 clockwise

  variation = clkHigh ? +100 : -100;

  chSysLockFromISR();
  if(dimmerMode == DIMMER_MODE_ON) {
    setRGBWithinBounds(&rgbStateSR, dimmerColor, variation);
  }
  chSysUnlockFromISR();
}
