#ifndef LED_H
#define LED_H
 
#include <stdbool.h>
#include <stdint.h>
 
typedef enum {
    LED_RED = 0,
    LED_GREEN,
    LED_BLUE,
    LED_COLOR_MAX
} led_color_t;
 
void led_init(void);
void led_all_off(void);
 
/* LED2 */
void led_set_rgb(uint8_t r, uint8_t g, uint8_t b);
 
/* LED1 */
void led_mode_init(void);
void led_mode_set(bool on);
 
#endif /* LED_H */
