#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H
#include <stdint.h>

#define STM32F405xx
#include "stm32f4xx.h"

// --- CS (Chip Select) ---
#define TFT_CS_LOW()    (GPIOB->BSRR = 0x00400000U)  // PB6 = 0
#define TFT_CS_HIGH()   (GPIOB->BSRR = 0x00000040U)  // PB6 = 1

// --- DC / RS (Data / Command) ---
#define TFT_DC_COMMAND() (GPIOB->BSRR = 0x00800000U) // PB7 = 0 (command)
#define TFT_DC_DATA()    (GPIOB->BSRR = 0x00000080U) // PB7 = 1 (data)

// --- RST (Reset) ---
#define TFT_RST_LOW()   (GPIOB->BSRR = 0x01000000U)  // PB8 = 0
#define TFT_RST_HIGH()  (GPIOB->BSRR = 0x00000100U)  // PB8 = 1

//Apaga PA2 y enciende PA3 para izquierda
#define left_motor()  (GPIOA->BSRR = 0x00080004U)

//Apaga PA3 y enciende PA2 para derecha
#define right_motor() (GPIOA->BSRR = 0x00040008U)

// Led RGB en PB12, PB13, PB14
#define LED_R_ON()    (GPIOB->BSRR = 0x00001000U)  // SET  PB12
#define LED_R_OFF()   (GPIOB->BSRR = 0x10000000U)  // RST  PB12
#define LED_G_ON()    (GPIOB->BSRR = 0x00002000U)  // SET  PB13
#define LED_G_OFF()   (GPIOB->BSRR = 0x20000000U)  // RST  PB13
#define LED_B_ON()    (GPIOB->BSRR = 0x00004000U)  // SET  PB14
#define LED_B_OFF()   (GPIOB->BSRR = 0x40000000U)  // RST  PB14
#define blink_R()     (GPIOB->ODR ^= 0x00001000U)  // TOGGLE PB12
#define blink_G()     (GPIOB->ODR ^= 0x00002000U)  // TOGGLE PB13
#define blink_B()     (GPIOB->ODR ^= 0x00004000U)  // TOGGLE PB14

void TFT_ctrl_gpio_init(void);
void GPIOB_Init_PB12_13_14_Output(void);
void gpio_pa2_pa3_output_init(void);

#endif