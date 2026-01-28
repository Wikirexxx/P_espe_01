#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H
#include <stdint.h>

void pwm_pb0_tim3_init(uint32_t pwm_hz, uint32_t duty_permille);
void pwm_pb0_tim3_set_duty(uint32_t duty_permille);
void pwm_pb0_tim3_invert_polarity(int invert);


#endif