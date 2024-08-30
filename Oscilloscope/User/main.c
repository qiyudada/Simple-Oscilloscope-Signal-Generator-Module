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
#include "freq.h"
#include "main.h"

volatile struct Oscilloscope oscilloscope={0};

void Init_Oscilloscope(volatile struct Oscilloscope *value);

int main(void)
{      
    uint16_t i=0;
    
    //�м�ֵ
    float median=0;
    
    //���ֵ
    float voltage=0;
    
    //������ѹֵ
    float max_data=1.0f;
    
    //���ηŴ���
    float gainFactor=0;
	
		float adcValue = 0;
    
    //�����ر��
    uint16_t Trigger_number=0;
    
    //��ʼ��ʾ��������
    Init_Oscilloscope(&oscilloscope);
    
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
    
    //ADC DMA��ʼ��
    ADC_DMA_Init();
    
    //��ʼ��ADC����
    Init_ADC();
    
    //��ʼ��PWM���
    Init_PWM_Output(oscilloscope.timerPeriod-1,oscilloscope.pwmOut);
    
    //��ʼ��EC11����
    Init_EC11_GPIO();
    
    //��ʼ����������
    Init_Key_GPIO();
    
    //��ʼ��Ƶ�ʶ�ʱ��2
    Init_FreqTimer();
    
    //��ʼ����̬UI
    TFT_StaticUI();
    while(1)
    {  
        //����ɨ�账����
        Key_Handle(&oscilloscope);
        
        //�����ȡ��ѹֵ��ɣ���ʼˢ��
        if(oscilloscope.showbit==1)
        {           
            oscilloscope.showbit=0;
            oscilloscope.vpp=0;
            
            //ת����ѹֵ
            for(i=0;i<300;i++)
            {
								adcValue = (Get_ADC_Value(i)*3.3f)/4096.0f;
							
								oscilloscope.voltageValue[i] = (5-(2.0f*adcValue));
							
                if((oscilloscope.vpp) < oscilloscope.voltageValue[i])
                {
                    oscilloscope.vpp = oscilloscope.voltageValue[i];
                }
                if(oscilloscope.vpp <= 0.3)
                {
                    oscilloscope.gatherFreq=0;
                }
            }
            
            //ˢ����ͬʱ��ȡ��ѹֵ
            dma_transfer_number_config(DMA_CH0, 300);
            dma_channel_enable(DMA_CH0);
            
            //�ҵ���ʼ��ʾ����ֵ
            for(i=0;i<200;i++)
            {
                if(oscilloscope.voltageValue[i] < max_data)
                {
                    for(;i<200;i++)
                    {
                        if(oscilloscope.voltageValue[i] > max_data)
                        {
                            Trigger_number=i;
                            break;
                        }
                    }
                    break;
                }
            }
            
            //�����ֵ��С������ַŴ��������²�����ʾ�쳣������
            if(oscilloscope.vpp > 0.3)
            {
								//��ȡ�м�ֵ
								median = oscilloscope.vpp / 2.0f;
							
                //�Ŵ�������Ҫȷ���Ŵ�֮������䣬�ҽ����ι̶���ʾ�ڣ�18.75~41.25�У���(41.25-18.75)/2=11.25f
                gainFactor = 11.25f/median;
              
            }
            
            //������ʾ����100�����ݣ��������Է�ֹ���ι���
            for(i=Trigger_number;i<Trigger_number+100;i++)
            {
                if(oscilloscope.keyValue == KEYD)
                {
                    oscilloscope.keyValue=0;
                    do
                    {
                        if(oscilloscope.keyValue == KEYD){
                            oscilloscope.keyValue=0;
                            break;
                        }
                    }while(1);
                }
                voltage=oscilloscope.voltageValue[i];

                if(voltage >= median)
                {
                    voltage = 30 - (voltage - median)*gainFactor;
                }
                else
                {
                    voltage = 30 + (median - voltage)*gainFactor;
                }
                drawCurve(80,voltage);
            }          
        }        
        //������ʾUI
        TFT_ShowUI(&oscilloscope); 
        
    }
}

/*
*   �������ݣ���ʼ��ʾ���������ṹ��
*   ����������volatile struct Oscilloscope *value--ʾ���������ṹ��ָ��
*   ����ֵ����
*/
void Init_Oscilloscope(volatile struct Oscilloscope *value)
{
    (*value).showbit    =0;                         //�����ʾ��־λ
    (*value).sampletime =ADC_SAMPLETIME_239POINT5;  //adc��������
    (*value).keyValue   =0;                         //�������ֵ
    (*value).ouptputbit =0;                         //�����־λ
    (*value).gatherFreq =0;                         //�ɼ�Ƶ��
    (*value).outputFreq =1000;                      //���Ƶ��
    (*value).pwmOut     =500;                       //PWM���������PWMռ�ձ�
    (*value).timerPeriod=1000;                      //PWM�����ʱ������
    (*value).vpp        =0.0f;                      //���ֵ
}


