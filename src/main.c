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

/* Device ID #7206 - same interpretation as in the previous workshops:
 * RED 7 breaths -> BLUE 2 -> RED 0 (silence marker) -> GREEN 6, then loop. */
static sequence_step_t const m_device_sequence[] = {
    {LED_RED,   7},
    {LED_BLUE,  2},
    {LED_RED,   0},     /* '0' digit: rendered as a silence marker */
    {LED_GREEN, 6},
};

APP_TIMER_DEF(m_tick_timer);

static void tick_handler(void * p_context)
{
    (void)p_context;

    button_update();
    sequence_tick(button_is_pressed());

    /* Low-rate heartbeat. Logs are only transmitted while a terminal has the
     * COM port open (see nrf_log_backend_usb.c), so a periodic message makes
     * the log pipeline observable at any time - same idea as the "Iter %d"
     * loop in examples/peripheral/dongle_logs. */
    static uint16_t heartbeat;
    if (++heartbeat >= HEARTBEAT_TICKS) {
        heartbeat = 0;
        NRF_LOG_INFO("Heartbeat: sequence running %u", (unsigned)sequence_is_running());
    }
}

static void on_button_double_click(void)
{
    sequence_toggle_running();
    NRF_LOG_INFO(sequence_is_running() ?
                 "Double click: sequence started" :
                 "Double click: sequence frozen");
}

static void clocks_start(void)
{
    /* nrf_drv_clock must be initialized before any request; the USB log
     * backend does the same dance, both are idempotent. */
    if (!nrf_drv_clock_init_check()) {
        APP_ERROR_CHECK(nrf_drv_clock_init());
    }

    /* LFCLK is required by app_timer (RTC1) and by the USB log backend. */
    nrf_drv_clock_lfclk_request(NULL);
    while (!nrf_drv_clock_lfclk_is_running()) {
        /* wait until the low frequency clock starts */
    }
}

static void logs_init(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    /* With LOG_BACKEND_USB_INIT_STACK=1 this also initializes the USB stack,
     * registers the CDC ACM class and waits until USBD is started. */
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void)
{
    clocks_start();
    logs_init();

    APP_ERROR_CHECK(app_timer_init());
    APP_ERROR_CHECK(app_timer_create(&m_tick_timer, APP_TIMER_MODE_REPEATED, tick_handler));
    APP_ERROR_CHECK(app_timer_start(m_tick_timer, APP_TIMER_TICKS(APP_TICK_MS), NULL));

    //
    NRF_LOG_INFO("hi");
    //

    led_init();
    button_init();
    button_set_double_click_handler(on_button_double_click);
    sequence_init(m_device_sequence,
                  sizeof(m_device_sequence) / sizeof(m_device_sequence[0]));

    NRF_LOG_INFO("ESL PWM project started (device ID %s)", NRF_LOG_PUSH(DEVICE_ID_STRING));

    for (;;) {
        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}
