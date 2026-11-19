/**
 * @file led.h
 *
 * LED HAL for the on-board RGB LED.
 *
 * The LEDs are driven by a hardware PWM peripheral (nrfx_pwm): the driver
 * plays an endless sequence from a RAM buffer, so the duty cycle of any
 * channel can be changed at any time by a simple RAM write.
 */
#ifndef LED_H
#define LED_H

#include <stdint.h>

/** LED colors. The enum order is also the PWM channel order (see led.c). */
typedef enum {
    LED_RED = 0,
    LED_GREEN,
    LED_BLUE,
    LED_COLOR_MAX
} led_color_t;

/**
 * @brief Configure the PWM peripheral and start the endless playback.
 *
 * All channels start at 0 % (LEDs off).
 */
void led_init(void);

/**
 * @brief Set the duty cycle of one LED.
 *
 * @param color   LED to update.
 * @param percent Duty cycle 0..100; 0 means "off", 100 means full brightness.
 *                Values above 100 are clamped.
 */
void led_set_duty(led_color_t color, uint8_t percent);

/** @brief Set the duty cycle of every LED to 0 (all off). */
void led_all_off(void);

#endif /* LED_H */
