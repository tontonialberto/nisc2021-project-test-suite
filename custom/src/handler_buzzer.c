#include "handler_buzzer.h"
#include "ch.h"
#include "hal.h"
#include "types.h"
#include "constants.h"

extern uint8_t service;
extern uint8_t buzzerMode;
extern uint16_t buzzerSeconds;
extern RGBPercentage rgbStateSR;

static THD_WORKING_AREA(waThreadBuzzer, 128);
static THD_FUNCTION(ThreadBuzzer, arg) {
  (void)arg;

  chRegSetThreadName("ThreadBuzzer");

  while(true){
    if(service == SERVICE_BUZZ) {
      if(buzzerMode == BUZZER_MODE_ON){
        palWriteLine(LINE_BUZZER, true);
        chThdSleepMilliseconds(buzzerSeconds * 1000);
        palWriteLine(LINE_BUZZER, false);
        chThdSleepMilliseconds(buzzerSeconds * 1000);
      }
      else{
        palWriteLine(LINE_BUZZER, false);
      }
    }
    else if(service == SERVICE_DEMO){
      _Bool shouldBuzz = false;
      chSysLock();
      if(rgbStateSR.red >= 10000 || rgbStateSR.green >= 10000 || rgbStateSR.blue >= 10000 ){
        shouldBuzz = true;
      }
      chSysUnlock();
      if(shouldBuzz == true){
        palWriteLine(LINE_BUZZER, true);
      }
      else{
        palWriteLine(LINE_BUZZER, false);
      }
    }
    else{
      palWriteLine(LINE_BUZZER ,false);
    }

    chThdSleepMilliseconds(10);
  }
}

thread_t *CreateThread_Buzzer(tprio_t prio, void *arg) {
  return chThdCreateStatic(waThreadBuzzer, sizeof(waThreadBuzzer), prio, ThreadBuzzer, arg);
}

