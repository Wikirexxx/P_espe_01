#ifndef PWM_IF_H
#define PWM_IF_H

#include <stdint.h>

#define PWM_TARGET_HZ   20000u   // 20 kHz (bueno para motor DC)


/* Callbacks */
typedef void (*pwm_void_fn_t)(void);
typedef void (*pwm_motor_init_fn_t)(uint32_t pwm_hz);
typedef void (*pwm_motor_set_fn_t)(uint16_t duty_permille);

typedef void (*pwm_bl_init_fn_t)(uint32_t pwm_hz, uint32_t duty_permille);
typedef void (*pwm_bl_set_fn_t)(uint32_t duty_permille);
typedef void (*pwm_bl_inv_fn_t)(int invert);

/* --- Interfaz PWM motor (TIM1/PA8 en tu driver) --- */
typedef struct {
    pwm_motor_init_fn_t init;
    pwm_motor_set_fn_t  set_duty_permille;   // 0..1000
} pwm_motor_if_t;

/* --- Interfaz PWM backlight TFT (TIM3/PB0 en tu driver) --- */
typedef struct {
    pwm_bl_init_fn_t init;
    pwm_bl_set_fn_t  set_duty;              // 0..1000
    pwm_bl_inv_fn_t  invert_polarity;       // 0 normal / 1 invertido
} pwm_backlight_if_t;

/* --- Interfaz PWM completa del sistema --- */
typedef struct {
    pwm_motor_if_t      motor;
    pwm_backlight_if_t  tft_bl;
} pwm_if_t;

/* ===== ESTA ES LA "INSTANCIA" QUE SE INYECTA ===== */
extern const pwm_if_t PWM_STM32;

#endif // PWM_IF_H