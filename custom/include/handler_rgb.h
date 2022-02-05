/*
 * handler_rgb.h
 *
 *  Created on: 3 set 2021
 *      Author: Amministratore
 */

#ifndef CUSTOM_INCLUDE_HANDLER_RGB_H_
#define CUSTOM_INCLUDE_HANDLER_RGB_H_

#include "ch.h"
#include "hal.h"

thread_t *CreateThread_RGBSoftware(tprio_t prio, void *arg);

thread_t *CreateThread_RGBHardware(tprio_t prio, void *arg);

#endif /* CUSTOM_INCLUDE_HANDLER_RGB_H_ */
