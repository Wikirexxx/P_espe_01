#define STM32F405xx
#include "stm32f4xx.h"
#include "gpio_driver.h"
#include "system_clock_driver.h"


void TFT_ctrl_gpio_init(void)
{
    // 1. Habilitar reloj de GPIOB
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* 2. MODER: PB6, PB7, PB8 como OUTPUT (01)
       PB6 -> bits [13:12]
       PB7 -> bits [15:14]
       PB8 -> bits [17:16]
    */
    GPIOB->MODER &= ~0x0003F000U;   // limpia PB6–PB8
    GPIOB->MODER |=  0x00015000U;   // OUTPUT en PB6,7,8

    /* 3. OTYPER: push-pull (0) */
    GPIOB->OTYPER &= ~0x000001C0U;  // bits 6,7,8 = 0

    /* 4. OSPEEDR: alta velocidad (11) */
    GPIOB->OSPEEDR |=  0x0003F000U; // PB6–PB8 high speed

    /* 5. PUPDR: sin pull-up / pull-down (00) */
    GPIOB->PUPDR &= ~0x0003F000U;

    /* 6. Estado inicial seguro */
    GPIOB->BSRR =  (1U << 6);              // CS = 1 (inactivo)
    GPIOB->BSRR =  (1U << 7);              // DC = 1 (data por defecto)
    GPIOB->BSRR =  (1U << 8);              // RST = 1 (no reset)
}
void test_TFT_pins(void)
{
    //test de pines de control de pantalla TFT
    TFT_RST_LOW();
    TFT_CS_LOW();
    TFT_DC_COMMAND();
    Delay_loop(1000000);
    TFT_RST_HIGH();
    TFT_CS_HIGH();
    TFT_DC_DATA();
    Delay_loop(1000000);
}