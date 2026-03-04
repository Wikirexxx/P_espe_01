#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H
#include <stdint.h>

/* ========= Config ========= */
#define PWM_TARGET_HZ   20000u   // 20 kHz (bueno para motor DC)
#define DUTY_MAX        1000u    // duty en "permil": 0..1000 = 0..100%

void pwm_pb0_tim3_init(uint32_t pwm_hz, uint32_t duty_permille);
void pwm_pb0_tim3_set_duty(uint32_t duty_permille);
void pwm_pb0_tim3_invert_polarity(int invert);

void pwm_tim1_pa8_init(uint32_t pwm_hz);
void pwm_tim1_set_duty_permille(uint16_t duty_permille);

#endif