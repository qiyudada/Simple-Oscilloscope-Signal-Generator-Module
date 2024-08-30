#include "gd32e23x.h"
#include "systick.h"
#include <string.h>
#include <stdio.h>
#include "led.h"
#include "tft.h"
#include "tft_init.h"
#include "adc.h"
#include "usart.h"
#include "timer.h"
#include "key.h"
#include "ec11.h"
#include "freq.h"
#include "spectrometer.h"
#include "fft.h"


double FFTHistory[FFT_N/2] = {0.0};

int sStartX = -5;
int sStartY = 40;
int sEndY = 120;

int sBarWidth = 4;
int sBarHeight = 0; // sBarHeight = sEndY - sStartY;

float scale = 1.0;


// �ɼ��㹻���ݱ�־
volatile int fftSampleFulfill = 0;

/*
*   �������ݣ���ֹUI����
*   ������������
*   ����ֵ��  ��
*/
void TFT_SpectroStaticUI(void)
{
    char showData[32]={0};
    sprintf(showData,"    SPECTROMETER    ");
    TFT_ShowString(0, 10, (uint8_t *)showData, BLACK, LIGHTBLUE,16,0); 
    memset(showData,0, 32);
}


// DMA �жϻص�
void OnDMAIntSpectro()
{
	timer_disable(TIMER0);
    fftSampleFulfill = 1;
}


// ��ת��ť 
void OnEc11RotateSpectroHandler(int direct)
{
    if (direct > 0)
        scale += 0.2;
    else
        scale -= 0.2;
    
    scale = fmax(scale, 1.0);
    scale = fmin(scale, 4.0);
}

void InitSpectro(void)
{   
    sBarHeight = sEndY - sStartY;
    
    //ʱ�ӳ�ʼ��
    systick_config();
    
    //LED��ʼ��
    Init_LED_GPIO();
    
    //��Ļ��ʼ��
    TFT_Init();
    
    //����ɫ
    TFT_Fill(0,0,160,128,BLACK);
    
    //��ʼ������
    Init_USART(115200);
    
    Init_Spectro_ADC();
    Init_Spectro_ADC_DMA();
    Init_Spector_ADC_Timer();

    //��ʼ��PWM���
    Init_PWM_Output(999,500);
    
    //��ʼ��EC11����
    Init_EC11_GPIO();
    
    //��ʼ����������
    Init_Key_GPIO();
    
    Set_EC11_Rotate_Handler(OnEc11RotateSpectroHandler);
    Set_DMA_Int_Handler(OnDMAIntSpectro);
    
    //��ʼ��Ƶ�ʶ�ʱ��2
    Init_FreqTimer();
    
    //��ʼ����̬UI
    TFT_SpectroStaticUI();
    
    //��ʼ��FFT
    Init_FFT();
}

void RunSpectro()
{
    uint16_t i=0;
    int iAmp = 0;
    
    while(1)
    { 
        
        //�����ȡ��ѹֵ��ɣ���ʼˢ��
        if(fftSampleFulfill==1)
        {           
            fftSampleFulfill=0;
            
            /////////////////////////////////
            // ��ʼ���Ӻ�����
            for (i = 0 ; i < FFT_N ; i++)
            {
                double x = i <= FFT_N /2 ? 2.0 * i / FFT_N : 2.0 * (FFT_N - i) /FFT_N;
                data_of_N_FFT[i].real = Get_ADC_Value(i) * (0.5 - 0.5 * Cos_find(x));
                data_of_N_FFT[i].imag = 0.0;
            }
            
            // FFT
            FFT();
            
            // ȡ���
            for(i = 2 ; i < FFT_N / 2 ; i++)
            {
                FFTHistory[i] = FFTHistory[i] * 0.7 + FFT_RESULT(i) * 0.3;
            }
            
            
            //ˢ����ͬʱ��ȡ��ѹֵ
            dma_transfer_number_config(DMA_CH0, 64);
            dma_channel_enable(DMA_CH0);
            timer_enable(TIMER0);
            
            // ��ʾ
            for (i = 2; i < FFT_N / 2 ; i ++)
            {
                iAmp = (int) (FFTHistory[i] * scale);
                iAmp = iAmp > sBarHeight ? sBarHeight :iAmp ;
                TFT_Fill( sStartX + i*(sBarWidth + 1), sStartY ,sStartX + (i+1) * (sBarWidth + 1) -1 , sEndY, 0b0011100101100111);
                TFT_Fill( sStartX + i*(sBarWidth + 1), sEndY - iAmp ,sStartX + (i+1) * (sBarWidth + 1) -1 , sEndY, BLUE); 
            }
            
        }
            
            //////////////////////////////////
    }
}

