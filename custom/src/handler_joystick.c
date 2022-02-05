#include "handler_joystick.h"
#include "ch.h"
#include "hal.h"

#define MSG_ADC_OK                  0x1337
#define MSG_ADC_KO                  0x7331
#define VOLTAGE_RES                 ((float)3.3/4096)
#define ADC_GRP_NUM_CHANNELS        2
#define ADC_GRP_BUF_DEPTH           16

static void adccallback(ADCDriver *adcp);
static void adcerrorcallback(ADCDriver *adcp, adcerror_t err);
static float centerAndScaleValue(float value, float maxValue);

static adcsample_t samples[ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH];
static thread_t *trp;

// SR: shared resource. Write code accordingly!
Point rawJoystickSR;

/*
 * ADC conversion group.
 * Mode:        Continuous on 2 channels, SW triggered.
 * Channels:    IN10 (GPIOC0), IN11 (GPIOC1)
 */
static const ADCConversionGroup adcgrpcfg = {
                                             FALSE,
                                             ADC_GRP_NUM_CHANNELS,
                                             adccallback,
                                             adcerrorcallback,
                                             0,                        /* CR1 */
                                             ADC_CR2_SWSTART,          /* CR2 */
                                             ADC_SMPR1_SMP_AN10(ADC_SAMPLE_144) | ADC_SMPR1_SMP_AN11(ADC_SAMPLE_144),  /* SMPR1 */
                                             0,                        /* SMPR2 */
                                             0,                        /* HTR */
                                             0,                        /* LTR */
                                             ADC_SQR1_NUM_CH(ADC_GRP_NUM_CHANNELS),                                    /* SQR1 */
                                             0,                        /* SQR2 */
                                             ADC_SQR3_SQ2_N(ADC_CHANNEL_IN11) | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN10)       /* SQR3 */
};

/*
 * ADC streaming callback.
 */
static void adccallback(ADCDriver *adcp) {
  if (adcIsBufferComplete(adcp)) {
    chSysLockFromISR();
    chThdResumeI(&trp, (msg_t) MSG_ADC_OK );  /* Resuming the thread with message 0x1337.*/
    chSysUnlockFromISR();
  }
}

/*
 * ADC error callback.
 */
static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {
  (void)adcp;
  (void)err;
  chSysLockFromISR();
  chThdResumeI(&trp, (msg_t) MSG_ADC_KO );  /* Resuming the thread with message 0x1337.*/
  chSysUnlockFromISR();
}

// Translates a value between -1 and +1. The center is obviously 0.
// It is assumed that value is inside the interval [0, maxValue].
static float centerAndScaleValue(float value, float maxValue) {
  float offset = maxValue / 2;
  return ((value / offset) - 1);
}


static THD_WORKING_AREA( waThreadJoystickADC, 128 );
static THD_FUNCTION( ThreadJoystickADC, arg ) {
  (void) arg;

  static float converted[ADC_GRP_NUM_CHANNELS];

  chRegSetThreadName("ThreadJoystickADC");

  /*
   * Group setting as analog input:
   *    PORTC PIN 0 -> ADC1_CH10
   *    PORTC PIN 1 -> ADC1_CH11
   */
  palSetGroupMode(GPIOC, PAL_PORT_BIT(0) | PAL_PORT_BIT(1),
                  0, PAL_MODE_INPUT_ANALOG);

  adcStart(&ADCD1, NULL);

  while( true ) {
    msg_t msg;
    int i;

    chSysLock();
    adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP_BUF_DEPTH);
    msg = chThdSuspendS(&trp);
    chSysUnlock();

    /* Check if acquisition is OK or KO */
    if( (uint32_t) msg == MSG_ADC_KO ) {
      continue;
    }

    /*
     * Clean the buffer
     */
    for( i = 0; i < ADC_GRP_NUM_CHANNELS; i++ ) {
      converted[i] = 0.0f;
    }

    for( i = 0; i < ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH; i++ ) {
      converted[ i % ADC_GRP_NUM_CHANNELS] += (float) samples[i] * VOLTAGE_RES;
    }

    for( i = 0; i < ADC_GRP_NUM_CHANNELS; i++ ) {
      converted[i] /= ADC_GRP_BUF_DEPTH;
    }

    /* Copy converted values into a new variable */
    chSysLock();
    rawJoystickSR.x = centerAndScaleValue(converted[1], 3.3f);
    rawJoystickSR.y = centerAndScaleValue(converted[0], 3.3f);
    chSysUnlock();
  }
}

thread_t *CreateThread_JoystickADC(tprio_t prio, void *arg) {
  return chThdCreateStatic(waThreadJoystickADC, sizeof(waThreadJoystickADC), prio, ThreadJoystickADC, arg);
}
