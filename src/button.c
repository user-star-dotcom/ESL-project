#include "button.h"

#include "board_config.h"
#include "app_error.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

static button_double_click_handler_t m_double_click_handler;
static button_single_click_handler_t m_single_click_handler;

APP_TIMER_DEF(m_debounce_timer);
APP_TIMER_DEF(m_click_timer); //for second click

static bool m_pressed;
static bool m_wait_second_click;

static bool pin_is_pressed_raw(void)
{
    return nrf_gpio_pin_read(BUTTON_PIN) == BUTTON_PRESSED_LEVEL;
}

static void debounce_timeout_handler(void * p_context)
{
    (void)p_context;

    bool pressed = pin_is_pressed_raw();
    if (pressed == m_pressed) {
        return;//if state of btn didn`t change
    }
    m_pressed = pressed;

    if (!pressed) {
        return;
    }

    if (m_wait_second_click) {
        m_wait_second_click = false;
        (void)app_timer_stop(m_click_timer);

        if (m_double_click_handler != NULL) {//<--------------------------------------??
            m_double_click_handler();
        }
        return;
    }

    //preparing for the second:
    m_wait_second_click = true;
    (void)app_timer_start(m_click_timer,
                          APP_TIMER_TICKS(DOUBLE_CLICK_WINDOW_MS), NULL);
}

static void click_timeout_handler(void * p_context)
{
    (void)p_context;

    m_wait_second_click = false;

    if (!pin_is_pressed_raw() && m_single_click_handler != NULL) {
        m_single_click_handler();
    }
}

static void gpiote_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    (void)pin;
    (void)action;

    (void)app_timer_stop(m_debounce_timer);
    (void)app_timer_start(m_debounce_timer,
                          APP_TIMER_TICKS(BUTTON_DEBOUNCE_MS), NULL);
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

    APP_ERROR_CHECK(app_timer_create(&m_debounce_timer,
                                     APP_TIMER_MODE_SINGLE_SHOT,
                                     debounce_timeout_handler));
    APP_ERROR_CHECK(app_timer_create(&m_click_timer,
                                     APP_TIMER_MODE_SINGLE_SHOT,
                                     click_timeout_handler));

    m_pressed = pin_is_pressed_raw();
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
