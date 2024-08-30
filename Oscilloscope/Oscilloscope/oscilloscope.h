#ifndef __OSCILLOSCOPE_H
#define __OSCILLOSCOPE_H

#include "gd32e23x.h"

struct Oscilloscope
{
   uint8_t showbit;         //画面更新标志位
   uint8_t keyValue;        //按键值
   uint8_t ouptputbit;      //输出标志位
   uint16_t outputFreq;     //波形输出频率
   uint16_t pwmOut;         //PWM引脚输出的PWM占空比
   uint32_t sampletime;     //电压采集时间
   uint32_t timerPeriod;    //定时器周期设置
   float gatherFreq;        //示波器采集频率
   float vpp;               //峰峰值
   float voltageValue[300]; //ADC采集电压值
};
extern volatile struct Oscilloscope oscilloscope;

void InitOscillo(void);
void RunOscillo(void);
#endif /* __OSCILLOSCOPE_H */