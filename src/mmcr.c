#include "mmcr.h"

// Variables globales solo para probar algoritmo minimos cuadrados recursivos

float producto_punto(volatile float *a, volatile float *b, int n)
{
    uint8_t i = 0;
    float resultado = 0.0f;
    for (i = 0; i < n; i++) 
    {
        resultado = resultado + (a[i] * b[i]);
    }
    return resultado;
}
void build_z(volatile float *z, volatile float ut, volatile float ut_k_1, volatile float ut_k_2, volatile float y_k_1, volatile float y_k_2)
{
    // z= [u(k);y(k-1);u(k-1);y(k-2);u(k-2)]
    z[0] = ut;
    z[1] = y_k_1;
    z[2] = ut_k_1;
    z[3] = y_k_2;
    z[4] = ut_k_2;
}
void crear_C(volatile float C[MAX_DIMX][MAX_DIMY], float valor_inicial)
{
    for (int i = 0; i < MAX_DIMX; i++) {
        for (int j = 0; j < MAX_DIMY; j++) {
            C[i][j] = 0.0f;
        }
        C[i][i] = valor_inicial;
    }
}
void multiplicar_matriz_vector(volatile float C[MAX_DIMX][MAX_DIMY], volatile float z[MAX_DIMX], volatile float g[MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++) {
        g[i] = 0.0f;

        for (int j = 0; j < MAX_DIMY; j++) {
            g[i] += C[i][j] * z[j];
        }
    }
}
void producto_exterior(volatile float K[MAX_DIMX], volatile float z[MAX_DIMX], volatile float M[MAX_DIMX][MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        for (int j = 0; j < MAX_DIMX; j++)
        {
            M[i][j] = K[i] * z[j];
        }
    }
}
void restar_matrices(volatile float A[MAX_DIMX][MAX_DIMX], volatile float B[MAX_DIMX][MAX_DIMX], volatile float R[MAX_DIMX][MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        for (int j = 0; j < MAX_DIMX; j++)
        {
            R[i][j] = A[i][j] - B[i][j];
        }
    }
}
void escalar_matriz_out(volatile float A[MAX_DIMX][MAX_DIMX], volatile float escalar, volatile float B[MAX_DIMX][MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        for (int j = 0; j < MAX_DIMX; j++)
        {
            B[i][j] = A[i][j] * escalar;
        }
    }
}
void multiplicar_matrices(volatile float A[MAX_DIMX][MAX_DIMX],volatile float B[MAX_DIMX][MAX_DIMX], volatile float R[MAX_DIMX][MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        for (int j = 0; j < MAX_DIMX; j++)
        {
            R[i][j] = 0.0f;

            for (int k = 0; k < MAX_DIMX; k++)
            {
                R[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}
void transponer_matriz(volatile float A[MAX_DIMX][MAX_DIMX],volatile float AT[MAX_DIMX][MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        for (int j = 0; j < MAX_DIMX; j++)
        {
            AT[j][i] = A[i][j];
        }
    }
}
void copiar_matriz(volatile float A[MAX_DIMX][MAX_DIMX], volatile float B[MAX_DIMX][MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        for (int j = 0; j < MAX_DIMX; j++)
        {
            B[i][j] = A[i][j];
        }
    }
}
void simetrizar(volatile float C[MAX_DIMX][MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        for (int j = i + 1; j < MAX_DIMX; j++)
        {
            float s = 0.5f * (C[i][j] + C[j][i]);
            C[i][j] = s;
            C[j][i] = s;
        }
    }
}
void clamp_covariance(volatile float C[MAX_DIMX][MAX_DIMX], float floor, float ceiling)
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        if (C[i][i] < floor)
        {
            C[i][i] = floor;
        }
        if (C[i][i] > ceiling)
        {
            C[i][i] = ceiling;
        }
    }
}