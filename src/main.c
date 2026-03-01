#define STM32F405xx
#include "stm32f4xx.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "system_clock_driver.h"
#include "spi_driver.h"
#include "gpio_driver.h"
#include "pwm_driver.h"
#include "ILI9341_driver.h"
#include "quad_enc.h"
#include "mmcr.h"
#include "uart_driver.h"
#include "timer_driver.h"

void FPU_Enable(void);
void clear_fault_flags(void);

float hist_rpm[10] = {0};
void shift_right(float *v, int size, float new_value);
float promedio_float(const float *v, size_t n);
uint8_t eval_error(float *e,uint8_t dim);



uint16_t posx_act_data = 0;
float rpmr_data = 0.0f;
float rpme_data = 0.0f;
float time_data = 0.0f;
void ini_pantalla(void);

float tabla_seno[] = {
    300,
    317,
    335,
    352,
    368,
    385,
    400,
    415,
    429,
    441,
    453,
    464,
    473,
    481,
    488,
    493,
    497,
    499,
    500,
    499,
    497,
    493,
    488,
    481,
    473,
    464,
    453,
    441,
    429,
    415,
    400,
    385,
    368,
    352,
    335,
    317,
    300,
    283,
    265,
    248,
    232,
    215,
    200,
    185,
    171,
    159,
    147,
    136,
    127,
    119,
    112,
    107,
    103,
    101,
    100,
    101,
    103,
    107,
    112,
    119,
    127,
    136,
    147,
    159,
    171,
    185,
    200,
    215,
    232,
    248,
    265,
    283
};
uint32_t pos = 0;
uint32_t dbg_pos = 0;
float rpm = 0;
uint8_t i = 0;
uint16_t d = 973;
uint8_t flag_cMotor = 0;
uint32_t intentos = 0;
uint32_t intentos2 = 0;
uint8_t flag_error = 0;
typedef struct{
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
} ieee_754_bytes_t;

typedef union {
    float f;
    ieee_754_bytes_t bytes;
} ieee_754_float_t;

    uint8_t out;
    uint8_t captura[4];
    uint8_t cont = 0;
    uint8_t j2 = 0;

