#include "button.h"
#include "nrf_gpio.h"

// Пин кнопки (уточните по схеме вашей платы, например 13)
#define PIN_BUTTON NRF_GPIO_PIN_MAP(1, 6)

void button_init(void) {
    // Настраиваем пин на вход с подтяжкой к питанию (Pull-up)
    nrf_gpio_cfg_input(PIN_BUTTON, NRF_GPIO_PIN_PULLUP);
}

bool button_is_pressed(void) {
    // При Pull-up нажатие дает уровень LOW (0)
    return (nrf_gpio_pin_read(PIN_BUTTON) == 0);
}
