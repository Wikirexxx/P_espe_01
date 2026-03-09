#ifndef ILI9341_DRIVER_H
#define ILI9341_DRIVER_H
#include <stdint.h>

#define max_valor_eje_y 1140

// Básicos
#define COLOR_NEGRO      0x0000
#define COLOR_BLANCO     0xFFFF
#define COLOR_ROJO       0xF800
#define COLOR_VERDE      0x07E0
#define COLOR_AZUL       0x001F

// Secundarios
#define COLOR_AMARILLO   0xFFE0
#define COLOR_CIAN       0x07FF
#define COLOR_MAGENTA    0xF81F

// Grises
#define COLOR_GRIS_CLARO 0xC618
#define COLOR_GRIS       0x8410
#define COLOR_GRIS_OSCURO 0x4208

// Otros útiles
#define COLOR_NARANJA    0xFD20
#define COLOR_ROSA       0xF81F
#define COLOR_MORADO     0x8010
#define COLOR_CAFE       0xA145
#define COLOR_LIMA       0xAFE5
#define COLOR_AZUL_CIELO 0x867D
#define COLOR_VERDE_OSC  0x03E0
#define COLOR_ROJO_OSC   0x7800

void ili_send_CMD(uint8_t cmd);
void ILI9341_init(void);
void __attribute__((optimize("O0"))) delay_cycles(uint32_t cyc);
void ili_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ili9341_fill_screen(uint16_t rgb565);
void reset_TFT(void);
void draw_char(uint16_t x, uint16_t y,const uint8_t *glyph,uint16_t color, uint16_t bg);
void draw_string(uint16_t x, uint16_t y,const char *s,uint16_t color, uint16_t bg);
void ili_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rgb565);
void ili_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rgb565);
void ili_draw_point4(uint16_t x, uint16_t y, uint16_t color);
void ili_float_2_intstring(float val, char* buf);
void ili_plantilla_grafica(void);
void ili_brightness(uint32_t brightness);
void ili_fill_graphs(void);
void ili_draw_graph(float rpmr_data, float rpme_data, float time_data);

#endif