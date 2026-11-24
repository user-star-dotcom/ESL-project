#ifndef HSV_H
#define HSV_H
 
#include <stdbool.h>
 
typedef enum {
    HSV_HUE = 0,   /* оттенок, 0..360 градусов       */
    HSV_SAT,       /* насыщенность, 0..100 процентов */
    HSV_VAL,       /* яркость, 0..100 процентов      */
    HSV_COMP_COUNT
} hsv_component_t;
 
void hsv_init(void);
 
/* Вызывать каждый тик: при active плавно меняет компоненту comp */
void hsv_ramp(hsv_component_t comp, bool active);
 
/* Пересчитать HSV -> RGB и обновить ШИМ светодиода LED2 */
void hsv_apply(void);
 
float hsv_get(hsv_component_t comp);
 
#endif /* HSV_H */
