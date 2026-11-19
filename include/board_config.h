#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "nrf_gpio.h"

#define LED_PIN_RED     NRF_GPIO_PIN_MAP(0, 8)
#define LED_PIN_GREEN   NRF_GPIO_PIN_MAP(1, 9)
#define LED_PIN_BLUE    NRF_GPIO_PIN_MAP(0, 12)

#define BUTTON_PIN              NRF_GPIO_PIN_MAP(1, 6)
#define BUTTON_PRESSED_LEVEL    0


#define LED_PWM_INSTANCE        0
#define LED_PWM_TOP_VALUE       1000    
#define BREATH_DUTY_MAX_PCT     100 

#define BREATH_DUTY_STEP_PCT    1

#define APP_TICK_MS             10
#define BEAT_TICKS         500


#define STEP_PAUSE_TICKS        50 
#define ZERO_DIGIT_PAUSE_TICKS  100 


#define BUTTON_DEBOUNCE_MS      30 
#define BUTTON_DEBOUNCE_TICKS   3
#define DOUBLE_CLICK_WINDOW_MS  400 


#define DEVICE_ID_STRING        "7206"

#endif /* BOARD_CONFIG_H */
