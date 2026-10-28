#ifndef LED_H
#define LED_H

#include <stdint.h>

/**
 * @brief
 * @param counts
 * @param size
 */
void led_sequence_init(const uint8_t *counts, uint8_t size);

/**
 * @brief
 */
void led_sequence_step(void);

#endif // LED_H