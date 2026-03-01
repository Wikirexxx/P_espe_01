#define STM32F405xx
#include "stm32f4xx.h"
#include "timer_driver.h"

void timer4_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // Habilitar el reloj para TIM4
    (void)RCC->APB1ENR; // Lectura para asegurar que el reloj se ha habilitado
    //reset opcional
    TIM4->CR1 = 0;
    TIM4->CR2 = 0;
    TIM4->SMCR = 0;
    TIM4->DIER = 0;

    // Configurar periodo:
    // Queremos 1 kHz => 1ms
    // TIM_CLK = 84 MHz
    // Elegimos PSC para llevar a 1 MHz: PSC = 84 - 1 => 83  (84MHz/(83+1)=1MHz)
    // Luego ARR para 1ms: ARR = 1000 - 1 => 999 (1MHz/(999+1)=1kHz)
    TIM4->PSC = 83;
    TIM4->ARR = 9999;

    // 4) Generar update para cargar PSC/ARR
    TIM4->EGR = TIM_EGR_UG;

    // 5) Habilitar interrupción por Update
    TIM4->SR = 0;                 // limpia flags
    TIM4->DIER |= TIM_DIER_UIE;    // Update interrupt enable

    // 6) NVIC enable
    NVIC_SetPriority(TIM4_IRQn, 5);
    NVIC_EnableIRQ(TIM4_IRQn);

    // 7) Arrancar timer
    TIM4->CR1 |= TIM_CR1_CEN;
}
