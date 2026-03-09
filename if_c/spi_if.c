#include "spi_if.h"
#include "spi_driver.h"

/* ===== ESTA ES LA "INSTANCIA" QUE SE INYECTA ===== */
const spi_if_t SPI1_STM32 = {
    .gpio_init = SPI1_gpio_init,
    .init      = SPI1_init,
    .send_byte = SPI1_send_byte
};