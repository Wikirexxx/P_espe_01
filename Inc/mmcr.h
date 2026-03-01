#ifndef MMCR_H
#define MMCR_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_DIMX 5
#define MAX_DIMY 5

// Variables globales solo para probar algoritmo minimos cuadrados recursivos
extern float P[5];

extern float ut_k_1;
extern float ut_k_2;
extern float y_k_1;
extern float y_k_2;
extern float z[MAX_DIMX];
extern float g[MAX_DIMX];
extern float fhi;
extern float ye;
extern float e;
extern float hist_error[20];
extern float yr;
extern float ut;
extern float C[MAX_DIMX][MAX_DIMY];
extern float alfa2;
extern float inv_alfa2;
extern float Pe[5];
extern float zgpp;

float producto_punto(float *a, float *b, int n);
void build_z(float *z, float ut, float ut_k_1, float ut_k_2, float y_k_1, float y_k_2);
void crear_C(float C[MAX_DIMX][MAX_DIMY]);
void multiplicar_matriz_vector(float C[MAX_DIMX][MAX_DIMY], float z[MAX_DIMX], float g[MAX_DIMX]);
void actualizar_C(float C[MAX_DIMX][MAX_DIMY], const float g[MAX_DIMX], float fhi, float alfa2);

#endif