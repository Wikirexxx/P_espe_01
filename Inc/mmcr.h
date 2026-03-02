#ifndef MMCR_H
#define MMCR_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_DIMX 5
#define MAX_DIMY 5

// Variables globales solo para probar algoritmo minimos cuadrados recursivos
extern float P[5];

extern float k_gain[MAX_DIMX];
extern float ut_k_1;
extern float ut_k_2;
extern float y_k_1;
extern float y_k_2;
extern float z[MAX_DIMX];
extern float g[MAX_DIMX];
extern float fhi;
extern float lambda;
extern float ye;
extern float e;
extern float error_promedio;
extern float hist_error[20];
extern float yr;
extern float ut;
extern float C[MAX_DIMX][MAX_DIMY];
extern float I[MAX_DIMX][MAX_DIMY];
extern float alfa2;
extern float inv_lambda;
extern float Pe[5];
extern float zgpp;
extern float kg_Z[MAX_DIMX][MAX_DIMY];
extern float I_minus_KZ[MAX_DIMX][MAX_DIMX];
extern float A_scaled[MAX_DIMX][MAX_DIMX];
extern float R[MAX_DIMX][MAX_DIMX];
extern float I_minus_KZ_T[MAX_DIMX][MAX_DIMX];
extern float C_new[MAX_DIMX][MAX_DIMX];

float producto_punto(float *a, float *b, int n);
void build_z(float *z, float ut, float ut_k_1, float ut_k_2, float y_k_1, float y_k_2);
void crear_C(float C[MAX_DIMX][MAX_DIMY], float valor_inicial);
void multiplicar_matriz_vector(float C[MAX_DIMX][MAX_DIMY], float z[MAX_DIMX], float g[MAX_DIMX]);
void producto_exterior(const float K[MAX_DIMX],const float z[MAX_DIMX],float M[MAX_DIMX][MAX_DIMX]);
void restar_matrices(float A[MAX_DIMX][MAX_DIMX], float B[MAX_DIMX][MAX_DIMX], float R[MAX_DIMX][MAX_DIMX]);
void escalar_matriz_out(const float A[MAX_DIMX][MAX_DIMX], float escalar, float B[MAX_DIMX][MAX_DIMX]);
void multiplicar_matrices(const float A[MAX_DIMX][MAX_DIMX],const float B[MAX_DIMX][MAX_DIMX],float R[MAX_DIMX][MAX_DIMX]);
void transponer_matriz(const float A[MAX_DIMX][MAX_DIMX],float AT[MAX_DIMX][MAX_DIMX]);
void copiar_matriz(const float A[MAX_DIMX][MAX_DIMX],float B[MAX_DIMX][MAX_DIMX]);
void simetrizar(float C[MAX_DIMX][MAX_DIMX]);
void clamp_covariance(float C[MAX_DIMX][MAX_DIMX], float floor, float ceiling);

#endif