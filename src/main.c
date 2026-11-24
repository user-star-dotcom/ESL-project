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
#include "hsv.h"

//new implementation:
typedef enum {
    MODE_NONE = 0,   /* нет ввода: LED1 выключена                 */
    MODE_HUE,        /* регулировка тона: LED1 мигает медленно    */
    MODE_SAT,        /* регулировка насыщенности: LED1 мигает     */
    MODE_VAL,        /* регулировка яркости: LED1 горит постоянно */
    MODE_COUNT
} input_mode_t;
 
static input_mode_t m_mode = MODE_NONE;
 
APP_TIMER_DEF(m_tick_timer);
 
static hsv_component_t mode_to_component(input_mode_t mode)
{
    switch (mode) {
    case MODE_HUE: return HSV_HUE;
    case MODE_SAT: return HSV_SAT;
    case MODE_VAL: return HSV_VAL;
    default:       return HSV_COMP_COUNT;   /* MODE_NONE */
    }
}
//1.2
/* Индикация режима на LED1: выкл / 1 Гц / 5 Гц / постоянно */
static void mode_led_update(void)
{
    static uint16_t blink_cnt;
 
    switch (m_mode) {
    case MODE_NONE:
        led_mode_set(false);
        blink_cnt = 0;
        break;
 
    case MODE_VAL:
        led_mode_set(true);
        blink_cnt = 0;
        break;
 
    case MODE_HUE:
        led_mode_set(blink_cnt < BLINK_SLOW_HALF_TICKS);
        if (++blink_cnt >= 2 * BLINK_SLOW_HALF_TICKS) {
            blink_cnt = 0;
        }
        break;
 
    case MODE_SAT:
        led_mode_set(blink_cnt < BLINK_FAST_HALF_TICKS);
        if (++blink_cnt >= 2 * BLINK_FAST_HALF_TICKS) {
            blink_cnt = 0;
        }
        break;
 
    default:
        break;
    }
}
//2
static void tick_handler(void * p_context)
{
    (void)p_context;
 
    button_update();
 
    hsv_component_t comp = mode_to_component(m_mode);
    if (comp != HSV_COMP_COUNT) {
        hsv_ramp(comp, button_is_pressed());
    }
    hsv_apply();
 
    mode_led_update();
}

static void on_button_double_click(void)
{
    m_mode = (input_mode_t)((m_mode + 1) % MODE_COUNT);
 
    static char * const mode_names[] = {
        "none", "hue", "saturation", "value",
    };
    NRF_LOG_INFO("Input mode: %s", NRF_LOG_PUSH(mode_names[m_mode]));
}
//end of new implementation

static void clocks_start(void)//нужно
{
    if (!nrf_drv_clock_init_check()) {
        APP_ERROR_CHECK(nrf_drv_clock_init());
    }

    nrf_drv_clock_lfclk_request(NULL);
    while (!nrf_drv_clock_lfclk_is_running()) {
    }
}

static void logs_init(void)//нужно
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void)//new main
{
    clocks_start();
    logs_init();
 
    APP_ERROR_CHECK(app_timer_init());
    APP_ERROR_CHECK(app_timer_create(&m_tick_timer,
                                     APP_TIMER_MODE_REPEATED,
                                     tick_handler));
    APP_ERROR_CHECK(app_timer_start(m_tick_timer,
                                    APP_TIMER_TICKS(APP_TICK_MS),
                                    NULL));
 
    led_init();
    led_mode_init();
    button_init();
    button_set_double_click_handler(on_button_double_click);
    hsv_init();
 
    NRF_LOG_INFO("ESL HSV project started (device ID %s)",
                 NRF_LOG_PUSH(DEVICE_ID_STRING));
 
    for (;;) {
        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}
