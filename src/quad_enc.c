#define STM32F405xx
#include "stm32f4xx.h"
#include "quad_enc.h"

/* =========================
   Encoder en TIM2 (PA0/PA1)
   ========================= */

void encoder_tim2_init(void)
{
    /* 1) Clocks */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* 2) PA0/PA1 -> AF1 (TIM2_CH1/CH2) */
    // MODER = Alternate function (10)
    GPIOA->MODER &= ~((3u<<(0*2)) | (3u<<(1*2)));
    GPIOA->MODER |=  ((2u<<(0*2)) | (2u<<(1*2)));

    // AFRL: pin0 y pin1 (AF1)
    GPIOA->AFR[0] &= ~((0xFu<<(0*4)) | (0xFu<<(1*4)));
    GPIOA->AFR[0] |= 0x00000011; // pin0=AF1, pin1=AF1

    // Pull-ups opcionales (depende de tu encoder; muchos son push-pull)
    // Si tu encoder es colector abierto, esto ayuda.
    GPIOA->PUPDR &= ~((3u<<(0*2)) | (3u<<(1*2)));
    GPIOA->PUPDR |=  ((1u<<(0*2)) | (1u<<(1*2))); // pull-up

    /* 3) TIM2 en modo Encoder */
    TIM2->CR1 = 0;
    TIM2->CR2 = 0;
    TIM2->SMCR = 0;
    TIM2->DIER = 0;

    // Máximo rango (32-bit)
    TIM2->ARR = 0xFFFFFFFFu;
    TIM2->CNT = 0;

    // Config entradas: IC1/IC2 mapeadas a TI1/TI2
    // CC1S=01, CC2S=01
    TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
    TIM2->CCMR1 |=  (1u << TIM_CCMR1_CC1S_Pos) | (1u << TIM_CCMR1_CC2S_Pos);

    // Filtro digital (opcional). Útil si hay ruido (cables largos).
    // 0 = sin filtro; 3..8 típicos. Empieza con 4 si tienes rebotes/ruido.
    TIM2->CCMR1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC2F);
    TIM2->CCMR1 |=  (4u << TIM_CCMR1_IC1F_Pos) | (4u << TIM_CCMR1_IC2F_Pos);

    // Polaridad: normalmente ambos rising (no invertidos)
    TIM2->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P | TIM_CCER_CC1NP | TIM_CCER_CC2NP);

    // Encoder mode 3: cuenta en ambos canales (cuadratura completa)
    TIM2->SMCR &= ~TIM_SMCR_SMS;
    TIM2->SMCR |=  (3u << TIM_SMCR_SMS_Pos); // SMS=011

    // Habilitar capturas (necesario para encoder)
    TIM2->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;

    // Arrancar
    TIM2->CR1 |= TIM_CR1_CEN;
}

/* Lee la posición (ticks acumulados). En TIM2 es 32-bit. */
int32_t encoder_get_count(void)
{
    return (int32_t)TIM2->CNT;
}

/* Resetea la cuenta a 0 */
void encoder_reset(void)
{
    TIM2->CNT = 0;
}

/* =========================
   Cálculo de velocidad simple
   - Llamar cada Ts segundos (ej. 1 ms, 10 ms)
   - Devuelve ticks/segundo
   ========================= */
int32_t encoder_get_speed_tps(float Ts)
{
    static int32_t last = 0;
    int32_t now = (int32_t)TIM2->CNT;
    int32_t delta = now - last;
    last = now;
    // ticks por segundo
    return (int32_t)((float)delta / Ts);
}