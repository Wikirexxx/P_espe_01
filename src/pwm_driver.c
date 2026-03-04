#define STM32F405xx
#include "stm32f4xx.h"
#include "gpio_driver.h"
#include "system_clock_driver.h"
#include "pwm_driver.h"

static uint32_t get_pclk2_hz(void);
static uint32_t get_tim1clk_hz(void);


static uint16_t g_arr = 0;

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
    {
        TIM3->CCER |=  TIM_CCER_CC3P;  // invertido
    }
    else        
    {
        TIM3->CCER &= ~TIM_CCER_CC3P;  // n
    }
}
/* ========= Helpers: obtener TIM1CLK real =========
   En STM32F4: si el prescaler de APB2 != 1, el clock de los timers en APB2 = 2*PCLK2
*/
static uint32_t get_pclk2_hz(void)
{
    // Asume que SystemCoreClock ya está correcto (por SystemInit o tu init de clocks)
    // Calcula PCLK2 usando RCC->CFGR (PPRE2)
    uint32_t hclk = 168000000;

    uint32_t ppre2 = (RCC->CFGR >> RCC_CFGR_PPRE2_Pos) & 0x7; // bits PPRE2[2:0]

    // Tabla de divisores para APB prescaler: 0xxx = /1, 100=/2, 101=/4, 110=/8, 111=/16
    uint32_t div;
    if (ppre2 < 4) div = 1;
    else          div = 1u << (ppre2 - 3); // 4->2,5->4,6->8,7->16

    return hclk / div;
}

static uint32_t get_tim1clk_hz(void)
{
    uint32_t pclk2 = get_pclk2_hz();
    uint32_t ppre2 = (RCC->CFGR >> RCC_CFGR_PPRE2_Pos) & 0x7;

    // Si APB2 prescaler = 1 => TIMCLK = PCLK2
    // Si APB2 prescaler != 1 => TIMCLK = 2*PCLK2
    if (ppre2 < 4) return pclk2;
    else           return 2u * pclk2;
}


void pwm_tim1_pa8_init(uint32_t pwm_hz)
{
    /* 1) Clocks */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;    // TIM1 clock

    /* 2) PA8 -> AF1 (TIM1_CH1) */
    // MODER: Alternate function (10)
    GPIOA->MODER &= ~(3u << (8 * 2));
    GPIOA->MODER |=  (2u << (8 * 2));

    // AFRH for pin 8 (pins 8..15): AFRH[3:0] corresponds to pin8
    GPIOA->AFR[1] &= ~(0xFu << ((8 - 8) * 4));
    GPIOA->AFR[1] |=  (0x1u << ((8 - 8) * 4)); // AF1 = TIM1/2

    // Opcional: salida rápida y push-pull
    GPIOA->OSPEEDR |= (3u << (8 * 2));    // very high speed
    GPIOA->OTYPER  &= ~(1u << 8);         // push-pull
    GPIOA->PUPDR   &= ~(3u << (8 * 2));   // no pull

    /* 3) Config TIM1 para PWM Mode 1 en CH1 */
    TIM1->CR1 = 0;         // reset básico
    TIM1->CR2 = 0;
    TIM1->SMCR = 0;
    TIM1->DIER = 0;

    uint32_t timclk = get_tim1clk_hz();

    // Queremos: PWM = timclk / ((PSC+1)*(ARR+1))
    // Elegimos PSC = 0 y calculamos ARR directo (si cabe), si no, ajusta PSC.
    uint32_t psc = 0;
    uint32_t arr = (timclk / pwm_hz) - 1u;

    // Si ARR excede 16-bit, subimos PSC hasta que quepa.
    while (arr > 0xFFFFu) {
        psc++;
        arr = (timclk / ((psc + 1u) * pwm_hz)) - 1u;
        if (psc > 0xFFFFu) break; // por seguridad
    }

    TIM1->PSC = (uint16_t)psc;
    TIM1->ARR = (uint16_t)arr;
    g_arr = (uint16_t)arr;

    // Duty inicial 0%
    TIM1->CCR1 = 0;

    // PWM mode 1 en OC1M, habilitar preload OC1PE
    TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M);
    TIM1->CCMR1 |=  (6u << TIM_CCMR1_OC1M_Pos); // 110 = PWM mode 1
    TIM1->CCMR1 |=  TIM_CCMR1_OC1PE;            // preload enable

    // Habilitar salida CH1
    TIM1->CCER |= TIM_CCER_CC1E;                // enable compare output

    // Auto-reload preload enable
    TIM1->CR1 |= TIM_CR1_ARPE;

    // TIM1 es "advanced": hay que habilitar main output
    TIM1->BDTR |= TIM_BDTR_MOE;

    // Generar update event para cargar registros
    TIM1->EGR |= TIM_EGR_UG;

    // Counter enable
    TIM1->CR1 |= TIM_CR1_CEN;
}

/* duty_permille: 0..1000 => 0%..100% */
void pwm_tim1_set_duty_permille(uint16_t duty_permille)
{
    if (duty_permille > DUTY_MAX) duty_permille = DUTY_MAX;

    // CCR1 = duty * (ARR+1) / 1000
    // (ARR+1) cabe en 16-bit, pero multiplica en 32-bit
    uint32_t ccr = ((uint32_t)duty_permille * (uint32_t)(g_arr + 1u)) / DUTY_MAX;

    if (ccr > g_arr) ccr = g_arr; // seguridad (100% = ARR)
    TIM1->CCR1 = (uint16_t)ccr;
}