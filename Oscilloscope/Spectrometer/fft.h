#ifndef __FFT_H_
#define __FFT_H_

#include <math.h>

#define PI 3.14159265358979323846264338327950288419716939937510    //圆周率

#define FFT_N 64        //傅里叶变换的点数 

#define FFT_RESULT(x) (sqrt(data_of_N_FFT[x].real*data_of_N_FFT[x].real+data_of_N_FFT[x].imag*data_of_N_FFT[x].imag)/ (FFT_N >> (x != 0)))
#define FFT_Hz(x, Sample_Frequency) ((double)(x * Sample_Frequency) / FFT_N)

#define IFFT_RESULT(x) (data_of_N_FFT[x].real / FFT_N)

typedef struct                        //定义复数结构体 
{
    double real, imag;
}Complex;

extern Complex data_of_N_FFT[];    
extern double SIN_TABLE_of_N_FFT[];

void Init_FFT(void);
void FFT(void);
void IFFT(void);
void Refresh_Data(int id, double wave_data);

double Sin_find(double x);
double Cos_find(double x);

#endif // !FFT_H_