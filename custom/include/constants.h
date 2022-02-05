#ifndef CUSTOM_INCLUDE_CONSTANTS_H_
#define CUSTOM_INCLUDE_CONSTANTS_H_

// Define here your constants to be used all over your code.
// Always prefer a named constant instead of a "magic number"!

// GPIO lines of RGB led.
#define LINE_EXT_R                      PAL_LINE(GPIOA, 8)
#define LINE_EXT_G                      PAL_LINE(GPIOA, 9)
#define LINE_EXT_B                      PAL_LINE(GPIOA, 10)

// GPIO lines of BUZZER.
#define LINE_BUZZER                     PAL_LINE(GPIOA, 5U)

// GPIO lines of rotary encoder.
#define LINE_ENC_DT                     PAL_LINE(GPIOA, 0)
#define LINE_ENC_CLK                    PAL_LINE(GPIOA, 1)

// Use these macros to refer to TIMx PWM channels of rgb led pins
#define CHANNEL_PWM_R                   0
#define CHANNEL_PWM_G                   1
#define CHANNEL_PWM_B                   2

// Use these macros to set PAL PWM modes of rgb led inside palSetLineMode()
#define MODE_PWM_R                      PAL_MODE_ALTERNATE(1)
#define MODE_PWM_G                      PAL_MODE_ALTERNATE(1)
#define MODE_PWM_B                      PAL_MODE_ALTERNATE(1)

// This is the value to be displayed when x or y reach max value and Cartesian mode is chosen
#define JOYSTICK_CARTESIAN_MAX_VALUE    200
#define JOYSTICK_RADIUS_MAX_VALUE       1.4141 * JOYSTICK_CARTESIAN_MAX_VALUE // sqrt(maxX^2 + maxY^2)

// Whether to display coordinates in polar/cartesian representation.
#define JOY_CHOICE_CARTESIAN            0
#define JOY_CHOICE_POLAR                1

// LED service available modes.
#define LED_MODE_STATIC                 0
#define LED_MODE_DYNAMIC                1

// Possible behaviours for when LED command is in static mode.
#define LED_STATIC_ON                   1
#define LED_STATIC_OFF                  0

// Whether to display LED status or joystick status in cartesian mode.
#define OLED_MODE_LED                   0
#define OLED_MODE_JOY                   1

// Use this macros to check how buzzer should behave
#define BUZZER_MODE_ON                  1
#define BUZZER_MODE_OFF                 0

// These will determine whether led should be driven using encoder
#define DIMMER_MODE_ON                  1
#define DIMMER_MODE_OFF                 0

// Each of the following macro is used to determine how the entire system should behave in a certain moment
#define SERVICE_LED                     0
#define SERVICE_JOY                     1
#define SERVICE_OLED                    2
#define SERVICE_BUZZ                    3
#define SERVICE_DIMMER                  4
#define SERVICE_DEMO                    5
#define SERVICE_NONE                    6 // It means: Nothing should be executing.

#endif /* CUSTOM_INCLUDE_CONSTANTS_H_ */
