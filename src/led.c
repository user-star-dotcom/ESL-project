#include "led.h"

#include "board_config.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "nrfx_pwm.h"

#define LED_PWM_MAX_PCT 100

static nrfx_pwm_t m_pwm = NRFX_PWM_INSTANCE(LED_PWM_INSTANCE);

static uint16_t m_compare[NRF_PWM_CHANNEL_COUNT];

static nrf_pwm_sequence_t const m_seq = {
    .values.p_common = m_compare,
    .length          = NRF_PWM_VALUES_LENGTH(m_compare),
    .repeats         = 0,
    .end_delay       = 0,
};

void led_init(void)
{
    nrfx_pwm_config_t const cfg = {
        .output_pins = {
            LED_PIN_RED   | NRFX_PWM_PIN_INVERTED,
            LED_PIN_GREEN | NRFX_PWM_PIN_INVERTED,
            LED_PIN_BLUE  | NRFX_PWM_PIN_INVERTED,
            NRFX_PWM_PIN_NOT_USED,
        },
        .irq_priority = APP_IRQ_PRIORITY_LOW,
        .base_clock   = NRF_PWM_CLK_1MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = LED_PWM_TOP_VALUE,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO,
    };

    APP_ERROR_CHECK(nrfx_pwm_init(&m_pwm, &cfg, NULL));

    led_all_off();

    APP_ERROR_CHECK(nrfx_pwm_simple_playback(&m_pwm, &m_seq, 1, NRFX_PWM_FLAG_LOOP));
}

void led_set_duty(led_color_t color, uint8_t percent)
{
    if (color >= LED_COLOR_MAX) {
        return;
    }
    if (percent > LED_PWM_MAX_PCT) {
        percent = LED_PWM_MAX_PCT;
    }

    m_compare[(uint8_t)color] =
        (uint16_t)((percent * LED_PWM_TOP_VALUE) / LED_PWM_MAX_PCT);
}

void led_all_off(void)
{
    for (uint8_t i = 0; i < NRF_PWM_CHANNEL_COUNT; i++) {
        m_compare[i] = 0;
    }
}
