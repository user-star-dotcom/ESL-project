#include <stdbool.h>
#include <stdint.h>
#include "led.h"

static const uint8_t DEVICE_ID_SEQUENCE[] = {7, 2, 0, 6};

int main(void)
{
    led_sequence_init(DEVICE_ID_SEQUENCE, sizeof(DEVICE_ID_SEQUENCE));

    while (true)
    {
        led_sequence_step();
    }
}