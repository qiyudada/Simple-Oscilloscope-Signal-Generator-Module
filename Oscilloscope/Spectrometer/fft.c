#include "fft.h"

Complex data_of_N_FFT[FFT_N];            //定义存储单元，原始数据与复数结果均使用之 
double SIN_TABLE_of_N_FFT[FFT_N / 4 + 1];

//创建正弦函数表 初始化FFT程序 
void Init_FFT(void)
{
    int i;
    for (i = 0; i <= FFT_N / 4; i++)
    {
        SIN_TABLE_of_N_FFT[i] = sin(2 * i * PI / FFT_N);
    }
}

double Sin_find(double x)
{
    int i = (int)(FFT_N * x);    //注意：i已经转化为0~N之间的整数了！ 
    i = i >> 1;                    // i = i / 2;
    if (i > FFT_N / 4)
    {    //根据FFT相关公式，sin()参数不会超过PI， 即i不会超过N/2 
        i = FFT_N / 2 - i;//i = i - 2*(i-Npart4);
    }
    return SIN_TABLE_of_N_FFT[i];
}

double Cos_find(double x)
{
    int i = (int)(FFT_N*x);//注意：i已经转化为0~N之间的整数了！ 
    i = i >> 1;
    if (i < FFT_N / 4)
    { //不会超过N/2 
        return SIN_TABLE_of_N_FFT[FFT_N / 4 - i];
    }
    else //i > Npart4 && i < N/2 
    {
        return -SIN_TABLE_of_N_FFT[i - FFT_N / 4];
    }
}

//变址 
void ChangeSeat(Complex *DataInput)
{
    int nextValue, nextM, i, k, j = 0;
    Complex temp;

    nextValue = FFT_N / 2;                    //变址运算，即把自然顺序变成倒位序，采用雷德算法
    nextM = FFT_N - 1;
    for (i = 0; i < nextM; i++)
    {
        if (i < j)                            //如果i<j,即进行变址
        {
            temp = DataInput[j];
            DataInput[j] = DataInput[i];
            DataInput[i] = temp;
        }
        k = nextValue;                        //求j的下一个倒位序
        while (k <= j)                        //如果k<=j,表示j的最高位为1
        {
            j = j - k;                        //把最高位变成0
            k = k / 2;                        //k/2，比较次高位，依次类推，逐个比较，直到某个位为0
        }
        j = j + k;                            //把0改为1
    }
}

//FFT运算函数 
void FFT(void)
{
    int L = FFT_N, B, J, K, M_of_N_FFT;
    int step, KB;
    double angle;
    Complex W, Temp_XX;

    ChangeSeat(data_of_N_FFT);                //变址 
                                            //CREATE_SIN_TABLE();
    for (M_of_N_FFT = 1; (L = L >> 1) != 1; ++M_of_N_FFT);    //计算蝶形级数
    for (L = 1; L <= M_of_N_FFT; L++)
    {
        step = 1 << L;                        //2^L
        B = step >> 1;                        //B=2^(L-1)
        for (J = 0; J < B; J++)
        {
            //P = (1<<(M-L))*J;//P=2^(M-L) *J 
            angle = (double)J / B;            //这里还可以优化 
            W.real = Cos_find(angle);         //使用C++时该函数可声明为inline W.real =  cos(angle*PI);
            W.imag = -Sin_find(angle);        //使用C++时该函数可声明为inline W.imag = -sin(angle*PI);

            for (K = J; K < FFT_N; K = K + step)
            {
                KB = K + B;
                //Temp_XX = XX_complex(data[KB],W); 用下面两行直接计算复数乘法，省去函数调用开销 
                Temp_XX.real = data_of_N_FFT[KB].real * W.real - data_of_N_FFT[KB].imag*W.imag;
                Temp_XX.imag = W.imag*data_of_N_FFT[KB].real + data_of_N_FFT[KB].imag*W.real;

                data_of_N_FFT[KB].real = data_of_N_FFT[K].real - Temp_XX.real;
                data_of_N_FFT[KB].imag = data_of_N_FFT[K].imag - Temp_XX.imag;

                data_of_N_FFT[K].real = data_of_N_FFT[K].real + Temp_XX.real;
                data_of_N_FFT[K].imag = data_of_N_FFT[K].imag + Temp_XX.imag;
            }
        }
    }
}

//IFFT运算函数 
void IFFT(void)
{
    int L = FFT_N, B, J, K, M_of_N_FFT;
    int step, KB;
    double angle;
    Complex W, Temp_XX;

    ChangeSeat(data_of_N_FFT);//变址 

    for (M_of_N_FFT = 1; (L = L >> 1) != 1; ++M_of_N_FFT);    //计算蝶形级数
    for (L = 1; L <= M_of_N_FFT; L++)
    {
        step = 1 << L;                        //2^L
        B = step >> 1;                        //B=2^(L-1)
        for (J = 0; J<B; J++)
        {
            angle = (double)J / B;            //这里还可以优化  
            W.real = Cos_find(angle);        //使用C++时该函数可声明为inline W.real = cos(angle*PI);
            W.imag = Sin_find(angle);        //使用C++时该函数可声明为inline W.imag = sin(angle*PI);

            for (K = J; K < FFT_N; K = K + step)
            {
                KB = K + B;
                //用下面两行直接计算复数乘法，省去函数调用开销 
                Temp_XX.real = data_of_N_FFT[KB].real * W.real - data_of_N_FFT[KB].imag*W.imag;
                Temp_XX.imag = W.imag*data_of_N_FFT[KB].real + data_of_N_FFT[KB].imag*W.real;

                data_of_N_FFT[KB].real = data_of_N_FFT[K].real - Temp_XX.real;
                data_of_N_FFT[KB].imag = data_of_N_FFT[K].imag - Temp_XX.imag;

                data_of_N_FFT[K].real = data_of_N_FFT[K].real + Temp_XX.real;
                data_of_N_FFT[K].imag = data_of_N_FFT[K].imag + Temp_XX.imag;
            }
        }
    }
}

/*****************************************************************
函数原型：void Refresh_Data(int id, double wave_data)
函数功能：更新数据
输入参数：id: 标号; wave_data: 一个点的值
输出参数：无
*****************************************************************/
void Refresh_Data(int id, double wave_data)
{
    data_of_N_FFT[id].real = wave_data;
    data_of_N_FFT[id].imag = 0;
}