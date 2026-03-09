#ifndef GPIO_IF_H
#define GPIO_IF_H

#include <stdint.h>

// ================================= Interfaz: Firma genérica: funciones sin argumentos =================================
typedef void (*gpio_void_fn_t)(void);

// ================================= Interfaz: Control TFT (CS/DC/RST) =================================
typedef struct 
{
    gpio_void_fn_t init;        // configura pines de control TFT
    gpio_void_fn_t cs_low;
    gpio_void_fn_t cs_high;
    gpio_void_fn_t dc_command;
    gpio_void_fn_t dc_data;
    gpio_void_fn_t rst_low;
    gpio_void_fn_t rst_high;
} gpio_tft_if_t;

// ================================= Interfaz: Dirección de motor (PA2/PA3) =================================
typedef struct 
{
    gpio_void_fn_t init;   
    gpio_void_fn_t left;   
    gpio_void_fn_t right;  
} gpio_motor_if_t;

// ================================= Interfaz: LED RGB (PB12/PB13/PB14) =================================
typedef struct 
{
    gpio_void_fn_t init;     // configura PB12/13/14 como salida
    gpio_void_fn_t r_on;
    gpio_void_fn_t r_off;
    gpio_void_fn_t g_on;
    gpio_void_fn_t g_off;
    gpio_void_fn_t b_on;
    gpio_void_fn_t b_off;
    gpio_void_fn_t r_toggle; 
    gpio_void_fn_t g_toggle; 
    gpio_void_fn_t b_toggle; 
} gpio_rgb_if_t;

// ================================= Interfaz GPIO completa del sistema =================================
typedef struct 
{
    gpio_tft_if_t   tft;
    gpio_motor_if_t motor;
    gpio_rgb_if_t   rgb;
} gpio_if_t;

// Esta es la instancia para STM32 implementación
extern const gpio_if_t GPIO_STM32;

#endif