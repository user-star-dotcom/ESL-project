#include "led.h"

#include "board_config.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "nrfx_pwm.h"
#include "nrf_gpio.h"

static nrfx_pwm_t m_pwm = NRFX_PWM_INSTANCE(LED_PWM_INSTANCE);

static uint16_t m_compare[NRF_PWM_CHANNEL_COUNT];

//from ws4 below

static nrf_pwm_sequence_t const m_seq = {//d
    .values.p_common = m_compare,
    .length          = NRF_PWM_VALUES_LENGTH(m_compare),
    .repeats         = 0,
    .end_delay       = 0,
};

void led_init(void)//d
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



void led_all_off(void)//d
{
    for (uint8_t i = 0; i < NRF_PWM_CHANNEL_COUNT; i++) {
        m_compare[i] = 0;
    }
}

//fromws4 above

//led.c +
void led_set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    m_compare[LED_RED] = (uint16_t)(r * LED_PWM_TOP_VALUE / 255);
    m_compare[LED_GREEN] = (uint16_t)(g * LED_PWM_TOP_VALUE / 255);
    m_compare[LED_BLUE] = (uint16_t)(b * LED_PWM_TOP_VALUE / 255);
}
 
void led_mode_init(void)
{
    nrf_gpio_cfg_output(LED_MODE_PIN);
    led_mode_set(false);
}
 
void led_mode_set(bool on)
{
    uint32_t level = on ? LED_MODE_ON_LEVEL : !LED_MODE_ON_LEVEL;
    nrf_gpio_pin_write(LED_MODE_PIN, level);
}
//led.c +
