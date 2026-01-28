#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H
#include <stdint.h>

// --- CS (Chip Select) ---
#define TFT_CS_LOW()    (GPIOB->BSRR = 0x00400000U)  // PB6 = 0
#define TFT_CS_HIGH()   (GPIOB->BSRR = 0x00000040U)  // PB6 = 1

// --- DC / RS (Data / Command) ---
#define TFT_DC_COMMAND() (GPIOB->BSRR = 0x00800000U) // PB7 = 0 (command)
#define TFT_DC_DATA()    (GPIOB->BSRR = 0x00000080U) // PB7 = 1 (data)

// --- RST (Reset) ---
#define TFT_RST_LOW()   (GPIOB->BSRR = 0x01000000U)  // PB8 = 0
#define TFT_RST_HIGH()  (GPIOB->BSRR = 0x00000100U)  // PB8 = 1
#define TFT_RST_TOGGLE() (GPIOB->ODR ^= 0x00000100U) // PB8 toggle

void TFT_ctrl_gpio_init(void);
void test_TFT_pins(void);

#endif