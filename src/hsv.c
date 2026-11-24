#include "hsv.h"
 
#include <math.h>
#include <stdint.h>
 
#include "board_config.h"
#include "led.h"
 
static float   m_hsv[HSV_COMP_COUNT];   /* текущие H, S, V        */
static int8_t  m_dir[HSV_COMP_COUNT];   /* направление пинг-понга */
static uint8_t m_ramp_cnt;              /* счётчик тиков до шага  */
 
static const float m_step[HSV_COMP_COUNT] = {
    HSV_HUE_STEP,
    HSV_SV_STEP,
    HSV_SV_STEP,
};
 
static float comp_max(hsv_component_t comp)
{
    return (comp == HSV_HUE) ? 360.0f : 100.0f;
}
 
void hsv_init(void)
{
    /* H из Device ID: последние две цифры как проценты от 360 */
    m_hsv[HSV_HUE] = 360.0f * (float)DEVICE_ID_HUE_PCT / 100.0f;
    m_hsv[HSV_SAT] = 100.0f;
    m_hsv[HSV_VAL] = 100.0f;
 
    /* S и V стартуют с максимума - сначала удобно двигаться вниз */
    m_dir[HSV_HUE] = 1;
    m_dir[HSV_SAT] = -1;
    m_dir[HSV_VAL] = -1;
 
    m_ramp_cnt = 0;
}
//2
void hsv_ramp(hsv_component_t comp, bool active)
{
    if (!active || comp >= HSV_COMP_COUNT) {
        return;
    }
 
    /* шаг делается один раз в HSV_RAMP_TICKS тиков (100 мс) */
    if (++m_ramp_cnt < HSV_RAMP_TICKS) {
        return;
    }
    m_ramp_cnt = 0;
 
    float v = m_hsv[comp] + (float)m_dir[comp] * m_step[comp];
    float max = comp_max(comp);
 
    /*на границе диапазона меняем направление */
    if (v >= max) {
        v = max;
        m_dir[comp] = -1;
    } else if (v <= 0.0f) {
        v = 0.0f;
        m_dir[comp] = 1;
    }
 
    m_hsv[comp] = v;
}
//3
/* Классическое преобразование HSV -> RGB, результат 0..255 */
static void hsv_to_rgb(float h, float s_pct, float v_pct,
                       uint8_t * p_r, uint8_t * p_g, uint8_t * p_b)
{
    float s  = s_pct / 100.0f;
    float v  = v_pct / 100.0f;
    float c  = v * s;                        /* хрома C = V * S     */
    float hp = h / 60.0f;                    /* номер сектора, 0..6 */
    float x  = c * (1.0f - fabsf(fmodf(hp, 2.0f) - 1.0f));
    float m  = v - c;
 
    float r = 0.0f, g = 0.0f, b = 0.0f;
 
    if (hp < 1.0f) { r = c; g = x; }    /* красный-жёлтый  */
    else if (hp < 2.0f) { r = x; g = c; }
    else if (hp < 3.0f) { g = c; b = x; }    /* зелёный-голубой */
    else if (hp < 4.0f) { g = x; b = c; }
    else if (hp < 5.0f) { r = x; b = c; }    /* синий-пурпурный */
    else { r = c; b = x; }
 
    *p_r = (uint8_t)((r + m) * 255.0f + 0.5f);
    *p_g = (uint8_t)((g + m) * 255.0f + 0.5f);
    *p_b = (uint8_t)((b + m) * 255.0f + 0.5f);
}
 
void hsv_apply(void)
{
    uint8_t r, g, b;
 
    hsv_to_rgb(m_hsv[HSV_HUE], m_hsv[HSV_SAT], m_hsv[HSV_VAL],
               &r, &g, &b);
    led_set_rgb(r, g, b);
}
 
float hsv_get(hsv_component_t comp)
{
    return (comp < HSV_COMP_COUNT) ? m_hsv[comp] : 0.0f;
}
