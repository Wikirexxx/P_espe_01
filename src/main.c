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

/* Definición + inicialización de variables para el motor*/
volatile v_motor_t g_motor = V_MOTOR_DEFAULTS;
/* Definición + inicialización de variables para el RLS */
volatile variables_RLS_t g_RLS = V_MMCr_DEFAULTS;

float dbg_rpmr = 0.0f;
float dbg_rpme = 0.0f;

uint8_t flag_multtimer = 0;
uint8_t cont_multtimer = 0;

void FPU_Enable(void);
void clear_fault_flags(void);




uint16_t posx_act_data = 0;
float rpmr_data = 0.0f;
float rpme_data = 0.0f;
float time_data = 0.0f;

void ini_pantalla(void);

// Variables del RLS





uint8_t flag_cMotor = 0;
uint32_t intentos = 0;
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
    clear_fault_flags();
    FPU_Enable();
    SystemClock_Config();
    ini_pantalla();
    encoder_tim2_init();
    pwm_tim1_pa8_init(PWM_TARGET_HZ);   // 20 kHz en PA8
    usart3_pb10_pb11_init_115200();
    gpio_pa2_pa3_output_init();
    GPIOB_Init_PB12_13_14_Output();
    crear_C(g_RLS.C,10.10f);
    g_RLS.lambda = g_RLS.fhi * g_RLS.fhi; 
    g_RLS.inv_lambda = 1.0f / g_RLS.lambda;
    timer4_init();


    while (1) 
    {
        left_motor();
        pwm_tim1_set_duty_permille(g_motor.d);
        //LED_R_ON();
        //LED_G_ON();
        //LED_B_ON();
        if (flag_multtimer == 1)
        {
            flag_multtimer = 0;
            if(g_RLS.ye > 620 )
            {
                g_RLS.ye = 620;
            }
            if (g_RLS.ye < 0)
            {
                g_RLS.ye = 0;
            }
            if (g_motor.rpm > 620)
            {
                g_motor.rpm = 620;
            }
            if (g_motor.rpm < 0)
            {
                g_motor.rpm = 0;
            }
            
            ili_draw_graph(g_RLS.ye, g_motor.rpm, 100);
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
void TIM4_IRQHandler(void)
{
    LED_R_ON();
    uint8_t i = 0;
    if (TIM4->SR & TIM_SR_UIF) 
    { 
        TIM4->SR &= ~TIM_SR_UIF;   // clear UIF (escritura 0)
        g_motor.pos = encoder_get_count(); // (void)pos; 
        // 44 pulsos por vuelta directo en motor, muestra cada 10ms, relación engranes = 9.28:1, rpm max = 1360 
        g_motor.rpm = g_motor.pos / 44.0f * 100.0f * 60.0f / 9.28f; // rpm = (pulsos / pulsos_por_vuelta) * 100 (para pasar a segundos) * 60 (para pasar a minutos) / relación de engranajes
        g_motor.pos = 0;
        encoder_reset();
        shift_right(g_motor.hist_rpm, 10, g_motor.rpm);
        if(intentos < 20)
        {
            g_RLS.hist_error[intentos] = 100.0f; // llenar el historial de error con un valor alto para evitar falsas activaciones del RLS al inicio
            intentos++;
        }
        else
        {
            g_motor.rpm = promedio_float(g_motor.hist_rpm, 10);
            dbg_rpmr = g_motor.rpm;
            
             //------------------------------------------------------------------------------------
            //------------------Se ejecula la estimación del modelo ARX---------------------------
            //------------------------------------------------------------------------------------
            g_RLS.ut = g_motor.d /1000.0f; // entrada normalizada (0.0 a 1.0)
            build_z(g_RLS.z, g_RLS.ut, g_RLS.ut_k_1, g_RLS.ut_k_2, g_RLS.y_k_1, g_RLS.y_k_2);       // z = vector de regresores
            
            g_RLS.ut_k_2 = g_RLS.ut_k_1;
            g_RLS.ut_k_1 = g_RLS.ut;
            g_RLS.y_k_2 = g_RLS.y_k_1;
            g_RLS.y_k_1 = g_motor.rpm/1000.0f; // y_k_1 = salida de la planta (rpm medida)

            g_RLS.ye = producto_punto(g_RLS.Pe, g_RLS.z, MAX_DIMX);           //===== Predicción con parámetros estimados ===== ye(k) = Pe.' * z;
            dbg_rpme = g_RLS.ye;
            g_RLS.e = g_motor.rpm - g_RLS.ye;                                   //===== Error a priori ===== e = y(k) - ye(k);
            shift_right(g_RLS.hist_error, 20, g_RLS.e);
            if(intentos == 20)
            {
                g_RLS.error_promedio = promedio_float(g_RLS.hist_error, 20);
                if (fabsf(g_RLS.error_promedio) > 5.0f && flag_cMotor == 0) // umbral de error promedio, ajustar según sea necesario
                {
                    multiplicar_matriz_vector(g_RLS.C, g_RLS.z, g_RLS.g);             // ======= RLS estable ======= g = C*z

                    g_RLS.zgpp = producto_punto(g_RLS.z, g_RLS.g, MAX_DIMX);          // ======= alpha^2 = lambda + z'*g =======
                    g_RLS.alfa2 = g_RLS.fhi * g_RLS.fhi + g_RLS.zgpp;
                                                                    
                    for (int i = 0; i < MAX_DIMX; i++)              // ======= Ganancia K = g / alfa2; =======
                    {
                        g_RLS.k_gain[i] = g_RLS.g[i] / g_RLS.alfa2;
                    }
                                
                    for (i = 0; i < MAX_DIMX; i++)                  // ======= Actualización de parámetros ======= Pe = Pe + K*e; =======
                    {
                        g_RLS.Pe[i] = g_RLS.Pe[i] + (g_RLS.k_gain[i] * g_RLS.e);
                    }
                                                                    // ======= Actualización covarianza (Joseph stabilized form) I = eye(size(C)); =======
                                                                    // ======= C = (1/lambda) * (I - K*z.') * C * (I - K*z.').' ; =======
                    crear_C(g_RLS.I,1.0f);                                        // identidad
                    producto_exterior(g_RLS.k_gain,g_RLS.z,g_RLS.kg_Z);                       // kg_Z = K*z.'
                    restar_matrices(g_RLS.I, g_RLS.kg_Z, g_RLS.I_minus_KZ);                   // I - K*z.'
                    escalar_matriz_out(g_RLS.I_minus_KZ, g_RLS.inv_lambda, g_RLS.A_scaled);   // A_scaled = (1/lambda) * (I - K*z.')
                    multiplicar_matrices(g_RLS.A_scaled, g_RLS.C, g_RLS.R);                   // R = A_scaled * C
                    // (1) Transpuesta
                    transponer_matriz(g_RLS.I_minus_KZ, g_RLS.I_minus_KZ_T);
                    // (2) C_new = R * (I-Kz^T)^T
                    multiplicar_matrices(g_RLS.R, g_RLS.I_minus_KZ_T, g_RLS.C_new);
                    // (3) C = C_new
                    copiar_matriz(g_RLS.C_new, g_RLS.C);
                    simetrizar(g_RLS.C);
                    clamp_covariance(g_RLS.C, 1e-3f, 1e3f);
                }
                if (fabs(g_RLS.error_promedio) < 5.0f)
                {
                    if(flag_cMotor == 0)
                    {
                        crear_C(g_RLS.C,10.10f);
                        g_RLS.alfa2 = 0.0f;
                        for(i = 0; i < MAX_DIMX; i++)
                        {
                            g_RLS.k_gain[i] = 0.0f;
                            g_RLS.g[i] = 0.0f;
                        }
                    }
                    flag_cMotor = 1;
                }
                else
                {
                    if (flag_cMotor == 1)
                    {
                        //for(i = 0; i < MAX_DIMX; i++)
                        //{
                        //    Pe[i] = 0.1f; // reiniciar parámetros a valores iniciales
                        //}
                    }
                    
                    flag_cMotor = 0;
                }
                
                
            }
            //------------------------------------------------------------------------------------
            //------------------------------------------------------------------------------------
        }
        cont_multtimer = cont_multtimer + 1;
        if(cont_multtimer >= 10) // cada 100ms
        {
            flag_multtimer = 1;
            cont_multtimer = 0;
        }
    }
    LED_R_OFF();
}