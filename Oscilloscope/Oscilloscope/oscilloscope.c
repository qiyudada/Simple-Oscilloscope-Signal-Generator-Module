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
#include "oscilloscope.h"

volatile struct Oscilloscope oscilloscope={0};

void Init_Oscilloscope(volatile struct Oscilloscope *value);


/*
*   �������ݣ���ֹUI����
*   ������������
*   ����ֵ��  ��
*/
void TFT_StaticUI(void)
{
    uint16_t i=0,j=0;
    
    char showData[32]={0};
    
    TFT_ShowChinese(10,0,(uint8_t *)"����ʾ����",BLACK,GREEN,16,0);
    
    sprintf(showData,"  PWM ");
    TFT_ShowString(110,0,(uint8_t *)showData,BLACK,YELLOW,16,0);  
    memset(showData,0,32);
    
    TFT_ShowChinese(110,20,(uint8_t *)"���״̬",WHITE,PURPLE,12,0);
    
    sprintf(showData,"      ");
    TFT_ShowString(110,36,(uint8_t *)showData,BLACK,YELLOW,16,0);  
    memset(showData,0,32);
    
    TFT_ShowChinese(110,56,(uint8_t *)"���Ƶ��",WHITE,PURPLE,12,0);
    
    sprintf(showData,"      ");
    TFT_ShowString(110,72,(uint8_t *)showData,BLACK,YELLOW,16,0);  
    memset(showData,0,32);
    
    sprintf(showData,"        ");
    TFT_ShowString(110,92,(uint8_t *)showData,WHITE,PURPLE,12,0);  
    memset(showData,0,32);
    TFT_ShowChinese(118,92,(uint8_t *)"ռ�ձ�",WHITE,PURPLE,12,0);
    
    sprintf(showData,"      ");
    TFT_ShowString(110,106,(uint8_t *)showData,BLACK,YELLOW,16,0);  
    memset(showData,0,32);
    
    TFT_ShowChinese(5,92,(uint8_t *)"�����ֵ",WHITE,PURPLE,12,0);
    
    TFT_ShowChinese(55,92,(uint8_t *)"����Ƶ��",WHITE,PURPLE,12,0);
    
    for(i=0;i<=128;i=i+2)/*margin line*/
    {
        TFT_DrawPoint(106,i,YELLOW);
    }
    
    for(i=0;i<100;i++)
    {
        TFT_DrawPoint(i,81,DARKBLUE);
    }
    for(i=30;i<=80;i++)
    {
        TFT_DrawPoint(0,i,DARKBLUE);
    }
    for(i=0;i<10;i++)
    {
        TFT_DrawPoint((i*10)+2,82,DARKBLUE); 
        TFT_DrawPoint((i*10)+3,82,DARKBLUE); 
    }
    for(i=0;i<10;i++)
    {
        TFT_DrawPoint((i*10)+2,83,DARKBLUE); 
        TFT_DrawPoint((i*10)+3,83,DARKBLUE);
    }
}
/*
*   �������ݣ�  ��ʾ�ַ���
*   ����������  uint16_t vpp--���ֵ
*               uint16_t freq-Ƶ��
*               float DoBias--ֱ��ƫִ�ź�
*   ����ֵ��    ��
*/
void TFT_ShowUI(volatile const struct Oscilloscope *value)
{
    char showData[32]={0};
    
    sprintf(showData,"%1.2fV ",(*value).vpp);
    TFT_ShowString(5,106,(uint8_t *)showData,BLACK,GREEN,16,0);  
    memset(showData,0,32);
    
    if((*value).gatherFreq>=1000)
    {
        sprintf(showData,"%2.0fKHz",(*value).gatherFreq/1000.0f);
        TFT_ShowString(55,106,(uint8_t *)showData,BLACK,GREEN,16,0);  
        memset(showData,0,32);
    }
    else
    {
        sprintf(showData,"%3.0fHz ",(*value).gatherFreq);
        TFT_ShowString(55,106,(uint8_t *)showData,BLACK,GREEN,16,0);  
        memset(showData,0,32);  
    }

    if((*value).ouptputbit == 1)
    {   
        TFT_ShowChinese(118,36,(uint8_t *)"��",BLACK,YELLOW,16,0);
    }
    else
    {
        TFT_ShowChinese(118,36,(uint8_t *)"�ر�",BLACK,YELLOW,16,0);     
    }
    
    if((*value).outputFreq>=1000)
    {
        sprintf(showData,"%3dKHz",(*value).outputFreq/1000);
        TFT_ShowString(110,72,(uint8_t *)showData,BLACK,YELLOW,16,0);  
        memset(showData,0,32); 
    }
    else
    {
        sprintf(showData," %3dHz",(*value).outputFreq);
        TFT_ShowString(110,72,(uint8_t *)showData,BLACK,YELLOW,16,0);  
        memset(showData,0,32);
    }  

    sprintf(showData,"%3.1f%%  ",(((*value).pwmOut)/((*value).timerPeriod+0.0f))*100);
    TFT_ShowString(110,106,(uint8_t *)showData,BLACK,YELLOW,16,0);  
    memset(showData,0,32);        
}



