#define STM32F405xx
#include "stm32f4xx.h"
#include "gpio_driver.h"
#include "system_clock_driver.h"

#include "stm32f4xx.h"

static uint32_t get_apb1_timer_clock_hz(void)
{
    // TIM3 está en APB1.
    // Si PPRE1 == 1 -> TIMCLK = PCLK1
    // Si PPRE1 != 1 -> TIMCLK = 2*PCLK1
    uint32_t hclk = 168000000UL; // Suponemos HCLK a 168 MHz

    static const uint16_t ahb_presc_tbl[16] = {
        1,1,1,1,1,1,1,1,2,4,8,16,64,128,256,512
    };
    static const uint8_t apb_presc_tbl[8] = {1,1,1,1,2,4,8,16};

    uint32_t cfgr = RCC->CFGR;

    uint32_t hpre  = (cfgr >> RCC_CFGR_HPRE_Pos) & 0xF;
    uint32_t ppre1 = (cfgr >> RCC_CFGR_PPRE1_Pos) & 0x7;

    uint32_t hclk_div  = ahb_presc_tbl[hpre];
    uint32_t pclk1_div = apb_presc_tbl[ppre1];

    uint32_t hclk_hz  = hclk / hclk_div;
    uint32_t pclk1_hz = hclk_hz / pclk1_div;

    return (pclk1_div == 1) ? pclk1_hz : (2U * pclk1_hz);
}

// PB0 = TIM3_CH3 (AF2)
void pwm_pb0_tim3_init(uint32_t pwm_hz, uint32_t duty_permille)
{
    if (pwm_hz == 0) pwm_hz = 1000;
    if (duty_permille > 1000) duty_permille = 1000;

    // 1) Clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // 2) PB0 -> Alternate Function AF2 (TIM3)
    // MODER: 10 = AF
    GPIOB->MODER &= ~(3U << (0U * 2U));
    GPIOB->MODER |=  (2U << (0U * 2U));

    // Push-pull, sin pull, high speed (opcional)
    GPIOB->OTYPER  &= ~(1U << 0U);
    GPIOB->PUPDR   &= ~(3U << (0U * 2U));
    GPIOB->OSPEEDR |=  (3U << (0U * 2U));

    // AFRL (pin 0): AF2
    GPIOB->AFR[0] &= ~(0xFU << (0U * 4U));
    GPIOB->AFR[0] |=  (0x2U << (0U * 4U));

    // 3) TIM3 base
    TIM3->CR1 = 0;

    uint32_t tim_clk = get_apb1_timer_clock_hz();

    // Elegimos tick ~1 MHz para buena resolución
    uint32_t target_tick = 1000000UL;     // 1 MHz
    uint32_t psc = tim_clk / target_tick;
    if (psc == 0) psc = 1;
    psc -= 1;

    uint32_t tick = tim_clk / (psc + 1U);
    uint32_t arr  = tick / pwm_hz;
    if (arr == 0) arr = 1;
    arr -= 1;

    TIM3->PSC = (uint16_t)psc;
    TIM3->ARR = (uint16_t)arr;

    // Duty en CH3
    uint32_t ccr = ((arr + 1UL) * duty_permille) / 1000UL;
    TIM3->CCR3 = (uint16_t)ccr;

    // 4) PWM mode 1 en CH3 + preload
    // CH3 vive en CCMR2
    TIM3->CCMR2 &= ~(TIM_CCMR2_CC3S_Msk | TIM_CCMR2_OC3M_Msk);
    TIM3->CCMR2 |=  (6U << TIM_CCMR2_OC3M_Pos) | TIM_CCMR2_OC3PE;

    // Enable CH3 output
    TIM3->CCER |= TIM_CCER_CC3E;

    // ARR preload
    TIM3->CR1 |= TIM_CR1_ARPE;

    // Cargar preloads
    TIM3->EGR |= TIM_EGR_UG;

    // Start
    TIM3->CR1 |= TIM_CR1_CEN;
}

void pwm_pb0_tim3_set_duty(uint32_t duty_permille)
{
    if (duty_permille > 1000) duty_permille = 1000;
    uint32_t arr = TIM3->ARR;
    TIM3->CCR3 = (uint16_t)(((arr + 1UL) * duty_permille) / 1000UL);
}

// Si tu backlight es activo-bajo, invierte la salida:
// llamar una vez después del init.
void pwm_pb0_tim3_invert_polarity(int invert)
{
    if (invert) 
    
    TIM3->CCER |=  TIM_CCER_CC3P;  // invertido
    else        
    TIM3->CCER &= ~TIM_CCER_CC3P;  // n
}
