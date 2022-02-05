/*
 * utils.h
 *
 *  Created on: 2 set 2021
 *      Author: Amministratore
 */

#ifndef CUSTOM_UTILS_H_
#define CUSTOM_UTILS_H_

#include <stdint.h>
#include "constants.h"
#include "types.h"

// Put here functions used by more source files of your project.

float getCartesian(float scaledValue, float maxValue);

float getPolarRadius(float xCartesian, float yCartesian);

uint16_t getPolarDegrees(float xCartesian, float yCartesian);

// Return rgb scale based on circular counterclockwise degrees of the color wheel.
// Intensity is determined based on the current polar radius.
// NB: degrees are in interval [0, 360], taken in counterclockwise way.
RGBPercentage degreesToRGB(uint16_t degrees, float radius, float maxRadius);

// Used to transform x coordinate computed from a square area to a circle area.
// It also needs corresponding y coordinate to compute result properly.
// Note: coordinates must be in interval [-1, +1]
float mapSquareToCircleX(float x, float y);

// Used to transform y coordinate computed from a square area to a circle area.
// It also needs corresponding x coordinate to compute result properly.
// Note: coordinates must be in interval [-1, +1]
float mapSquareToCircleY(float x, float y);

#endif /* CUSTOM_UTILS_H_ */
