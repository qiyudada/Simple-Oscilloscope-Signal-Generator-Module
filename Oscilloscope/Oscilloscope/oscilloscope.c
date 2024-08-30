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
*   函数内容：静止UI界面
*   函数参数：无
*   返回值：  无
*/
void TFT_StaticUI(void)
{
    uint16_t i=0,j=0;
    
    char showData[32]={0};
    
    TFT_ShowChinese(10,0,(uint8_t *)"简易示波器",BLACK,GREEN,16,0);
    
    sprintf(showData,"  PWM ");
    TFT_ShowString(110,0,(uint8_t *)showData,BLACK,YELLOW,16,0);  
    memset(showData,0,32);
    
    TFT_ShowChinese(110,20,(uint8_t *)"输出状态",WHITE,PURPLE,12,0);
    
    sprintf(showData,"      ");
    TFT_ShowString(110,36,(uint8_t *)showData,BLACK,YELLOW,16,0);  
    memset(showData,0,32);
    
    TFT_ShowChinese(110,56,(uint8_t *)"输出频率",WHITE,PURPLE,12,0);
    
    sprintf(showData,"      ");
    TFT_ShowString(110,72,(uint8_t *)showData,BLACK,YELLOW,16,0);  
    memset(showData,0,32);
    
    sprintf(showData,"        ");
    TFT_ShowString(110,92,(uint8_t *)showData,WHITE,PURPLE,12,0);  
    memset(showData,0,32);
    TFT_ShowChinese(118,92,(uint8_t *)"占空比",WHITE,PURPLE,12,0);
    
    sprintf(showData,"      ");
    TFT_ShowString(110,106,(uint8_t *)showData,BLACK,YELLOW,16,0);  
    memset(showData,0,32);
    
    TFT_ShowChinese(5,92,(uint8_t *)"输入幅值",WHITE,PURPLE,12,0);
    
    TFT_ShowChinese(55,92,(uint8_t *)"输入频率",WHITE,PURPLE,12,0);
    
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
*   函数内容：  显示字符串
*   函数参数：  uint16_t vpp--峰峰值
*               uint16_t freq-频率
*               float DoBias--直流偏执信号
*   返回值：    无
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
        TFT_ShowChinese(118,36,(uint8_t *)"打开",BLACK,YELLOW,16,0);
    }
    else
    {
        TFT_ShowChinese(118,36,(uint8_t *)"关闭",BLACK,YELLOW,16,0);     
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
*   函数内容：按键处理函数
*   函数参数：无
*   返回值：  无
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
            //ADC常规通道配置--PA3，顺序组0，通道3，采样时间
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
            //ADC常规通道配置--PA3，顺序组0，通道3，采样时间
            adc_regular_channel_config(0, ADC_CHANNEL_3, (*value).sampletime);   
            break;
        case KEYD:
            break;
		default:
			break;
	}
    (*value).keyValue=0;
    //参数显示UI
    TFT_ShowUI(value); 
}

//Ec11 旋转回调
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

// DMA 中断回调
void OnDMAInt()
{
    oscilloscope.showbit = 1;
}


void InitOscillo(void)
{
    //初始化示波器参数
    Init_Oscilloscope(&oscilloscope);
    
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
    
    //ADC DMA初始化
    ADC_DMA_Init();
    
    //初始化ADC引脚
    Init_ADC();
    
    //初始化PWM输出
    Init_PWM_Output(oscilloscope.timerPeriod-1,oscilloscope.pwmOut);
    
    //初始化EC11引脚
    Init_EC11_GPIO();
    
    //初始化按键引脚
    Init_Key_GPIO();
    
    Set_EC11_Rotate_Handler(OnEc11RotateOscillo);
    Set_EC11_Click_Handler(OnEc11ClickOscillo);
    Set_Key1_Click_Handler(OnKey1Click);
    Set_Key2_Click_Handler(OnKey2Click);
    Set_Key3_Click_Handler(OnKey3Click);
    
    Set_DMA_Int_Handler(OnDMAInt);
    
    //初始化频率定时器2
    Init_FreqTimer();
    
    //初始化静态UI
    TFT_StaticUI();
}

void RunOscillo()
{
    uint16_t i=0;
    
    //中间值
    float median=0;
    
    //峰峰值
    float voltage=0;
    
    //触发电压值
    float max_data=1.0f;
    
    //波形放大倍数
    float gainFactor=0;
	
    float adcValue = 0;
    
    //触发沿标记
    uint16_t Trigger_number=0;
    
    
    while(1)
    {  
        //按键扫描处理函数
        Key_Handle(&oscilloscope);
        
        //如果获取电压值完成，开始刷屏
        if(oscilloscope.showbit==1)
        {           
            oscilloscope.showbit=0;
            oscilloscope.vpp=0;
            
            //转换电压值
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
            
            //刷屏的同时获取电压值
            dma_transfer_number_config(DMA_CH0, 300);
            dma_channel_enable(DMA_CH0);
            
            //找到起始显示波形值
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
            
            //如果幅值过小，会出现放大倍数过大导致波形显示异常的问题
            if(oscilloscope.vpp > 0.3)
            {
                //获取中间值
                median = oscilloscope.vpp / 2.0f;
							
                //放大倍数，需要确定放大之后的区间，我将波形固定显示在（18.75~41.25中），(41.25-18.75)/2=11.25f
                gainFactor = 11.25f/median;
              
            }
            
            //依次显示后续100个数据，这样可以防止波形滚动
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
        //参数显示UI
        TFT_ShowUI(&oscilloscope); 
        
    }
}

/*
*   函数内容：初始化示波器参数结构体
*   函数参数：volatile struct Oscilloscope *value--示波器参数结构体指针
*   返回值：无
*/
void Init_Oscilloscope(volatile struct Oscilloscope *value)
{
    (*value).showbit    =0;                         //清除显示标志位
    (*value).sampletime =ADC_SAMPLETIME_239POINT5;  //adc采样周期
    (*value).keyValue   =0;                         //清楚按键值
    (*value).ouptputbit =0;                         //输出标志位
    (*value).gatherFreq =0;                         //采集频率
    (*value).outputFreq =1000;                      //输出频率
    (*value).pwmOut     =500;                       //PWM引脚输出的PWM占空比
    (*value).timerPeriod=1000;                      //PWM输出定时器周期
    (*value).vpp        =0.0f;                      //峰峰值
}

