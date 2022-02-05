#ifndef CUSTOM_HANDLER_JOYSTICK_H_
#define CUSTOM_HANDLER_JOYSTICK_H_

#include "ch.h"
#include "types.h"

extern Point rawJoystickSR;

thread_t *CreateThread_JoystickADC(tprio_t prio, void *arg);

#endif
