#define STM32F405xx
#include "stm32f4xx.h"
#include "spi_driver.h"
#include "gpio_driver.h"
#include "ILI9341_driver.h"
#include "system_clock_driver.h"
#include "Chars.h"
#include "pwm_driver.h"

// Maximos caracteres por rengló 40, cuenta con 32 renglones

#define ILI9341_W 240U
#define ILI9341_H 320U

// Datos de plantilla
#define altura_grafica 200
#define ancho_grafica 210
#define inicio_x_grafica 30
#define inicio_y_grafica 100
#define pos_y_texto_grafica 305
#define ini_renglon_grafica 30
#define primera_linea_grafica 120
#define paso_renglon_grafica 20
#define max_valor_eje_y 620
#define division_eje_y 10
#define offset_alinea_y 7
#define divs_col_grafica 105
#define pos_text_datos_x 0
#define pos_text_vel_r_y 30
#define pos_text_tiempo_y 50
#define pos_text_vel_e_y 40
#define pos_text_error_y 60
#define pos_unidades_y 80
#define pos_unidades_y_Offset_x 5 

#define posx_float_rpmr 40
#define posy_float_rpmr 30
#define posx_float_rpme 40
#define posy_float_rpme 40
#define posx_float_time 40
#define posy_float_time 50
#define inc_col_data 2
#define offset_en_grafica 3

static uint16_t vec_datar[divs_col_grafica];
static uint16_t vec_datae[divs_col_grafica];
static uint16_t posx_act_data = 0;

void SPI1_send_CMD(uint8_t cmd)
{
    TFT_DC_COMMAND();
    SPI1_send_byte(cmd);
}
void SPI1_send_data(uint8_t data)
{
    TFT_DC_DATA();
    SPI1_send_byte(data);
}
void ILI9341_init(void)
{
    TFT_CS_LOW();
    // (Display off)
    SPI1_send_CMD(0x28);

    // Issue a series of initialization commands from the
    // Adafruit library for a simple 'known good' test.
    SPI1_send_CMD(0xEF);
    SPI1_send_data(0x03);
    SPI1_send_data(0x80);
    SPI1_send_data(0x02);
    SPI1_send_CMD(0xCF);
    SPI1_send_data(0x00);
    SPI1_send_data(0xC1);
    SPI1_send_data(0x30);
    SPI1_send_CMD(0xED);
    SPI1_send_data(0x64);
    SPI1_send_data(0x03);
    SPI1_send_data(0x12);
    SPI1_send_data(0x81);
    SPI1_send_CMD(0xE8);
    SPI1_send_data(0x85);
    SPI1_send_data(0x00);
    SPI1_send_data(0x78);
    SPI1_send_CMD(0xCB);
    SPI1_send_data(0x39);
    SPI1_send_data(0x2C);
    SPI1_send_data(0x00);
    SPI1_send_data(0x34);
    SPI1_send_data(0x02);
    SPI1_send_CMD(0xF7);
    SPI1_send_data(0x20);
    SPI1_send_CMD(0xEA);
    SPI1_send_data(0x00);
    SPI1_send_data(0x00);
    // PWCTR1
    SPI1_send_CMD(0xC0);
    SPI1_send_data(0x23);
    // PWCTR2
    SPI1_send_CMD(0xC1);
    SPI1_send_data(0x10);
    // VMCTR1
    SPI1_send_CMD(0xC5);
    SPI1_send_data(0x3E);
    SPI1_send_data(0x28);
    // VMCTR2
    SPI1_send_CMD(0xC7);
    SPI1_send_data(0x86);
    // MADCTL
    SPI1_send_CMD(0x36);
    SPI1_send_data(0x48);
    // VSCRSADD
    SPI1_send_CMD(0x37);
    SPI1_send_data(0x00);
    // PIXFMT
    SPI1_send_CMD(0x3A);
    SPI1_send_data(0x55);
    // FRMCTR1
    SPI1_send_CMD(0xB1);
    SPI1_send_data(0x00);
    SPI1_send_data(0x18);
    // DFUNCTR
    SPI1_send_CMD(0xB6);
    SPI1_send_data(0x08);
    SPI1_send_data(0x82);
    SPI1_send_data(0x27);
    SPI1_send_CMD(0xF2);
    SPI1_send_data(0x00);
    // GAMMASET
    SPI1_send_CMD(0x26);
    SPI1_send_data(0x01);
    // (Actual gamma settings)
    SPI1_send_CMD(0xE0);
    SPI1_send_data(0x0F);
    SPI1_send_data(0x31);
    SPI1_send_data(0x2B);
    SPI1_send_data(0x0C);
    SPI1_send_data(0x0E);
    SPI1_send_data(0x08);
    SPI1_send_data(0x4E);
    SPI1_send_data(0xF1);
    SPI1_send_data(0x37);
    SPI1_send_data(0x07);
    SPI1_send_data(0x10);
    SPI1_send_data(0x03);
    SPI1_send_data(0x0E);
    SPI1_send_data(0x09);
    SPI1_send_data(0x00);
    SPI1_send_CMD(0xE1);
    SPI1_send_data(0x00);
    SPI1_send_data(0x0E);
    SPI1_send_data(0x14);
    SPI1_send_data(0x03);
    SPI1_send_data(0x11);
    SPI1_send_data(0x07);
    SPI1_send_data(0x31);
    SPI1_send_data(0xC1);
    SPI1_send_data(0x48);
    SPI1_send_data(0x08);
    SPI1_send_data(0x0F);
    SPI1_send_data(0x0C);
    SPI1_send_data(0x31);
    SPI1_send_data(0x36);
    SPI1_send_data(0x0F);

    // Exit sleep mode.
    // Display on.
    SPI1_send_CMD(0x11);
    delay_cycles(8000000);
    SPI1_send_CMD(0x29);
    delay_cycles(8000000);
    TFT_CS_HIGH();
}
void __attribute__((optimize("O0"))) delay_cycles(uint32_t cyc) 
{
    uint32_t d_i;
    for (d_i = 0; d_i < cyc; ++d_i) 
    {
        asm("NOP");
    }
}
void ili_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    TFT_CS_LOW();

    SPI1_send_CMD(0x2A); // Column Address Set
    SPI1_send_data((uint8_t)(x0 >> 8));
    SPI1_send_data((uint8_t)(x0 & 0xFF));
    SPI1_send_data((uint8_t)(x1 >> 8));
    SPI1_send_data((uint8_t)(x1 & 0xFF));

    SPI1_send_CMD(0x2B); // Page Address Set
    SPI1_send_data((uint8_t)(y0 >> 8));
    SPI1_send_data((uint8_t)(y0 & 0xFF));
    SPI1_send_data((uint8_t)(y1 >> 8));
    SPI1_send_data((uint8_t)(y1 & 0xFF));

    TFT_CS_HIGH();
}

