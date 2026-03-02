#include "mmcr.h"

// Variables globales solo para probar algoritmo minimos cuadrados recursivos
float P[5]={0,0.5,1,-0.6,0};

float k_gain[MAX_DIMX] = {0,0,0,0,0};
float ut_k_1 = 0;
float ut_k_2 = 0;
float y_k_1 = 0;
float y_k_2 = 0;
float z[MAX_DIMX] = {0,0,0,0,0};
float g[MAX_DIMX] = {0,0,0,0,0};
float fhi = 0.989f;
float lambda = 0.0f;
float ye = 0;
float e = 100;
float hist_error[20] = {0};
float error_promedio = 0;
float yr = 0;
float ut = 0;
float C[MAX_DIMX][MAX_DIMY];
float I[MAX_DIMX][MAX_DIMY];
float alfa2 = 0;
float inv_lambda = 0;
float Pe[5]={0.1,0.1,0.1,0.1,0.1};
float zgpp = 0;
float kg_Z[MAX_DIMX][MAX_DIMY];
float I_minus_KZ[MAX_DIMX][MAX_DIMX];
float A_scaled[MAX_DIMX][MAX_DIMX];    
float R[MAX_DIMX][MAX_DIMX];
float I_minus_KZ_T[MAX_DIMX][MAX_DIMX];
float C_new[MAX_DIMX][MAX_DIMX];

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
void crear_C(float C[MAX_DIMX][MAX_DIMY], float valor_inicial)
{
    for (int i = 0; i < MAX_DIMX; i++) {
        for (int j = 0; j < MAX_DIMY; j++) {
            C[i][j] = 0.0f;
        }
        C[i][i] = valor_inicial;
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
void producto_exterior(const float K[MAX_DIMX],const float z[MAX_DIMX],float M[MAX_DIMX][MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        for (int j = 0; j < MAX_DIMX; j++)
        {
            M[i][j] = K[i] * z[j];
        }
    }
}
void restar_matrices(float A[MAX_DIMX][MAX_DIMX], float B[MAX_DIMX][MAX_DIMX], float R[MAX_DIMX][MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        for (int j = 0; j < MAX_DIMX; j++)
        {
            R[i][j] = A[i][j] - B[i][j];
        }
    }
}
void escalar_matriz_out(const float A[MAX_DIMX][MAX_DIMX], float escalar, float B[MAX_DIMX][MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        for (int j = 0; j < MAX_DIMX; j++)
        {
            B[i][j] = A[i][j] * escalar;
        }
    }
}
void multiplicar_matrices(const float A[MAX_DIMX][MAX_DIMX],const float B[MAX_DIMX][MAX_DIMX],float R[MAX_DIMX][MAX_DIMX])
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
void transponer_matriz(const float A[MAX_DIMX][MAX_DIMX],float AT[MAX_DIMX][MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        for (int j = 0; j < MAX_DIMX; j++)
        {
            AT[j][i] = A[i][j];
        }
    }
}
void copiar_matriz(const float A[MAX_DIMX][MAX_DIMX],float B[MAX_DIMX][MAX_DIMX])
{
    for (int i = 0; i < MAX_DIMX; i++)
    {
        for (int j = 0; j < MAX_DIMX; j++)
        {
            B[i][j] = A[i][j];
        }
    }
}
void simetrizar(float C[MAX_DIMX][MAX_DIMX])
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
void clamp_covariance(float C[MAX_DIMX][MAX_DIMX], float floor, float ceiling)
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