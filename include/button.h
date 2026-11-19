/**
 * @file button.h
 *
 * Button driver for SW1.
 *
 * Two independent concerns, deliberately split:
 *  - the current (debounced) button level, updated by button_update() from the
 *    application tick - used by the sequence to gate the breathing;
 *  - double-click recognition, driven by GPIOTE edge events - used to
 *    start/stop the sequence (Workshop: "use GPIOTE to handle button clicks").
 *
 * GPIOTE event handlers run in interrupt context: the registered callback is
 * called from the GPIOTE interrupt, so it must stay short.
 */
#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

/** Called on a recognized double-click, in GPIOTE interrupt context. */
typedef void (* button_double_click_handler_t)(void);

/** @brief Configure the pin and the GPIOTE event. */
void button_init(void);

/**
 * @brief Refresh the debounced button level.
 *
 * Call exactly once per application tick. The level is accepted after it has
 * been stable for BUTTON_DEBOUNCE_TICKS consecutive samples.
 */
void button_update(void);

/** @brief Current debounced button state (true = pressed). */
bool button_is_pressed(void);

/** @brief Register the callback fired on a double-click. Pass NULL to clear. */
void button_set_double_click_handler(button_double_click_handler_t handler);

#endif /* BUTTON_H */