void ili9341_fill_screen(uint16_t rgb565)
{
    uint32_t n = (uint32_t)ILI9341_W * (uint32_t)ILI9341_H;

    ili_set_window(0, 0, ILI9341_W - 1, ILI9341_H - 1);

    TFT_CS_LOW();
    SPI1_send_CMD(0x2C); // Memory Write

    while (n--)
    {
        SPI1_send_data((uint8_t)(rgb565 >> 8));
        SPI1_send_data((uint8_t)(rgb565 & 0xFF));
    }

    TFT_CS_HIGH();
}
void reset_TFT(void)
{
    TFT_RST_LOW();
    Delay_ms(12);
    TFT_RST_HIGH();
    Delay_ms(122);
}
void draw_char(uint16_t x, uint16_t y,const uint8_t *glyph,uint16_t color, uint16_t bg)
{
    ili_set_window(x, y, x+4, y+6);
    TFT_CS_LOW();
    SPI1_send_CMD(0x2C);
    for (int row = 0; row < 7; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            if (glyph[row] & (1 << (4 - col)))
            {
                SPI1_send_data(color >> 8);
                SPI1_send_data(color & 0xFF);
            }
            else
            {
                SPI1_send_data(bg >> 8);
                SPI1_send_data(bg & 0xFF);
            }
        }
    }
    TFT_CS_HIGH();
}
void draw_string(uint16_t x, uint16_t y,const char *s,uint16_t color, uint16_t bg)
{
    uint16_t cx = x;

    while (*s) {
        const uint8_t *glyph = FONT_SPACE;

        if (*s >= 'A' && *s <= 'Z') 
        {
            glyph = FONT_A_Z[*s - 'A'];
        } 
        else if (*s >= '0' && *s <= '9') 
        {
            glyph = FONT_0_9[*s - '0'];
        } 
        else if (*s == ' ') 
        {
            glyph = FONT_SPACE;
        }
        else if (*s == '*') 
        {
            glyph = FONT_ASTERISK;
        }
        else if (*s >= 'a' && *s <= 'z') 
        {
            glyph = FONT_a_z[*s - 'a'];
        }
        else if (*s == '.') 
        {
            glyph = FONT_DOT;
        }
        else if (*s ==  '-')
        {
            glyph = FONT_DASH;
        }
        else if(*s == ':')
        {
            glyph = FONT_COLON;
        }
        draw_char(cx, y, glyph, color, bg);
        cx += 6;  // 5 px + 1 px de espacio
        s++;
    }
}
// ili_fill_rect(20, 30, 80, 50, 0xF800); // rojo relleno
void ili_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rgb565)
{
    uint32_t n;

    if (w == 0 || h == 0) return;

    // Ventana de dibujo
    ili_set_window(x, y, x + w - 1, y + h - 1);

    TFT_CS_LOW();
    SPI1_send_CMD(0x2C); // Memory Write

    n = (uint32_t)w * (uint32_t)h;
    while (n--) {
        SPI1_send_data((uint8_t)(rgb565 >> 8));
        SPI1_send_data((uint8_t)(rgb565 & 0xFF));
    }

    TFT_CS_HIGH();
}
// ili_draw_rect(20, 30, 80, 50, 0x07E0); // verde contorno
void ili_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rgb565)
{
    if (w == 0 || h == 0) return;

    // Línea superior
    ili_fill_rect(x, y, w, 1, rgb565);
    // Línea inferior
    if (h > 1) ili_fill_rect(x, y + h - 1, w, 1, rgb565);
    // Línea izquierda
    if (h > 2) ili_fill_rect(x, y + 1, 1, h - 2, rgb565);
    // Línea derecha
    if (w > 1 && h > 2) ili_fill_rect(x + w - 1, y + 1, 1, h - 2, rgb565);
}
void ili_draw_point4(uint16_t x, uint16_t y, uint16_t color)
{
    // Dibuja un punto 2x2 (4 píxeles)
    ili_fill_rect(x, y, 2, 2, color);
}
void ili_float_2_intstring(float val, char* buf)
{
    uint16_t entero = (uint16_t)val;
    uint16_t mil = (uint16_t)(entero / 1000);
    uint16_t centena = (uint16_t)((entero - (mil * 1000)) / 100);
    uint16_t decena = (uint16_t)((entero - (mil *  1000) - (centena * 100)) / 10);
    uint16_t unidad = (uint16_t)(entero - (mil * 1000) - (centena * 100) - (decena * 10));
    buf[0] = (uint8_t)(mil + '0');
    buf[1] = (uint8_t)(centena + '0');
    buf[2] = (uint8_t)(decena + '0');
    buf[3] = (uint8_t)(unidad + '0');
}
void ili_plantilla_grafica(void)
{
    uint8_t it = 0;
    //uint8_t max_renglon = 0;
    char buf_valor_eje_y[4];
    float valor_eje_y_f = 0.0f;
    //uint16_t ini_texto_eje_x = 0;
    //--------------------------------------------------- Grafica
    ili_fill_rect(inicio_x_grafica, inicio_y_grafica , ancho_grafica, altura_grafica, COLOR_BLANCO); // rojo relleno
    ili_fill_rect(ini_renglon_grafica, inicio_y_grafica-3, ancho_grafica, 3, COLOR_BLANCO);        
    //max_renglon = altura_grafica / paso_renglon_grafica - 1;
    //for(it = 0; it < max_renglon; it++)
    //{
    //    ili_fill_rect(ini_renglon_grafica, primera_linea_grafica + paso_renglon_grafica * it, ancho_grafica, 1, COLOR_GRIS_CLARO);        
    //}
    //--------------------------------------------------- Valores eje y
    valor_eje_y_f = (float)(max_valor_eje_y);
    for(it = 0 ; it< division_eje_y; it++)
    {
        ili_float_2_intstring(valor_eje_y_f, buf_valor_eje_y);
        draw_string(0, (inicio_y_grafica-offset_alinea_y) + paso_renglon_grafica * it, buf_valor_eje_y, COLOR_BLANCO, COLOR_NEGRO); // blanco sobre negro
        valor_eje_y_f -= (float)(max_valor_eje_y) / (float)(division_eje_y);
    }
    draw_string(0, (inicio_y_grafica-offset_alinea_y) + paso_renglon_grafica * it, "0000", COLOR_BLANCO, COLOR_NEGRO); // blanco sobre negro
    //--------------------------------------------------- Valores eje x
    //ini_texto_eje_x = (uint16_t)(altura_grafica + inicio_y_grafica + offset_alinea_y);
    draw_string((uint16_t)(ancho_grafica / 2 + inicio_x_grafica -10), pos_y_texto_grafica, "TIME", COLOR_ROJO, COLOR_NEGRO); // blanco sobre negro
    ili_fill_rect(inicio_x_grafica, pos_y_texto_grafica+2, ancho_grafica/2 -15, 1, COLOR_ROJO);
    ili_fill_rect(inicio_x_grafica + (ancho_grafica/2 + 17), pos_y_texto_grafica+2, ancho_grafica/2 -17, 1, COLOR_ROJO);
    draw_string(pos_text_datos_x, pos_text_vel_r_y, "RPMr:", COLOR_BLANCO, COLOR_NEGRO); 
    draw_string(pos_text_datos_x, pos_text_vel_e_y, "RPMe:", COLOR_BLANCO, COLOR_NEGRO); 
    draw_string(pos_text_datos_x, pos_text_tiempo_y, "Time:", COLOR_BLANCO, COLOR_NEGRO); 
    draw_string(pos_text_datos_x, pos_text_error_y, "Error abs:", COLOR_BLANCO, COLOR_NEGRO); 
    draw_string(pos_text_datos_x + pos_unidades_y_Offset_x, pos_unidades_y, "RPM", COLOR_BLANCO, COLOR_NEGRO);
}
void ili_brightness(uint32_t brightness)
{
    pwm_pb0_tim3_set_duty(brightness);
}
void ili_fill_graphs(void)
{
    uint8_t i = 0;
    for(i = 0; i < divs_col_grafica; i++)
    {
        vec_datar[i] = (uint16_t)(((inicio_y_grafica + altura_grafica)-(0 * altura_grafica / max_valor_eje_y))-offset_en_grafica);
        vec_datae[i] = (uint16_t)(((inicio_y_grafica + altura_grafica)-(0 * altura_grafica / max_valor_eje_y))-offset_en_grafica);
    }   
}
void ili_draw_graph(float rpmr_data, float rpme_data, float time_data)
{
    char val_step[5];        
    val_step[4] = '\0';

    // Escribir RPMr
    ili_float_2_intstring(rpmr_data, val_step);
    draw_string(posx_float_rpmr, posy_float_rpmr, val_step, COLOR_BLANCO, COLOR_NEGRO);
    // Escribir RPMe
    ili_float_2_intstring(rpme_data, val_step);
    draw_string(posx_float_rpme, posy_float_rpme, val_step, COLOR_BLANCO, COLOR_NEGRO);
    // Escribir Tiempo
    ili_float_2_intstring(time_data, val_step);
    draw_string(posx_float_time, posy_float_time, val_step, COLOR_BLANCO, COLOR_NEGRO);

    //Procedimiento de grafica
    ili_draw_point4(posx_act_data * inc_col_data + inicio_x_grafica,  vec_datar[posx_act_data], COLOR_BLANCO); // Se borra el punto actual
    ili_draw_point4(posx_act_data * inc_col_data + inicio_x_grafica, (uint16_t)(((inicio_y_grafica + altura_grafica)-(rpmr_data * altura_grafica / max_valor_eje_y))-offset_en_grafica) , COLOR_ROJO); // Se dibuja el nuevo punto

    ili_draw_point4(posx_act_data * inc_col_data + inicio_x_grafica,  vec_datae[posx_act_data], COLOR_BLANCO); // Se borra el punto actual
    ili_draw_point4(posx_act_data * inc_col_data + inicio_x_grafica, (uint16_t)(((inicio_y_grafica + altura_grafica)-(rpme_data * altura_grafica / max_valor_eje_y))-offset_en_grafica) , COLOR_AZUL); // Se dibuja el nuevo punto

    vec_datar[posx_act_data] = (uint16_t)(((inicio_y_grafica + altura_grafica)-(rpmr_data * altura_grafica / max_valor_eje_y))-offset_en_grafica); // Se guarda el nuevo punto
    vec_datae[posx_act_data] = (uint16_t)(((inicio_y_grafica + altura_grafica)-(rpme_data * altura_grafica / max_valor_eje_y))-offset_en_grafica); // Se guarda el nuevo punto
    posx_act_data = posx_act_data + 1;
    if (posx_act_data >= divs_col_grafica)
    {
        posx_act_data = 0;
    }
}