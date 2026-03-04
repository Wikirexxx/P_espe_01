#ifndef MMCR_H
#define MMCR_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_DIMX 5
#define MAX_DIMY 5
//////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    float    rpm;
    uint32_t pos;
    uint16_t d;
    float hist_rpm[10];
} v_motor_t;

#define V_MOTOR_DEFAULTS   { \
    .rpm = 0.0f, \
    .pos = 0u, \
    .d = 973u, \
    .hist_rpm = {0} \
}

/* Variable global compartida: solo declaración */
extern volatile v_motor_t g_motor;

// Variables globales solo para probar algoritmo minimos cuadrados recursivos
typedef struct
{
    float k_gain[MAX_DIMX];
    float ut_k_1;
    float ut_k_2;
    float y_k_1;
    float y_k_2;
    float z[MAX_DIMX];
    float g[MAX_DIMX];
    float fhi;
    float lambda;
    float ye;
    float e;
    float hist_error[20];
    float error_promedio;
    float yr;
    float ut;
    float C[MAX_DIMX][MAX_DIMY];
    float I[MAX_DIMX][MAX_DIMY];
    float alfa2;
    float inv_lambda;
    float Pe[5];
    float zgpp;
    float kg_Z[MAX_DIMX][MAX_DIMY];
    float I_minus_KZ[MAX_DIMX][MAX_DIMX];
    float A_scaled[MAX_DIMX][MAX_DIMX];    
    float R[MAX_DIMX][MAX_DIMX];
    float I_minus_KZ_T[MAX_DIMX][MAX_DIMX];
    float C_new[MAX_DIMX][MAX_DIMX];
} variables_RLS_t;

#define V_MMCr_DEFAULTS { \
    .k_gain = {0}, \
    .ut_k_1 = 0.0f, \
    .ut_k_2 = 0.0f, \
    .y_k_1 = 0.0f, \
    .y_k_2 = 0.0f, \
    .z = {0}, \
    .g = {0}, \
    .fhi = 0.989f, \
    .lambda = 0.0f, \
    .ye = 0.0f, \
    .e = 100.0f, \
    .hist_error = {0}, \
    .error_promedio = 0.0f, \
    .yr = 0.0f, \
    .ut = 0.0f, \
    .C = {{0}}, \
    .I = {{0}}, \
    .alfa2 = 0.0f, \
    .inv_lambda = 0.0f, \
    .Pe = {0.1f,0.1f,0.1f,0.1f,0.1f}, \
    .zgpp = 0.0f, \
    .kg_Z = {{0}}, \
    .I_minus_KZ = {{0}}, \
    .A_scaled = {{0}}, \
    .R = {{0}}, \
    .I_minus_KZ_T = {{0}}, \
    .C_new = {{0}} \
}
extern volatile variables_RLS_t g_RLS;

float producto_punto(volatile float *a, volatile float *b, int n);
void build_z(volatile float *z, volatile float ut, volatile float ut_k_1, volatile float ut_k_2, volatile float y_k_1, volatile float y_k_2);
void crear_C(volatile float C[MAX_DIMX][MAX_DIMY], float valor_inicial);
void multiplicar_matriz_vector(volatile float C[MAX_DIMX][MAX_DIMY], volatile float z[MAX_DIMX], volatile float g[MAX_DIMX]);
void producto_exterior(volatile float K[MAX_DIMX],volatile float z[MAX_DIMX],volatile float M[MAX_DIMX][MAX_DIMX]);
void restar_matrices(volatile float A[MAX_DIMX][MAX_DIMX], volatile float B[MAX_DIMX][MAX_DIMX], volatile float R[MAX_DIMX][MAX_DIMX]);
void escalar_matriz_out(volatile float A[MAX_DIMX][MAX_DIMX],  volatile float escalar, volatile float B[MAX_DIMX][MAX_DIMX]);
void multiplicar_matrices(volatile float A[MAX_DIMX][MAX_DIMX],volatile float B[MAX_DIMX][MAX_DIMX], volatile float R[MAX_DIMX][MAX_DIMX]);
void transponer_matriz(volatile float A[MAX_DIMX][MAX_DIMX],volatile float AT[MAX_DIMX][MAX_DIMX]);
void copiar_matriz(volatile float A[MAX_DIMX][MAX_DIMX], volatile float B[MAX_DIMX][MAX_DIMX]);
void simetrizar(volatile float C[MAX_DIMX][MAX_DIMX]);
void clamp_covariance(volatile float C[MAX_DIMX][MAX_DIMX], float floor, float ceiling);

#endif