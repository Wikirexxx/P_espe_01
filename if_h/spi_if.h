#ifndef SPI_IF_H
#define SPI_IF_H

#include <stdint.h>

/* callback tipo void(void) */
typedef void (*spi_void_fn_t)(void);

/* callback tipo void(uint8_t) */
typedef void (*spi_send_byte_fn_t)(uint8_t data);

typedef struct {
    spi_void_fn_t        gpio_init;   // configura pines AF del SPI
    spi_void_fn_t        init;        // configura el periférico SPI
    spi_send_byte_fn_t   send_byte;   // envía 1 byte
} spi_if_t;

/* ===== ESTA ES LA "INSTANCIA" QUE SE INYECTA ===== */
extern const spi_if_t SPI1_STM32;

#endif // SPI_IF_H