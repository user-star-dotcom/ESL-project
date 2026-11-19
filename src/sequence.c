/**
 * @file sequence.c
 *
 * Breathing-sequence state machine.
 *
 * Each step plays `digit` breaths (duty ramps 0 -> 100 % -> 0) on one LED,
 * then the player pauses for STEP_PAUSE_TICKS and moves to the next step.
 * A digit of 0 is rendered as ZERO_DIGIT_PAUSE_TICKS of silence instead of
 * being skipped instantly, so the "0" of the device ID stays visible in the
 * rhythm. After the last step the sequence wraps around and keeps looping
 * while it is running and the button is pressed.
 */
#include "sequence.h"

#include "board_config.h"
#include "nrf_log.h"

/** Phases inside one pass of the state machine. */
typedef enum {
    PHASE_BREATH,       /* duty is ramping up/down on the active LED */
    PHASE_STEP_PAUSE,   /* pause after a step finished all its breaths */
    PHASE_ZERO_PAUSE,   /* silence marker replacing a '0' digit */
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

    NRF_LOG_INFO("Step %u: color %u, digit %u",
                 (unsigned)m_step,
                 (unsigned)m_steps[m_step].color,
                 (unsigned)digit);
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
    /* Frozen: duty changes only while the sequence is running AND the button
     * is pressed. The PWM keeps playing the last written duty, so the LED
     * stays exactly as bright as it was - also after a double-click freeze. */
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
            /* one breath (up + down) finished */
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
            m_step = (uint8_t)((m_step + 1) % m_step_count);    /* loop */
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
