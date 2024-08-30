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


// 采集足够数据标志
volatile int fftSampleFulfill = 0;

/*
*   函数内容：静止UI界面
*   函数参数：无
*   返回值：  无
*/
void TFT_SpectroStaticUI(void)
{
    char showData[32]={0};
    sprintf(showData,"    SPECTROMETER    ");
    TFT_ShowString(0, 10, (uint8_t *)showData, BLACK, LIGHTBLUE,16,0); 
    memset(showData,0, 32);
}


// DMA 中断回调
void OnDMAIntSpectro()
{
	timer_disable(TIMER0);
    fftSampleFulfill = 1;
}


// 旋转旋钮 
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
    
    //时钟初始化
    systick_config();
    
    //LED初始化
    Init_LED_GPIO();
    
    //屏幕初始化
    TFT_Init();
    
    //填充白色
    TFT_Fill(0,0,160,128,BLACK);
    
    //初始化串口
    Init_USART(115200);
    
    Init_Spectro_ADC();
    Init_Spectro_ADC_DMA();
    Init_Spector_ADC_Timer();

    //初始化PWM输出
    Init_PWM_Output(999,500);
    
    //初始化EC11引脚
    Init_EC11_GPIO();
    
    //初始化按键引脚
    Init_Key_GPIO();
    
    Set_EC11_Rotate_Handler(OnEc11RotateSpectroHandler);
    Set_DMA_Int_Handler(OnDMAIntSpectro);
    
    //初始化频率定时器2
    Init_FreqTimer();
    
    //初始化静态UI
    TFT_SpectroStaticUI();
    
    //初始化FFT
    Init_FFT();
}

void RunSpectro()
{
    uint16_t i=0;
    int iAmp = 0;
    
    while(1)
    { 
        
        //如果获取电压值完成，开始刷屏
        if(fftSampleFulfill==1)
        {           
            fftSampleFulfill=0;
            
            /////////////////////////////////
            // 初始化加汉明窗
            for (i = 0 ; i < FFT_N ; i++)
            {
                double x = i <= FFT_N /2 ? 2.0 * i / FFT_N : 2.0 * (FFT_N - i) /FFT_N;
                data_of_N_FFT[i].real = Get_ADC_Value(i) * (0.5 - 0.5 * Cos_find(x));
                data_of_N_FFT[i].imag = 0.0;
            }
            
            // FFT
            FFT();
            
            // 取结果
            for(i = 2 ; i < FFT_N / 2 ; i++)
            {
                FFTHistory[i] = FFTHistory[i] * 0.7 + FFT_RESULT(i) * 0.3;
            }
            
            
            //刷屏的同时获取电压值
            dma_transfer_number_config(DMA_CH0, 64);
            dma_channel_enable(DMA_CH0);
            timer_enable(TIMER0);
            
            // 显示
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

