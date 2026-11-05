#include "led.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define MAX_SEQUENCE_LENGTH   10
#define BLINK_DELAY_MS        250
#define PAUSE_BETWEEN_LEDS_MS 600

// Аппаратные пины (настройте под схему вашей платы, без boards.h)
#define PIN_LED_RED    NRF_GPIO_PIN_MAP(0, 8)
#define PIN_LED_GREEN  NRF_GPIO_PIN_MAP(1, 9)
#define PIN_LED_BLUE   NRF_GPIO_PIN_MAP(0, 12)

static led_color_t m_sequence_colors[MAX_SEQUENCE_LENGTH];
static uint8_t     m_sequence_counts[MAX_SEQUENCE_LENGTH];
static uint8_t     m_sequence_size = 0;
static uint8_t     m_current_index = 0;

static uint32_t get_pin_number(led_color_t color) {
    switch (color) {
        case LED_RED:   return PIN_LED_RED;
        case LED_GREEN: return PIN_LED_GREEN;
        case LED_BLUE:  return PIN_LED_BLUE;
        default:        return PIN_LED_RED;
    }
}

bool led_control_init(void) {
    nrf_gpio_cfg_output(PIN_LED_RED);
    nrf_gpio_cfg_output(PIN_LED_GREEN);
    nrf_gpio_cfg_output(PIN_LED_BLUE);

    // Безопасное выключенное состояние (Active Low)
    led_off(LED_RED);
    led_off(LED_GREEN);
    led_off(LED_BLUE);

    return true;
}

void led_on(led_color_t color) {
    if (color < LED_COLOR_MAX) {
        nrf_gpio_pin_clear(get_pin_number(color)); // Active Low
    }
}

void led_off(led_color_t color) {
    if (color < LED_COLOR_MAX) {
        nrf_gpio_pin_set(get_pin_number(color));   // Active Low
    }
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
    return true;
}

void led_sequence_reset(void) {
    m_current_index = 0;
}
// Добавляем статическую переменную для хранения текущего количества миганий конкретного цвета
//static uint8_t m_current_index     = 0; // Индекс текущего цвета в массиве
static uint8_t m_current_blink_cnt = 0; // Сколько раз уже мигнул текущий цвет (cnt)

bool led_sequence_step_next(void) {
    if (m_sequence_size == 0) {
        return true;
    }

    // Защита: если у текущего цвета 0 миганий, пропускаем его и сбрасываем счетчик
    while (m_sequence_counts[m_current_index] == 0) {
        m_current_index++;
        if (m_current_index >= m_sequence_size) {
            m_current_index = 0;
        }
        m_current_blink_cnt = 0;
    }

    led_color_t color = m_sequence_colors[m_current_index];
    uint8_t target_count = m_sequence_counts[m_current_index];

    // Выполняем ровно ОДИН blink за одно нажатие / один вызов функции
    led_on(color);
    nrf_delay_ms(BLINK_DELAY_MS);
    led_off(color);
    nrf_delay_ms(BLINK_DELAY_MS);

    // Увеличиваем счетчик выполненных миганий для этого цвета (cnt++)
    m_current_blink_cnt++;

    // Проверяем: если отмигали нужное количество раз (достигли count)
    if (m_current_blink_cnt >= target_count) {
        m_current_blink_cnt = 0; // Сбрасываем счетчик для следующего цвета
        m_current_index++;       // Переходим к следующему цвету в цепочке
        
        // Закольцовываем всю последовательность, если дошли до конца
        if (m_current_index >= m_sequence_size) {
            m_current_index = 0;
            return true; // Вся последовательность завершена один раз
        }
    }

    return false;
}
