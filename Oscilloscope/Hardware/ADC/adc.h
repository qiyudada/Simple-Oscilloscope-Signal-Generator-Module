#ifndef __ADC_H
#define __ADC_H

#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>
#include "oscilloscope.h"

typedef void (*DMAIntHandler)(void);

uint16_t Get_ADC_Value(uint16_t value);
void Init_ADC(void);
void ADC_DMA_Init(void);
void ADC_Config(void);

void Init_Spectro_ADC(void);
void Init_Spectro_ADC_DMA(void);
void Init_Spector_ADC_Timer();
    
void Set_DMA_Int_Handler(DMAIntHandler handler);

unsigned int Get_ADC_Value2(uint8_t  ADC_CHANNEL_x);
#endif