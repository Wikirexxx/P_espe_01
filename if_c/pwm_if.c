#include "pwm_if.h"
#include "pwm_driver.h"

/* ===== ESTA ES LA "INSTANCIA" QUE SE INYECTA ===== */
const pwm_if_t PWM_STM32 = {
    .motor = {
        .init             = pwm_tim1_pa8_init,
        .set_duty_permille= pwm_tim1_set_duty_permille,
    },
    .tft_bl = {
        .init            = pwm_pb0_tim3_init,
        .set_duty        = pwm_pb0_tim3_set_duty,
        .invert_polarity = pwm_pb0_tim3_invert_polarity,
    }
};