#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

/**
 * @brief Инициализация пина кнопки
 */
void button_init(void);

/**
 * @brief Проверка состояния нажатия кнопки
 * @return true, если кнопка нажата, иначе false
 */
bool button_is_pressed(void);

#endif // BUTTON_H
