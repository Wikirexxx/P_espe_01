#ifndef QUAD_ENC_H
#define QUAD_ENC_H
#include <stdint.h>

void encoder_tim2_init(void);
int32_t encoder_get_count(void);
void encoder_reset(void);
int32_t encoder_get_speed_tps(float Ts);

#endif