#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Перечисление доступных цветов светодиодов
 */
typedef enum {
    LED_RED = 0,
    LED_GREEN,
    LED_BLUE,
    LED_COLOR_MAX
} led_color_t;

/**
 * @brief Инициализация аппаратной части (GPIO пинов светодиодов)
 */
bool led_control_init(void);

/**
 * @brief Инициализация последовательности миганий
 * @param colors Массив цветов последовательности
 * @param counts Массив количеств миганий для каждого цвета
 * @param size Размер массивов
 */
bool led_sequence_init(const led_color_t *colors, const uint8_t *counts, uint8_t size);

/**
 * @brief Выполнить один полный шаг (мигание для текущего цвета) и перейти к следующему
 * @return true, если пройдена вся последовательность до конца, иначе false
 */
bool led_sequence_step_next(void);

/**
 * @brief Сбросить текущий индекс последовательности на начало
 */
void led_sequence_reset(void);

// Базовые низкоуровневые функции
void led_on(led_color_t color);
void led_off(led_color_t color);
void led_toggle(led_color_t color);

#endif // LED_H
