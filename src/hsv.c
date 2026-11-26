#include "hsv.h"
 
#include <math.h>
#include <stdint.h>
 
#include "board_config.h"
#include "led.h"

#include "nrf_log.h"//++
 
static float   m_hsv[HSV_COMP_COUNT];//H, S, V
static int8_t  m_dir[HSV_COMP_COUNT];//dir
static uint8_t m_ramp_cnt;//ticks count
 
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
    /*i`ve taken first two numbers as initial color*/
    m_hsv[HSV_HUE] = 360.0f * (float)DEVICE_ID_HUE_PCT / 100.0f;
    m_hsv[HSV_SAT] = 100.0f;
    m_hsv[HSV_VAL] = 100.0f;
 
    //direction
    m_dir[HSV_HUE] = 1;
    m_dir[HSV_SAT] = -1;
    m_dir[HSV_VAL] = -1;
 
    m_ramp_cnt = 0;
}

void hsv_ramp(hsv_component_t comp, bool active)
{
    if (!active || comp >= HSV_COMP_COUNT) {
        return;
    }
 
    if (++m_ramp_cnt < HSV_RAMP_TICKS) {
        return;
    }
    m_ramp_cnt = 0;
 
    float v = m_hsv[comp] + (float)m_dir[comp] * m_step[comp];
    float max = comp_max(comp);
 
    if (v >= max) {
        v = max;
        m_dir[comp] = -1;
    } else if (v <= 0.0f) {
        v = 0.0f;
        m_dir[comp] = 1;
    }
 
    m_hsv[comp] = v;
    
    hsv_log();//++
}
/*HSV -> RGB*/
static void hsv_to_rgb(float h, float s_pct, float v_pct,
                       uint8_t * p_r, uint8_t * p_g, uint8_t * p_b)
{
    float s  = s_pct / 100.0f;
    float v  = v_pct / 100.0f;
    float c  = v * s;
    float hp = h / 60.0f;
    float x  = c * (1.0f - fabsf(fmodf(hp, 2.0f) - 1.0f));
    float m  = v - c;
 
    float r = 0.0f, g = 0.0f, b = 0.0f;
 
    if (hp < 1.0f) { r = c; g = x; }
    else if (hp < 2.0f) { r = x; g = c; }
    else if (hp < 3.0f) { g = c; b = x; }
    else if (hp < 4.0f) { g = x; b = c; }
    else if (hp < 5.0f) { r = x; b = c; }
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

void hsv_log(void)//++
{
    NRF_LOG_INFO("h=%u s=%u v=%u",
                 (uint32_t)(m_hsv[HSV_HUE] + 0.5f),
                 (uint32_t)(m_hsv[HSV_SAT] + 0.5f),
                 (uint32_t)(m_hsv[HSV_VAL] + 0.5f));
}
