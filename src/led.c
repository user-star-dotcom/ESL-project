#include "led.h"
#include "boards.h"
#include "nrf_delay.h"

#define MAX_LEDS              4
#define BLINK_DELAY_MS        200
#define PAUSE_BETWEEN_LEDS_MS 800
#define SEQUENCE_PAUSE_MS     1500

static uint8_t m_blink_counts[MAX_LEDS];
static uint8_t m_num_leds = 0;

void led_sequence_init(const uint8_t *counts, uint8_t size)
{
    bsp_board_init(BSP_INIT_LEDS);

    m_num_leds = (size < MAX_LEDS) ? size : MAX_LEDS;
    for (uint8_t i = 0; i < m_num_leds; i++)
    {
        m_blink_counts[i] = counts[i];
    }
}

void led_sequence_step(void)
{
    for (uint8_t i = 0; i < m_num_leds; i++)
    {
        if (m_blink_counts[i] == 0)
        {
            continue;
        }


        for (uint8_t b = 0; b < m_blink_counts[i]; b++)
        {
            bsp_board_led_on(i);
            nrf_delay_ms(BLINK_DELAY_MS);
            bsp_board_led_off(i);
            nrf_delay_ms(BLINK_DELAY_MS);
        }

        nrf_delay_ms(PAUSE_BETWEEN_LEDS_MS);
    }

    nrf_delay_ms(SEQUENCE_PAUSE_MS);
}