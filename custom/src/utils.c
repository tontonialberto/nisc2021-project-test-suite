#include "utils.h"
#include <math.h>

float getCartesian(float scaledValue, float maxValue) {
  return scaledValue * maxValue;
}

float getPolarRadius(float xCartesian, float yCartesian) {
  return sqrt((xCartesian * xCartesian) + (yCartesian * yCartesian));
}

uint16_t getPolarDegrees(float xCartesian, float yCartesian) {
  int result;

  if(xCartesian == 0 && yCartesian == 0) {
    return 0;
  }

  // Compute atan and convert to degrees.
  result = (atan2(yCartesian, xCartesian) * 57.2957795f);

  // Since atan2 degrees are in interval [-180, +180], translate in order to have [0, +360]
  result = (result + 360) % 360;

  return (uint16_t)result;
}

// Shorthand to evaluate whether      min < x < max (with equal signs)
static _Bool IS_BETWEEN(int x, int min, int max) { return (x >= min) && (x <= max); }

RGBPercentage degreesToRGB(uint16_t degrees, float radius, float maxRadius) {
  RGBPercentage result;

  // Handle RED component
  if(IS_BETWEEN(degrees, 210, 330)) {
    result.red = 0;
  }
  else if(IS_BETWEEN(degrees, 330, 360)) {
    result.red = 10000 * (degrees - 330) / 60;
  }
  else if(IS_BETWEEN(degrees, 0, 30)) {
    result.red = 10000 * (degrees + 30) / 60;
  }
  else if(IS_BETWEEN(degrees, 30, 150)) {
    result.red = 10000;
  }
  else { // degrees between [150, 210]
    result.red = 10000 * (210 - degrees) / 60;
  }

  // Handle GREEN component
  if(IS_BETWEEN(degrees, 90, 210)) {
    result.green = 0;
  }
  else if(IS_BETWEEN(degrees, 210, 270)) {
    result.green = 10000 * (degrees - 210) / 60;
  }
  else if(IS_BETWEEN(degrees, 270, 360) || IS_BETWEEN(degrees, 0, 30)) {
    result.green = 10000;
  }
  else { // degrees between [30, 90]
    result.green = 10000 * (90 - degrees) / 60;
  }

  // Handle BLUE component
  if(IS_BETWEEN(degrees, 330, 360) || IS_BETWEEN(degrees, 0, 90)) {
    result.blue = 0;
  }
  else if(IS_BETWEEN(degrees, 90, 150)) {
    result.blue = 10000 * (degrees - 90) / 60;
  }
  else if(IS_BETWEEN(degrees, 150, 270)) {
    result.blue = 10000;
  }
  else { // degrees between [270, 330]
    result.blue = 10000 * (330 - degrees) / 60;
  }

  radius = radius + (radius / 100); // Used to avoid sticking at 99% intensity.
  float intensityRatio = radius / maxRadius;

  result.red *= intensityRatio;
  result.green *= intensityRatio;
  result.blue *= intensityRatio;

  // Fix the rgb value to 100 even if the computation resulted in a bigger value
  if(result.red > 10000) result.red = 10000;
  if(result.green > 10000) result.green = 10000;
  if(result.blue > 10000) result.blue = 10000;

  return result;
}

float mapSquareToCircleX(float x, float y) {
  return x * sqrt(1 - (y * y / 2));
}

float mapSquareToCircleY(float x, float y) {
  return y * sqrt(1 - (x * x / 2));
}
