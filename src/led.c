#include "led.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define MAX_LEDS               4
#define BLINK_DELAY_MS         300
#define PAUSE_BETWEEN_LEDS_MS  800
#define SEQUENCE_PAUSE_MS      1800

// Аппаратные пины
#define PIN_LED_RED    NRF_GPIO_PIN_MAP(0, 8)
#define PIN_LED_GREEN  NRF_GPIO_PIN_MAP(1, 9)
#define PIN_LED_BLUE   NRF_GPIO_PIN_MAP(0, 12)

// Инкапсулированное состояние модуля
static led_color_t m_sequence_colors[MAX_LEDS];
static uint8_t     m_blink_counts[MAX_LEDS];
static uint8_t     m_num_leds = 0;

static uint32_t get_pin_number(led_color_t color) {
    switch (color) {
        case LED_RED:   return PIN_LED_RED;
        case LED_GREEN: return PIN_LED_GREEN;
        case LED_BLUE:  return PIN_LED_BLUE;
        default:        return PIN_LED_RED;
    }
}

void led_control_init(void) {
    nrf_gpio_cfg_output(PIN_LED_RED);
    nrf_gpio_cfg_output(PIN_LED_GREEN);
    nrf_gpio_cfg_output(PIN_LED_BLUE);

    // Безопасное состояние по умолчанию (выключено)
    led_off(LED_RED);
    led_off(LED_GREEN);
    led_off(LED_BLUE);
}

void led_on(led_color_t color) {
    nrf_gpio_pin_clear(get_pin_number(color)); // Active Low
}

void led_off(led_color_t color) {
    nrf_gpio_pin_set(get_pin_number(color));   // Active Low
}

void led_toggle(led_color_t color) {
    nrf_gpio_pin_toggle(get_pin_number(color));
}

void led_sequence_init(const led_color_t *colors, const uint8_t *counts, uint8_t size) {
    m_num_leds = (size < MAX_LEDS) ? size : MAX_LEDS;
    for (uint8_t i = 0; i < m_num_leds; i++) {
        m_sequence_colors[i] = colors[i];
        m_blink_counts[i] = counts[i];
    }
}

void led_sequence_run(void) {
    if (m_num_leds == 0) {
        return;
    }

    for (uint8_t i = 0; i < m_num_leds; i++) {
        uint8_t count = m_blink_counts[i];
        if (count == 0) {
            nrf_delay_ms(PAUSE_BETWEEN_LEDS_MS);
            continue;
        }

        led_color_t color = m_sequence_colors[i];

        for (uint8_t b = 0; b < count; b++) {
            led_on(color);
            nrf_delay_ms(BLINK_DELAY_MS);
            led_off(color);
            nrf_delay_ms(BLINK_DELAY_MS);
        }

        nrf_delay_ms(PAUSE_BETWEEN_LEDS_MS);
    }

    nrf_delay_ms(SEQUENCE_PAUSE_MS);
}
