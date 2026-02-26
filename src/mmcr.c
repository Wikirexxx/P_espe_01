#include "mmcr.h"

// Variables globales solo para probar algoritmo minimos cuadrados recursivos
float P[5]={0,0.5,1,-0.6,0};

float ut_k_1 = 0;
float ut_k_2 = 0;
float y_k_1 = 0;
float y_k_2 = 0;
float z[MAX_DIMX] = {0,0,0,0,0};
float g[MAX_DIMX] = {0,0,0,0,0};
float fhi = 0.98f;
float ye = 0;
float e = 0;
float yr = 0;
float ut = 0;
float C[MAX_DIMX][MAX_DIMY];
float alfa2 = 0;
float Pe[5]={0.1,0.1,0.1,0.1,0.1};
float zgpp = 0;

float producto_punto(float *a, float *b, int n) 
{
    uint8_t i = 0;
    float resultado = 0.0f;
    for (i = 0; i < n; i++) 
    {
        resultado = resultado + (a[i] * b[i]);
    }
    return resultado;
}
void build_z(float *z, float ut, float ut_k_1, float ut_k_2, float y_k_1, float y_k_2)
{
    // z= [u(k);y(k-1);u(k-1);y(k-2);u(k-2)]
    z[0] = ut;
    z[1] = y_k_1;
    z[2] = ut_k_1;
    z[3] = y_k_2;
    z[4] = ut_k_2;
}
void crear_C(float C[MAX_DIMX][MAX_DIMY])
{
    for (int i = 0; i < MAX_DIMX; i++) {
        for (int j = 0; j < MAX_DIMY; j++) {
            C[i][j] = 0.0f;
        }
        C[i][i] = 100.0f;
    }
}
void multiplicar_matriz_vector(float C[MAX_DIMX][MAX_DIMY], float z[MAX_DIMX], float g[MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++) {
        g[i] = 0.0f;

        for (int j = 0; j < MAX_DIMY; j++) {
            g[i] += C[i][j] * z[j];
        }
    }
}
void actualizar_C(float C[MAX_DIMX][MAX_DIMY], const float g[MAX_DIMX], float fhi, float alfa2)
{
    float inv_fhi2  = 1.0f / (fhi * fhi);
    float inv_alfa2 = 1.0f / alfa2;

    for (int i = 0; i < MAX_DIMX; i++) {
        for (int j = 0; j < MAX_DIMY; j++) {
            C[i][j] = inv_fhi2 * ( C[i][j] - (inv_alfa2 * g[i] * g[j]) );
        }
    }
}