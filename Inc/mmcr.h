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
extern float yr;
extern float ut;


float producto_punto(float *a, float *b, int n);
void build_z(float *z, float ut, float ut_k_1, float ut_k_2, float y_k_1, float y_k_2);


#endif