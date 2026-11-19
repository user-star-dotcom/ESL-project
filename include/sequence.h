/**
 * @file sequence.h
 *
 * Breathing-sequence player.
 *
 * A pure state machine: it owns no hardware except the led HAL and is fully
 * driven by sequence_tick(), so the whole logic is non-blocking and can be
 * frozen/resumed at any point (button released, double-click) without losing
 * its position.
 */
#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdbool.h>
#include <stdint.h>

#include "led.h"

/** One step of the sequence: which LED and how many breaths (its digit). */
typedef struct {
    led_color_t color;
    uint8_t     digit;      /* number of breaths; 0 = silence marker */
} sequence_step_t;

/**
 * @brief Load the sequence and reset the state machine.
 *
 * The player starts stopped (LEDs off); nothing happens until
 * sequence_toggle_running() is called.
 */
void sequence_init(sequence_step_t const * p_steps, uint8_t step_count);

/**
 * @brief Advance the state machine by one application tick.
 *
 * @param button_pressed Current debounced button state. While the button is
 *                       released the sequence freezes and every LED keeps its
 *                       current duty cycle; it resumes from the same position
 *                       when the button is pressed again.
 */
void sequence_tick(bool button_pressed);

/** @brief Start/stop the sequence (double-click). Stopping freezes the current duty cycle. */
void sequence_toggle_running(void);

/** @brief true if the sequence is currently running. */
bool sequence_is_running(void);

#endif /* SEQUENCE_H */
