#ifndef __USART_H
#define __USART_H

#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>

void Init_USART(uint32_t bound);
void Init_USART0RecDMA(void);
void USART0_SendData(char *SendData);
void USART0_RecHandle(void);
#endif