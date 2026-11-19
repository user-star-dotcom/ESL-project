/**
 * @file board_config.h
 *
 * Single source of truth for everything board-specific: LED/button pins,
 * PWM parameters, sequence timing and the device identity.
 *
 * This file replaces boards.h from the SDK (Workshop 3 requirement):
 * the project must not include it.
 */
#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "nrf_gpio.h"

/* ================= Board: nRF52840 Dongle (PCA10059) ================= */

/* RGB LED1 is common anode, so the PWM outputs are configured inverted:
 * compare value 0 = LED off, compare value TOP = full brightness.
 * PWM channel order used by the LED driver matches the led_color_t enum:
 * ch0 = RED, ch1 = GREEN, ch2 = BLUE (see led.c). */
#define LED_PIN_RED     NRF_GPIO_PIN_MAP(0, 8)
#define LED_PIN_GREEN   NRF_GPIO_PIN_MAP(1, 9)
#define LED_PIN_BLUE    NRF_GPIO_PIN_MAP(0, 12)

/* SW1 pulls the line low while pressed (external pull-up on the board). */
#define BUTTON_PIN              NRF_GPIO_PIN_MAP(1, 6)
#define BUTTON_PRESSED_LEVEL    0

/* ============================== PWM ============================== */

#define LED_PWM_INSTANCE        0
#define LED_PWM_TOP_VALUE       1000    /* 1 MHz base clock / 1000 = 1 kHz PWM */
#define BREATH_DUTY_MAX_PCT     100     /* breathing ramps 0..100 % */

/* Duty step per app tick: 1 % / 10 ms -> 1 s up + 1 s down = 2 s per breath. */
#define BREATH_DUTY_STEP_PCT    1

/* ========================== Application tick ========================== */

#define APP_TICK_MS             10
#define HEARTBEAT_TICKS         500     /* one log message every 5 s */

/* ========================== Sequence timing ========================== */

#define STEP_PAUSE_TICKS        50      /* 500 ms pause between sequence steps */
#define ZERO_DIGIT_PAUSE_TICKS  100     /* 1 s silence marking a '0' digit */

/* ========================== Button tuning ========================== */

#define BUTTON_DEBOUNCE_MS      30      /* GPIOTE edge guard window */
#define BUTTON_DEBOUNCE_TICKS   3       /* stable app ticks before the level is accepted */
#define DOUBLE_CLICK_WINDOW_MS  400     /* max time between two presses */

/* ========================= Device identity ========================= */

#define DEVICE_ID_STRING        "#7206"

#endif /* BOARD_CONFIG_H */
