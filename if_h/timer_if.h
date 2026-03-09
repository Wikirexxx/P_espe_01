#ifndef TIMER_IF_H
#define TIMER_IF_H
#include <stdint.h>

typedef struct 
{
    void (*tim_control_init)(void);
}timer_if_t;

extern const timer_if_t timer_STM32;

#endif