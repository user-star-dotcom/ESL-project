#include <stdbool.h>
#include <stdint.h>
#include "led.h"

static const uint8_t     DEVICE_ID_SEQUENCE[] = {7, 2, 0, 6};
static const led_color_t DEVICE_COLORS[]      = {LED_RED, LED_BLUE, LED_RED, LED_GREEN};

int main(void) {
    led_control_init();

    led_sequence_init(DEVICE_COLORS, DEVICE_ID_SEQUENCE, sizeof(DEVICE_ID_SEQUENCE));
    
    while (true) {
        led_sequence_run();
    }
}
