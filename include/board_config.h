#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "nrf_gpio.h"

#define LED_PIN_RED     NRF_GPIO_PIN_MAP(0, 8)
#define LED_PIN_GREEN   NRF_GPIO_PIN_MAP(1, 9)
#define LED_PIN_BLUE    NRF_GPIO_PIN_MAP(0, 12)

#define LED_MODE_PIN        NRF_GPIO_PIN_MAP(0, 6)
#define LED_MODE_ON_LEVEL   0

#define BUTTON_PIN              NRF_GPIO_PIN_MAP(1, 6)
#define BUTTON_PRESSED_LEVEL    0

#define LED_PWM_INSTANCE        0
#define LED_PWM_TOP_VALUE       1000    

#define APP_TICK_MS             10
 
/* LED1 */
#define BLINK_SLOW_HALF_TICKS   50
#define BLINK_FAST_HALF_TICKS   10
 
/* Плавное изменение значения при удержании кнопки */
#define HSV_RAMP_TICKS          10   /* один шаг раз в 100 мс */
#define HSV_HUE_STEP            1.0f /* 1 градус за шаг*/
#define HSV_SV_STEP             1.0f /* 1 процент за шаг*/
 
/* Начальный Hue: последние две цифры Device ID как проценты */
#define DEVICE_ID_STRING        "7206"
#define DEVICE_ID_HUE_PCT       6    /* 06 -> 6 % -> 360 * 0.06 = 21.6 гр */
 
#define BUTTON_DEBOUNCE_MS      30 
#define BUTTON_DEBOUNCE_TICKS   3
#define DOUBLE_CLICK_WINDOW_MS  400 

#endif /* BOARD_CONFIG_H */
