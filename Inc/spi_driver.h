#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H
#include <stdint.h>


void SPI1_gpio_init(void);
void SPI1_init(void);
void SPI1_send_byte(uint8_t data);

#endif