int main(void)
{
    ieee_754_float_t captura_float;
    clear_fault_flags();
    FPU_Enable();
    volatile uint32_t cpacr = SCB->CPACR;
    SystemClock_Config();
    ini_pantalla();
    encoder_tim2_init();
    pwm_tim1_pa8_init(PWM_TARGET_HZ);   // 20 kHz en PA8
    usart3_pb10_pb11_init_115200();
    gpio_pa2_pa3_output_init();
    GPIOB_Init_PB12_13_14_Output();
    crear_C(C);
    timer4_init();


    while (1) 
    {
        left_motor();
        pwm_tim1_set_duty_permille(d);
        //LED_R_ON();
        //LED_G_ON();
        //LED_B_ON();
        /*
        if(d < 700)
        {
            d++;
        }
        else
        {
            d = 400;
            flag_cMotor ^= 1;
            if (flag_cMotor)
            {
                right_motor();
            }
            else
            {
                left_motor();
            }
        }
        pwm_tim1_set_duty_permille(d);

        pos = encoder_get_count(); // (void)pos; // int32_t
        (void)pos;
        ili_draw_graph(tabla_seno[i], tabla_seno[i] + 5, time_data);
        i = i + 1;
       
        time_data = time_data + 1;
        if(time_data > 500)
        {
            time_data = 0;
        }
        if (i >= 72)
        {
            i = 0;
        }
        Delay_ms(50);
        */
        
        if (usart3_read_byte_nonblocking(&out))
        {
            for (j2 = 0; j2 < 4; j2++)
            {
                while(!(USART3->SR & USART_SR_RXNE)){} 
                captura[j2] = (USART3->DR & 0xFFu);
            }
            captura_float.bytes.d = captura[0];
            captura_float.bytes.c = captura[1];
            captura_float.bytes.b = captura[2];
            captura_float.bytes.a = captura[3];   
            
            // Implementación de algoritmo de mínimos cuadrados recursivos

            ut = captura_float.f;                               // ut = valor de entrada a la planta
            build_z(z, ut, ut_k_1, ut_k_2, y_k_1, y_k_2);       // z = vector de regresores
            yr = producto_punto(P, z, MAX_DIMX);                // yr = salida de la planta estimada por el modelo
            
            ut_k_2 = ut_k_1;
            ut_k_1 = ut;
            y_k_2 = y_k_1;
            y_k_1 = yr;
            
            multiplicar_matriz_vector(C, z, g);
            zgpp = producto_punto(z, g, MAX_DIMX);
            alfa2 = fhi * fhi + zgpp;
            ye = producto_punto(Pe, z, MAX_DIMX);
            e = yr - ye;
            for (i = 0; i < MAX_DIMX; i++) 
            {
                Pe[i] = Pe[i] + (1.0f / alfa2) * g[i] * e;
            }
            actualizar_C(C, g, fhi, alfa2);
            
            Delay_ms(4);
            captura_float.f = yr; 
            usart3_write_byte(captura_float.bytes.d);
            usart3_write_byte(captura_float.bytes.c);
            usart3_write_byte(captura_float.bytes.b);
            usart3_write_byte(captura_float.bytes.a);
            captura_float.f = ye;
            usart3_write_byte(captura_float.bytes.d);
            usart3_write_byte(captura_float.bytes.c);
            usart3_write_byte(captura_float.bytes.b);
            usart3_write_byte(captura_float.bytes.a); 
            //*********************************************************************
        }
    }
}
void ini_pantalla(void)
{
    TFT_ctrl_gpio_init();
    pwm_pb0_tim3_init(10000, 450); // PWM Usado para brillo TFT
    SPI1_gpio_init();
    SPI1_init();
    reset_TFT();
    ILI9341_init();
    ili_plantilla_grafica();
    ili_fill_graphs();
}
void FPU_Enable(void)
{
    SCB->CPACR |= (0xF << 20);  // Habilita CP10 y CP11 (FPU)
    __DSB();
    __ISB();
}
void clear_fault_flags(void)
{
    SCB->CFSR = 0xFFFFFFFF;   // limpia UFSR/BFSR/MMFSR
    SCB->HFSR = 0xFFFFFFFF;   // limpia hardfault status (w1c)
    SCB->DFSR = 0xFFFFFFFF;   // opcional
}
void shift_right(float *v, int size, float new_value)
{
    for (int i = size - 1; i > 0; i--)
    {
        v[i] = v[i - 1];
    }

    v[0] = new_value;
}
float promedio_float(const float *v, size_t n)
{
    if (n == 0) return 0.0f;   // evitar división por cero

    float suma = 0.0f;

    for (size_t i = 0; i < n; i++)
    {
        suma += v[i];
    }

    return suma / (float)n;
}
uint8_t eval_error(float *e,uint8_t dim)
{
    uint8_t count = 0;
    uint8_t i = 0;
    for (i = 0; i < dim; i++)
    {
        if (fabsf(e[i]) > 10.0f) // umbral de error, ajustar según sea necesario
        {
            count++;
        }
    }
    return count;
}
void TIM4_IRQHandler(void)
{
    ieee_754_float_t captura_float;
    if (TIM4->SR & TIM_SR_UIF) 
    { 
        TIM4->SR &= ~TIM_SR_UIF;   // clear UIF (escritura 0)
        pos = encoder_get_count(); // (void)pos; // int32_t
        (void)pos;
        blink_R();
        // 44 pulsos por vuelta directo en motor, muestra cada 10ms, relación engranes = 9.28:1, rpm max = 1360 
        rpm = pos / 44.0f * 100.0f * 60.0f / 9.28f; // rpm = (pulsos / pulsos_por_vuelta) * 100 (para pasar a segundos) * 60 (para pasar a minutos) / relación de engranajes
        if(rpm == 0)
        (void)rpm;
        dbg_pos = pos; // variable para debug en breakpoint, muestra el valor de pos cada 10ms
        pos = 0;
        encoder_reset();
        shift_right(hist_rpm, 10, rpm);
        if(intentos < 10)
        {
            intentos++;
        }
        else
        {
            rpm = promedio_float(hist_rpm, 10);
             //------------------------------------------------------------------------------------
            //------------------Se ejecula la estimación del modelo ARX---------------------------
            //------------------------------------------------------------------------------------
            ut = d;
            build_z(z, ut, ut_k_1, ut_k_2, y_k_1, y_k_2);       // z = vector de regresores
            

            ut_k_2 = ut_k_1;
            ut_k_1 = ut;
            y_k_2 = y_k_1;
            y_k_1 = rpm; // y_k_1 = salida de la planta (rpm medida)

            if (flag_error == 0)
            {
                multiplicar_matriz_vector(C, z, g);
                zgpp = producto_punto(z, g, MAX_DIMX);
                alfa2 = fhi * fhi + zgpp;
            }
            ye = producto_punto(Pe, z, MAX_DIMX);
            e = rpm - ye;
            shift_right(hist_error, 20, e);
            if(eval_error(hist_error,20))
            {
                flag_error = 0;
            }
            else
            {
                flag_error = 1;
            }
            if (flag_error == 0)
            {
                inv_alfa2 = 1.0f / alfa2;
                if (!isfinite(alfa2) || fabsf(alfa2) < 1e-12f) 
                {
                    while(1);
                }
                if (!isfinite(fhi) || fabsf(fhi) < 1e-12f) 
                {
                    while(1);
                }
                for (i = 0; i < MAX_DIMX; i++) 
                {
                    Pe[i] = Pe[i] + (1.0f / alfa2) * g[i] * e;
                }
                actualizar_C(C, g, fhi, alfa2);
            }
            //------------------------------------------------------------------------------------
            //------------------------------------------------------------------------------------
        }
        
       
    }
}