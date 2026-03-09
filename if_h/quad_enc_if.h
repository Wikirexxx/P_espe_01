#ifndef QUAD_ENC_IF_H
#define QUAD_ENC_IF_H

#include <stdint.h>

typedef struct 
{
    void (*init)(void);
    int32_t (*get_count)(void);
    void (*reset)(void);
    int32_t (*get_speed_tps)(float Ts);
}quad_enc_if_t;

extern const quad_enc_if_t quad_STM32;

#endif