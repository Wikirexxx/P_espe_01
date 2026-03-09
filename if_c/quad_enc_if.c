#include "quad_enc_if.h"
#include "quad_enc.h"

/// ===== ESTA ES LA "INSTANCIA" QUE SE INYECTA =====
const quad_enc_if_t quad_STM32 = {
    .init = encoder_tim2_init,
    .get_count = encoder_get_count,
    .reset = encoder_reset,
    .get_speed_tps = encoder_get_speed_tps
};