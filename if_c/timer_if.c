#include "timer_if.h"
#include "timer_driver.h"

const timer_if_t timer_STM32 = {
    .tim_control_init = timer4_init
};