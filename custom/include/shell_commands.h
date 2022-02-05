/*
 * shell_commands.h
 *
 *  Created on: 3 set 2021
 *      Author: Amministratore
 */

#ifndef CUSTOM_INCLUDE_SHELL_COMMANDS_H_
#define CUSTOM_INCLUDE_SHELL_COMMANDS_H_

#include "hal.h"

void cmd_led(BaseSequentialStream *chp, int argc, char *argv[]);

void cmd_joy(BaseSequentialStream *chp, int argc, char *argv[]);

void cmd_oled(BaseSequentialStream *chp, int argc, char *argv[]);

void cmd_buzz(BaseSequentialStream *chp, int argc, char *argv[]);

void cmd_dimmer(BaseSequentialStream *chp, int argc, char *argv[]);

void cmd_demo(BaseSequentialStream *chp, int argc, char *argv[]);

#endif /* CUSTOM_INCLUDE_SHELL_COMMANDS_H_ */
