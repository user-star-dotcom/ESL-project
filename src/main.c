/**
 * @file main.c
 *
 * ESL project - Workshop: PWM breathing sequence with button gating.
 *
 * Composition root only: brings up clocks/logging/timers and wires the
 * modules together. All business logic lives in the modules:
 *
 *   app_timer 10 ms tick -> button_update() + sequence_tick(button state)
 *                           -> led_set_duty() -> PWM (nrfx_pwm)
 *   GPIOTE double-click  -> button callback -> sequence_toggle_running()
 *   main loop            -> USB CDC log pumping
 */
#include <stdbool.h>
#include <stdint.h>

#include "app_error.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"

#include "board_config.h"
#include "button.h"
#include "led.h"
#include "sequence.h"

static sequence_step_t const m_device_sequence[] = {
    {LED_RED, 7},
    {LED_BLUE, 2},
    {LED_RED, 0}, 
    {LED_GREEN, 6},
};

APP_TIMER_DEF(m_tick_timer);

static void tick_handler(void * p_context)
{
    (void)p_context;

    button_update();
    sequence_tick(button_is_pressed());

    static uint16_t beat;
    if (++beat >= BEAT_TICKS) {
        beat = 0;
        NRF_LOG_INFO("State: sequence running %u", (unsigned)sequence_is_running());
    }
}

static void on_button_double_click(void)
{
    sequence_toggle_running();

    if (sequence_is_running()) {
        NRF_LOG_INFO("Double click: sequence started");
        sequence_log_current_step();
    } else {
        NRF_LOG_INFO("Double click: sequence frozen");
    }
}

static void on_button_single_click(void)
{
    NRF_LOG_INFO("Single click: sequence running %u", (unsigned)sequence_is_running());
}

static void clocks_start(void)
{

    if (!nrf_drv_clock_init_check()) {
        APP_ERROR_CHECK(nrf_drv_clock_init());
    }


    nrf_drv_clock_lfclk_request(NULL);
    while (!nrf_drv_clock_lfclk_is_running()) {
    }
}

static void logs_init(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void)
{
    clocks_start();
    logs_init();

    APP_ERROR_CHECK(app_timer_init());
    APP_ERROR_CHECK(app_timer_create(&m_tick_timer, APP_TIMER_MODE_REPEATED, tick_handler));
    APP_ERROR_CHECK(app_timer_start(m_tick_timer, APP_TIMER_TICKS(APP_TICK_MS), NULL));

    led_init();
    button_init();
    button_set_double_click_handler(on_button_double_click);
    button_set_single_click_handler(on_button_single_click);

    sequence_init(m_device_sequence,
                  sizeof(m_device_sequence) / sizeof(m_device_sequence[0]));

    NRF_LOG_INFO("ESL PWM project started (device ID %s)", NRF_LOG_PUSH(DEVICE_ID_STRING));

    for (;;) {
        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}