/*
*   �������ݣ�����������
*   ������������
*   ����ֵ��  ��
*/
void Key_Handle(volatile struct Oscilloscope *value)
{
    uint8_t i=0,j=0;
    float tempValue=0;
	switch((*value).keyValue)
	{
		case KEY1:
            (*value).pwmOut=((*value).timerPeriod*0.05f)+(*value).pwmOut;
            if((*value).pwmOut > (*value).timerPeriod)
            {
                (*value).pwmOut = 0;
            }
            Set_Output_PWMComparex((*value).pwmOut);
			break;
        case KEY3:
            tempValue=(*value).pwmOut/((*value).timerPeriod+0.0f);
            (*value).timerPeriod = (*value).timerPeriod/2.0f;
            if((*value).timerPeriod < 250)
            {
                (*value).timerPeriod = 1000;
            }
            (*value).outputFreq=1000000/(*value).timerPeriod;
            (*value).pwmOut=(*value).timerPeriod*tempValue;
            Set_Output_PWMComparex((*value).pwmOut);
            Set_Output_Freq((*value).timerPeriod-1);
            tempValue=0;
			break;
        case KEY2:
            if((*value).ouptputbit == 0)
            {
                (*value).ouptputbit=1;
                timer_enable(TIMER14);
            }
            else
            {
                (*value).ouptputbit=0;
               timer_disable(TIMER14); 
            }
            break;
        case KEYA:
            switch((*value).sampletime)
            {
                case ADC_SAMPLETIME_239POINT5:
                    (*value).sampletime=ADC_SAMPLETIME_71POINT5;
                    break;
                case ADC_SAMPLETIME_71POINT5:
                    (*value).sampletime=ADC_SAMPLETIME_55POINT5;
                    break;
                case ADC_SAMPLETIME_55POINT5:
                    (*value).sampletime=ADC_SAMPLETIME_41POINT5;
                    break;
                case ADC_SAMPLETIME_41POINT5:
                    (*value).sampletime=ADC_SAMPLETIME_28POINT5;
                    break;
                case ADC_SAMPLETIME_28POINT5:
                    (*value).sampletime=ADC_SAMPLETIME_28POINT5;
                    break;
                default:
                    (*value).sampletime=ADC_SAMPLETIME_239POINT5;
                    break;
            }
            //ADC����ͨ������--PA3��˳����0��ͨ��3������ʱ��
            adc_regular_channel_config(0, ADC_CHANNEL_3, (*value).sampletime);            
            break;
        case KEYB:
            switch((*value).sampletime)
            {
                case ADC_SAMPLETIME_239POINT5:
                    (*value).sampletime=ADC_SAMPLETIME_239POINT5;
                    break;
                case ADC_SAMPLETIME_71POINT5:
                    (*value).sampletime=ADC_SAMPLETIME_239POINT5;
                    break;
                case ADC_SAMPLETIME_55POINT5:
                    (*value).sampletime=ADC_SAMPLETIME_71POINT5;
                    break;
                case ADC_SAMPLETIME_41POINT5:
                    (*value).sampletime=ADC_SAMPLETIME_55POINT5;
                    break;
                case ADC_SAMPLETIME_28POINT5:
                    (*value).sampletime=ADC_SAMPLETIME_41POINT5;
                    break;
                default:
                    (*value).sampletime=ADC_SAMPLETIME_239POINT5;
                    break;
            }    
            //ADC����ͨ������--PA3��˳����0��ͨ��3������ʱ��
            adc_regular_channel_config(0, ADC_CHANNEL_3, (*value).sampletime);   
            break;
        case KEYD:
            break;
		default:
			break;
	}
    (*value).keyValue=0;
    //������ʾUI
    TFT_ShowUI(value); 
}

//Ec11 ��ת�ص�
void OnEc11RotateOscillo (int direct)
{
    if (direct>0)
    {
        oscilloscope.keyValue=KEYB;
    }
    else
    {
        oscilloscope.keyValue=KEYA;
    }
}

void OnEc11ClickOscillo (void)
{
    oscilloscope.keyValue=KEYD;
}

void OnKey1Click(void)
{
    oscilloscope.keyValue=KEY1;
}
void OnKey2Click(void)
{
    oscilloscope.keyValue=KEY2;
}
void OnKey3Click(void)
{
    oscilloscope.keyValue=KEY3;
}

// DMA �жϻص�
void OnDMAInt()
{
    oscilloscope.showbit = 1;
}


void InitOscillo(void)
{
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
    
    Set_EC11_Rotate_Handler(OnEc11RotateOscillo);
    Set_EC11_Click_Handler(OnEc11ClickOscillo);
    Set_Key1_Click_Handler(OnKey1Click);
    Set_Key2_Click_Handler(OnKey2Click);
    Set_Key3_Click_Handler(OnKey3Click);
    
    Set_DMA_Int_Handler(OnDMAInt);
    
    //��ʼ��Ƶ�ʶ�ʱ��2
    Init_FreqTimer();
    
    //��ʼ����̬UI
    TFT_StaticUI();
}

void RunOscillo()
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
                Draw_Curve(80,voltage);
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

