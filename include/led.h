#ifndef LED_H
#define LED_H

#include <stdint.h>

typedef enum {
    LED_RED = 0,
    LED_GREEN,
    LED_BLUE,
    LED_COLOR_MAX
} led_color_t;

void led_init(void);

void led_set_duty(led_color_t color, uint8_t percent);

void led_all_off(void);

#endif /* LED_H */
