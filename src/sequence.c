#include "sequence.h"

#include "board_config.h"
#include "nrf_log.h"

typedef enum {
    PHASE_BREATH,
    PHASE_STEP_PAUSE,
    PHASE_ZERO_PAUSE,
} sequence_phase_t;

static sequence_step_t const * m_steps;
static uint8_t                 m_step_count;
static uint8_t                 m_step;
static sequence_phase_t        m_phase;
static uint8_t                 m_breaths_left;
static uint8_t                 m_duty;
static int8_t                  m_ramp_dir = 1;
static uint16_t                m_ticks_left;
static bool                    m_running;

static void log_current_step(void)
{
    NRF_LOG_INFO("Step %u: color %u, digit %u",
                 (unsigned)m_step,
                 (unsigned)m_steps[m_step].color,
                 (unsigned)m_steps[m_step].digit);
}

static void enter_step(void)
{
    uint8_t const digit = m_steps[m_step].digit;

    m_duty     = 0;
    m_ramp_dir = 1;

    if (digit == 0) {
        m_phase      = PHASE_ZERO_PAUSE;
        m_ticks_left = ZERO_DIGIT_PAUSE_TICKS;
    } else {
        m_breaths_left = digit;
        m_phase        = PHASE_BREATH;
    }

    led_set_duty(m_steps[m_step].color, 0);
    log_current_step();
}

void sequence_init(sequence_step_t const * p_steps, uint8_t step_count)
{
    m_steps      = p_steps;
    m_step_count = step_count;
    m_step       = 0;
    m_running    = false;

    enter_step();
}

void sequence_tick(bool button_pressed)
{
    if (!m_running || !button_pressed) {
        return;
    }

    switch (m_phase) {
    case PHASE_BREATH:
        m_duty = (uint8_t)(m_duty + m_ramp_dir);
        if (m_duty >= BREATH_DUTY_MAX_PCT) {
            m_duty     = BREATH_DUTY_MAX_PCT;
            m_ramp_dir = -1;
        } else if (m_duty == 0) {
            m_ramp_dir = 1;
            if (--m_breaths_left == 0) {
                m_phase      = PHASE_STEP_PAUSE;
                m_ticks_left = STEP_PAUSE_TICKS;
            }
        }
        led_set_duty(m_steps[m_step].color, m_duty);
        break;

    case PHASE_STEP_PAUSE:
    case PHASE_ZERO_PAUSE:
        if (--m_ticks_left == 0) {
            m_step = (uint8_t)((m_step + 1) % m_step_count);
            enter_step();
        }
        break;
    }
}

void sequence_toggle_running(void)
{
    m_running = !m_running;
}

bool sequence_is_running(void)
{
    return m_running;
}

void sequence_log_current_step(void)
{
    log_current_step();
}
