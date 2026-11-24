#include "button.h"

#include "board_config.h"
#include "app_error.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

static button_double_click_handler_t m_double_click_handler;
static button_single_click_handler_t m_single_click_handler;

APP_TIMER_DEF(m_click_timer);


static bool    m_pressed;
static uint8_t m_unstable_ticks;

static bool     m_have_prev_press;
static uint32_t m_last_press_cnt;

static bool     m_have_edge;
static uint32_t m_last_edge_cnt;

static bool pin_is_pressed_raw(void)
{
    return nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED_LEVEL;
}

static void click_timeout_handler(void * p_context)
{
    (void)p_context;

    m_have_prev_press = false;

    if (!pin_is_pressed_raw() && m_single_click_handler != NULL) {
        m_single_click_handler();
    }
}

static void on_press_edge(void)
{
    uint32_t now = app_timer_cnt_get();

    if (m_have_prev_press &&
        app_timer_cnt_diff_compute(now, m_last_press_cnt) <
            APP_TIMER_TICKS(DOUBLE_CLICK_WINDOW_MS)) {

        (void)app_timer_stop(m_click_timer);
        m_have_prev_press = false;

        if (m_double_click_handler != NULL) {
            m_double_click_handler();
        }
        return;
    }

    m_have_prev_press = true;
    m_last_press_cnt  = now;
    (void)app_timer_start(m_click_timer, APP_TIMER_TICKS(DOUBLE_CLICK_WINDOW_MS), NULL);
}

static void gpiote_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    (void)pin;
    (void)action;

    uint32_t now = app_timer_cnt_get();
    if (m_have_edge &&
        app_timer_cnt_diff_compute(now, m_last_edge_cnt) <
            APP_TIMER_TICKS(BUTTON_DEBOUNCE_MS)) {
        return;
    }
    m_last_edge_cnt = now;
    m_have_edge     = true;

    if (pin_is_pressed_raw()) {
        on_press_edge();
    }
}

void button_init(void)
{
    if (!nrfx_gpiote_is_init()) {
        APP_ERROR_CHECK(nrfx_gpiote_init());
    }

    nrfx_gpiote_in_config_t cfg = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    cfg.pull = NRF_GPIO_PIN_PULLUP;

    APP_ERROR_CHECK(nrfx_gpiote_in_init(BUTTON_PIN, &cfg, gpiote_handler));
    nrfx_gpiote_in_event_enable(BUTTON_PIN, true);

    APP_ERROR_CHECK(app_timer_create(&m_click_timer,
                                     APP_TIMER_MODE_SINGLE_SHOT,
                                     click_timeout_handler));

    m_pressed = pin_is_pressed_raw();
}

void button_update(void)
{
    if (pin_is_pressed_raw() == m_pressed) {
        m_unstable_ticks = 0;
        return;
    }

    if (++m_unstable_ticks >= BUTTON_DEBOUNCE_TICKS) {
        m_pressed        = pin_is_pressed_raw();
        m_unstable_ticks = 0;
    }
}

bool button_is_pressed(void)
{
    return m_pressed;
}

void button_set_double_click_handler(button_double_click_handler_t handler)
{
    m_double_click_handler = handler;
}

void button_set_single_click_handler(button_single_click_handler_t handler)
{
    m_single_click_handler = handler;
}
