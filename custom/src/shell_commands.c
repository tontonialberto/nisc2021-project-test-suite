#include "shell_commands.h"
#include "string.h"
#include "stdlib.h"
#include "chprintf.h"
#include "constants.h"

extern uint8_t ledMode;
extern uint8_t ledColor;
extern uint8_t service;
extern uint8_t ledStaticMode;
extern uint32_t ledDynamicMs;
extern uint8_t joyMode;
extern uint8_t oledMode;
extern uint8_t buzzerMode;
extern uint16_t buzzerSeconds;
extern uint8_t dimmerMode;
extern uint8_t dimmerColor;

// "RED" = 0
// "GREEN" = 1
// "BLUE" = 2
// Otherwise -1
static int getColorFromLabel(const char *label) {

  int result = -1;

  if(strcmp(label, "RED") == 0)
    result = 0;
  else if(strcmp(label, "GREEN") == 0)
    result = 1;
  else if(strcmp(label, "BLUE") == 0)
    result = 2;

  return result;
}


static _Bool containsDigitsOnly(const char *str) {

  _Bool result = true;

  while(*str != 0) {
    if(*str < '0' || *str > '9') {
      return false;
    }
    str++;
  }

  return result;
}

void cmd_led(BaseSequentialStream *chp, int argc, char *argv[]) {
  const char *strColor = argv[0];
  const char *strMode = argv[1];

  // argv[2] assigned to both variables because they will be used on different user input configs
  const char *strStaticMode = argv[2];
  const char *strDynamicMs = argv[2];

  if(argc < 3
      || (  ( strcmp(strColor, "RED") != 0 ) && ( strcmp(strColor, "GREEN") != 0 ) && ( strcmp(strColor, "BLUE") != 0 )    )
      || (  ( strcmp(strMode, "STATIC") != 0 ) && ( strcmp(strMode, "DYNAMIC") != 0 )    )
  ) {
    chprintf(chp, "Usage: LED [RED|GREEN|BLUE] [STATIC|DYNAMIC] ...\r\n");
    return;
  }

  if( ( strcmp(strMode, "STATIC") == 0 )
      && ( strcmp(strStaticMode, "ON") != 0 )
      && ( strcmp(strStaticMode, "OFF") != 0 )
  ) {
    chprintf(chp, "Usage: LED [RED|GREEN|BLUE] [STATIC] [ON|OFF] \r\n");
    return;
  }
  else if(( strcmp(strMode, "DYNAMIC") == 0 ) && !containsDigitsOnly(strDynamicMs)) {
    chprintf(chp, "Usage: LED [RED|GREEN|BLUE] [DYNAMIC] [PERIODMS(1-1000)]\r\n");
    return;
  }

  // At this point the input is assumed to be correct.

  service = SERVICE_LED;

  if(strcmp(strMode, "STATIC") == 0) {

    ledMode = LED_MODE_STATIC;
    ledColor = getColorFromLabel(strColor);

    if(strcmp(strStaticMode, "ON") == 0) {
      ledStaticMode = LED_STATIC_ON;
    }
    else if(strcmp(strStaticMode, "OFF") == 0) {
      ledStaticMode = LED_STATIC_OFF;
    }
  }
  else if(strcmp(strMode, "DYNAMIC") == 0) {

    ledMode = LED_MODE_DYNAMIC;
    ledColor = getColorFromLabel(strColor);
    ledDynamicMs = atoi(strDynamicMs);
  }
}

void cmd_joy(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void)argv;

  const char *strMode = argv[0];

  // Input checking.
  if(argc != 1
      || (
          (strcmp(strMode,"POLAR") != 0)
          && (strcmp(strMode,"XY") != 0)
      )
  ) {
    chprintf(chp, "Usage: JOY [XY|POLAR]\r\n");
    return;
  }

  // At this point the input is assumed to be correct.

  service = SERVICE_JOY;
  if(strcmp(strMode,"XY") == 0) {
    joyMode = JOY_CHOICE_CARTESIAN;
  }
  else if(strcmp(strMode,"POLAR") == 0) {
    joyMode = JOY_CHOICE_POLAR;
  }
}

void cmd_oled(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void)argv;

  const char *strMode = argv[0];

  if(   ( argc != 1 ) || (    ( strcmp(strMode, "LED") != 0 ) && ( strcmp(strMode, "JOY") != 0 )    )   ) {
    chprintf(chp, "Usage: OLED [LED|JOY]\r\n");
    return;
  }

  // At this point the input is assumed to be correct.

  service = SERVICE_OLED;

  if(strcmp(strMode, "LED") == 0) {
    oledMode = OLED_MODE_LED;
  }
  else if(strcmp(strMode, "JOY") == 0) {
    oledMode = OLED_MODE_JOY;
  }
}

void cmd_buzz(BaseSequentialStream *chp, int argc, char *argv[]) {

  const char *strMode = argv[0];
  const char *strSeconds = argv[1];

  // The input should be:
  // -At least one parameter
  // -ON requires another parameter, and it is a positive number
  // -OFF is the only parameter
  // -First parameter is "ON" or "OFF"
  if(
      argc <= 0
      || (  ( strcmp(strMode, "ON") == 0 ) && ( argc != 2 ) )
      || (  ( strcmp(strMode, "OFF") == 0 ) && ( argc != 1 )  )
      || (  ( strcmp(strMode, "ON") != 0 ) && ( strcmp(strMode, "OFF") != 0 )    )
      || (  ( strcmp(strMode, "ON") == 0 ) && ( !containsDigitsOnly(strSeconds) )  )
  ) {
    chprintf(chp, "Usage: BUZZ [ON|OFF] [PERIOD]\r\n");
    return;
  }

  // At this point the input is assumed to be correct.

  service = SERVICE_BUZZ;

  if(strcmp(strMode, "ON") == 0) {
    buzzerMode = BUZZER_MODE_ON;
    buzzerSeconds = atoi(strSeconds);
  }
  else if(strcmp(strMode, "OFF") == 0) {
    buzzerMode = BUZZER_MODE_OFF;
  }
}

void cmd_dimmer(BaseSequentialStream *chp, int argc, char *argv[]) {

  const char *strMode = argv[0];
  const char *strColor = argv[1];

  // Check all input errors:
  // -No parameters
  // -ON requires another parameter, which is red, green, or blue
  // -OFF is the only parameter
  // -Wrong parameter name
  if(
      argc <= 0
      || (  ( strcmp(strMode, "ON") == 0 ) && ( argc != 2 ) )
      || (  ( strcmp(strMode, "OFF") == 0) && ( argc != 1)  )
      || (  ( strcmp(strMode, "ON") != 0 ) && ( strcmp(strMode, "OFF") != 0 )    )
      || (  ( strcmp(strMode, "ON") == 0 ) && ( strcmp(strColor, "RED") != 0) && ( strcmp(strColor, "GREEN") != 0 ) && ( strcmp(strColor, "BLUE") != 0 )    )
  ) {

    chprintf(chp, "Usage: DIMMER [ON|OFF] [RED|GREEN|BLUE]\r\n");
    return;
  }

  // At this point the input is assumed to be correct.

  service = SERVICE_DIMMER;

  if(strcmp(strMode, "OFF") == 0) {
    dimmerMode = DIMMER_MODE_OFF;
  }
  else if(strcmp(strMode, "ON") == 0) {
    dimmerMode = DIMMER_MODE_ON;
    dimmerColor = (uint8_t)getColorFromLabel(strColor);
  }
}

void cmd_demo(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void)chp;
  (void)argc;
  (void)argv;

  // No parameters, nothing to check here.

  service = SERVICE_DEMO;
}
