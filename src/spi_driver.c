#define STM32F405xx
#include "stm32f4xx.h"
#include "spi_driver.h"

#define SPI1ENb 0x00001000U
#define GPIOAENb 0x00000001U

// SPI1 MOSI    PA7
// SPI1 SCK     PA5
// SPI1 MISO    PA6  (no usado para el caso de ILI9341)

void SPI1_gpio_init(void)
{
    // Enable clock access to GPIOA
    RCC->AHB1ENR |= GPIOAENb;

    /* -------- MODER: PA5 y PA7 en Alternate Function (10b) --------
       PA5 -> bits [11:10]
       PA7 -> bits [15:14]
       Clear mask: 0x0000CC00
       Set value : 0x00008800
    */
    GPIOA->MODER &= ~0x0000CC00U;
    GPIOA->MODER |=  0x00008800U;

    /* -------- AFRL: AF5 para PA5 y PA7 --------
       PA5 AFRL5 -> bits [23:20] = 0x5
       PA7 AFRL7 -> bits [31:28] = 0x5
       Clear mask: 0xF0F00000
       Set value : 0x50500000
    */
    GPIOA->AFR[0] &= ~0xF0F00000U;
    GPIOA->AFR[0] |=  0x50500000U;

    /* (Opcional) OSPEEDR: alta velocidad (11b) para PA5 y PA7 */
    GPIOA->OSPEEDR |= 0x0000CC00U;

    /* (Opcional) OTYPER: push-pull (0) para PA5 y PA7 */
    GPIOA->OTYPER &= ~0x000000A0U;   // bit5=0x20, bit7=0x80

    /* (Opcional) PUPDR: sin pull-up/pull-down (00b) para PA5 y PA7 */
    GPIOA->PUPDR &= ~0x0000CC00U;
}


void SPI1_init(void)
{
    // Enable clock access to SPI1
    RCC->APB2ENR |= SPI1ENb;
    // Set clock BR = 111 (÷256)
    SPI1->CR1 &= ~0x0038U;
    SPI1->CR1 |=  0x0010U;  // esto equivale a fPCLK/64 84MHz/16 = 5.25MHz 
    // Para ILI9341, la velocidad máxima de reloj SPI es de 10 MHz. 
    // Configuración para ILI9341
    // Set SPI Mode 0
    // Eso corresponde al SPI modo 0 (CPOL = 0, CPHA = 0):
    // → CPOL = 0 → reloj inactivo en bajo.
    // → CPHA = 0 → los datos se capturan en el flanco ascendente (primer flanco).
    SPI1->CR1 &= ~0x0003;   //configuro CPOL=0 y CPHA=0
    // Set full-duplex mode
    SPI1->CR1 &= ~0x0400U;
    // Set MSB first
    SPI1->CR1 &= ~0x0080U;
    // Set master mode
    SPI1->CR1 |=  0x0004U;
    // Set data format to 8 bits
    SPI1->CR1 &= ~0x0800U;
    // Selec slave by software
    SPI1->CR1 |=  0x0300U;
    // Enable SPI1 
    SPI1->CR1 |=  0x0040U;
}
void SPI1_send_byte(uint8_t data)
{
    // Esperar a que el buffer de transmisión esté vacío
    while (!(SPI1->SR & SPI_SR_TXE));

    // Escribir el byte (acceso de 8 bits al DR)
    *((volatile uint8_t *)&SPI1->DR) = data;

    // Esperar a que termine la transmisión
    while (!(SPI1->SR & SPI_SR_TXE));
    while (SPI1->SR & SPI_SR_BSY);
}
