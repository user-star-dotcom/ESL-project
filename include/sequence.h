#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdbool.h>
#include <stdint.h>

#include "led.h"


typedef struct {
    led_color_t color;
    uint8_t     digit; //number of breaths
} sequence_step_t;

void sequence_init(sequence_step_t const * p_steps, uint8_t step_count);


void sequence_tick(bool button_pressed);

void sequence_toggle_running(void);

bool sequence_is_running(void);


void sequence_log_current_step(void);

#endif /* SEQUENCE_H */
