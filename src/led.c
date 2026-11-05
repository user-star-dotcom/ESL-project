#include "led.h"
#include "button.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define MAX_SEQUENCE_LENGTH   10
#define BLINK_DELAY_MS        250
#define PAUSE_BETWEEN_LEDS_MS 600

#define PIN_LED_RED    NRF_GPIO_PIN_MAP(0, 8)
#define PIN_LED_GREEN  NRF_GPIO_PIN_MAP(1, 9)
#define PIN_LED_BLUE   NRF_GPIO_PIN_MAP(0, 12)

static led_color_t m_sequence_colors[MAX_SEQUENCE_LENGTH];
static uint8_t     m_sequence_counts[MAX_SEQUENCE_LENGTH];
static uint8_t     m_sequence_size = 0;
static uint8_t     m_current_index = 0;
static uint8_t     m_current_blink_cnt = 0;
static bool        m_paused_on = false;

static uint32_t get_pin_number(led_color_t color) {
    switch (color) {
        case LED_RED:   return PIN_LED_RED;
        case LED_GREEN: return PIN_LED_GREEN;
        case LED_BLUE:  return PIN_LED_BLUE;
        default:        return PIN_LED_RED;
    }
}

// Низкоуровневое включение светодиода (Active Low: 0 = ON)
void led_on(led_color_t color) {
    if (color < LED_COLOR_MAX) {
        nrf_gpio_pin_clear(get_pin_number(color));
    }
}

// Низкоуровневое выключение светодиода (Active Low: 1 = OFF)
void led_off(led_color_t color) {
    if (color < LED_COLOR_MAX) {
        nrf_gpio_pin_set(get_pin_number(color));
    }
}

static void led_all_off(void) {
    led_off(LED_RED);
    led_off(LED_GREEN);
    led_off(LED_BLUE);
}

bool led_control_init(void) {
    // 1. Сначала выключаем все (ставим 1), чтобы избежать вспышек
    led_all_off();

    // 2. Настраиваем пины как выходы
    nrf_gpio_cfg_output(PIN_LED_RED);
    nrf_gpio_cfg_output(PIN_LED_GREEN);
    nrf_gpio_cfg_output(PIN_LED_BLUE);

    return true;
}

void led_toggle(led_color_t color) {
    if (color < LED_COLOR_MAX) {
        nrf_gpio_pin_toggle(get_pin_number(color));
    }
}

bool led_sequence_init(const led_color_t *colors, const uint8_t *counts, uint8_t size) {
    if (colors == NULL || counts == NULL || size == 0 || size > MAX_SEQUENCE_LENGTH) {
        return false;
    }

    m_sequence_size = size;
    for (uint8_t i = 0; i < m_sequence_size; i++) {
        m_sequence_colors[i] = colors[i];
        m_sequence_counts[i] = counts[i];
    }
    m_current_index = 0;
    m_current_blink_cnt = 0;
    m_paused_on = false;
    
    led_all_off();
    return true;
}

void led_sequence_reset(void) {
    m_current_index = 0;
    m_current_blink_cnt = 0;
    m_paused_on = false;
    led_all_off();
}

bool led_sequence_step_next(bool is_button_pressed) {
    if (m_sequence_size == 0) {
        return true;
    }

    led_color_t color = m_sequence_colors[m_current_index];

    if (m_paused_on) {
        if (is_button_pressed) {
            led_all_off();
            m_paused_on = false;
            
            m_current_blink_cnt++;
            if (m_current_blink_cnt >= m_sequence_counts[m_current_index]) {
                m_current_blink_cnt = 0;
                m_current_index++;
                if (m_current_index >= m_sequence_size) {
                    m_current_index = 0;
                    return true;
                }
            }
            nrf_delay_ms(PAUSE_BETWEEN_LEDS_MS);
        } else {
            led_on(color);
        }
        return false;
    }

    if (!is_button_pressed) {
        led_all_off();
        return false;
    }

    while (m_sequence_counts[m_current_index] == 0) {
        m_current_index++;
        if (m_current_index >= m_sequence_size) {
            m_current_index = 0;
        }
        m_current_blink_cnt = 0;
    }

    color = m_sequence_colors[m_current_index];
    uint8_t target_count = m_sequence_counts[m_current_index];


    led_all_off();
    led_on(color);

    uint32_t elapsed = 0;
    while (elapsed < BLINK_DELAY_MS) {
        nrf_delay_ms(10);
        elapsed += 10;
        
        if (!button_is_pressed()) {
            m_paused_on = true;
            led_on(color);
            return false;
        }
    }


    led_off(color);
    nrf_delay_ms(BLINK_DELAY_MS);

    m_current_blink_cnt++;
    if (m_current_blink_cnt >= target_count) {
        m_current_blink_cnt = 0;
        m_current_index++;
        if (m_current_index >= m_sequence_size) {
            m_current_index = 0;
            return true;
        }
        nrf_delay_ms(PAUSE_BETWEEN_LEDS_MS);
    }

    return false;
}
