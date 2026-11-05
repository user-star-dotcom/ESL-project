#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    LED_RED = 0,
    LED_GREEN,
    LED_BLUE,
    LED_COLOR_MAX
} led_color_t;

bool led_control_init(void);
bool led_sequence_init(const led_color_t *colors, const uint8_t *counts, uint8_t size);
void led_sequence_reset(void);

/**
 * @brief Выполнить шаг последовательности с учетом состояния кнопки
 * @param is_button_pressed Текущее состояние кнопки (нажата/отпущена)
 * @return true, если цикл завершен
 */
bool led_sequence_step_next(bool is_button_pressed);

void led_on(led_color_t color);
void led_off(led_color_t color);
void led_toggle(led_color_t color);

#endif // LED_H
