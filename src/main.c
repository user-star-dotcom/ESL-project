#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "led.h"
#include "button.h"

// Конфигурация последовательности на основе Device ID
static const led_color_t DEVICE_COLORS[]      = {LED_RED, LED_BLUE, LED_RED, LED_GREEN};
static const uint8_t     DEVICE_ID_SEQUENCE[] = {7,       2,        0,         6};

int main(void) {
    // Инициализация железа
    if (!led_control_init()) {
        while (true);
    }
    button_init();

    // Инициализация последовательности
    if (!led_sequence_init(DEVICE_COLORS, DEVICE_ID_SEQUENCE, sizeof(DEVICE_ID_SEQUENCE) / sizeof(DEVICE_ID_SEQUENCE[0]))) {
        while (true);
    }

    // Главный цикл программы
    while (true) {
        if (button_is_pressed()) {
            // Выполняем один шаг последовательности
            led_sequence_step_next();
        }
    }
}
