#ifndef MATRICES_H
#define MATRICES_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_DIMX 5
#define MAX_DIMY 5

float ut_k_1 = 0;
float ut_k_2 = 0;
float y_k_1 = 0;
float y_k_2 = 0;
float z[MAX_DIMX] = {0,0,0,0,0};
float g[MAX_DIMX] = {0,0,0,0,0};

float fhi = 0.98;

float ye = 0;
float e;

float producto_punto(float *a, float *b, int n);
void build_z(float *z, float ut, float ut_k_1, float ut_k_2, float y_k_1, float y_k_2);


#endif