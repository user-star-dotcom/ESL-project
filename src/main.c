#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "led.h"
#include "button.h"

static const led_color_t DEVICE_COLORS[]      = {LED_RED, LED_BLUE, LED_RED, LED_GREEN};
static const uint8_t     DEVICE_ID_SEQUENCE[] = {7,       2,        0,         6};

int main(void) {
    if (!led_control_init()) {
        while (true);
    }
    button_init();

    if (!led_sequence_init(DEVICE_COLORS, DEVICE_ID_SEQUENCE, sizeof(DEVICE_ID_SEQUENCE) / sizeof(DEVICE_ID_SEQUENCE[0]))) {
        while (true);
    }

    while (true) {
        bool pressed = button_is_pressed();
        
        led_sequence_step_next(pressed);

        nrf_delay_ms(10);
    }
}
