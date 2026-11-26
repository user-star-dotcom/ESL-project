#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

typedef void (* button_double_click_handler_t)(void);

typedef void (* button_single_click_handler_t)(void);

void button_init(void);

bool button_is_pressed(void);

void button_set_double_click_handler(button_double_click_handler_t handler);

void button_set_single_click_handler(button_single_click_handler_t handler);

#endif /* BUTTON_H */
