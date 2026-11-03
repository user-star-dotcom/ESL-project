#ifndef LED_H
#define LED_H

#include <stdint.h>

/**
 * @brief Перечисление цветов светодиодов
 */
typedef enum {
    LED_RED,
    LED_GREEN,
    LED_BLUE
} led_color_t;

/**
 * @brief Инициализация аппаратной части (GPIO пинов)
 */
void led_control_init(void);

/**
 * @brief Инициализация последовательности миганий
 * @param colors Массив цветов
 * @param counts Массив количеств миганий
 * @param size Размер массивов
 */
void led_sequence_init(const led_color_t *colors, const uint8_t *counts, uint8_t size);

/**
 * @brief Запустить воспроизведение всей последовательности (в цикле или один проход)
 */
void led_sequence_run(void);

// Базовые функции управления
void led_on(led_color_t color);
void led_off(led_color_t color);
void led_toggle(led_color_t color);

#endif // LED_H
