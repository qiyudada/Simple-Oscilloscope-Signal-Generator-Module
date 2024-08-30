#include "stm32f10x.h" // Device header
#include "LED.h"
#include "delay.h"

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED1_PIN | LED2_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED1_PORT, &GPIO_InitStructure);

	GPIO_SetBits(LED1_PORT, LED1_PIN | LED2_PIN);
}

void LED1_ON(void)
{
	GPIO_ResetBits(LED1_PORT, LED1_PIN);
}

void LED1_OFF(void)
{
	GPIO_SetBits(LED1_PORT, LED1_PIN);
}

void LED1_Show(void)
{
	LED1_ON();
	Delay_ms(20);
	LED1_OFF();
}

void LED2_Show(void)
{
	LED2_ON();
	Delay_ms(20);
	LED2_OFF();
}
void LED1_Toggle(void)
{
	if (GPIO_ReadOutputDataBit(LED1_PORT, LED1_PIN) == 0)
	{
		GPIO_SetBits(LED1_PORT, LED1_PIN);
	}
	else
	{
		GPIO_ResetBits(LED1_PORT, LED1_PIN);
	}
}

void LED2_ON(void)
{
	GPIO_ResetBits(LED2_PORT, LED2_PIN);
}

void LED2_OFF(void)
{
	GPIO_SetBits(LED2_PORT, LED2_PIN);
}

void LED2_Toggle(void)
{
	if (GPIO_ReadOutputDataBit(LED2_PORT, LED2_PIN) == 0)
	{
		GPIO_SetBits(LED2_PORT, LED2_PIN);
	}
	else
	{
		GPIO_ResetBits(LED2_PORT, LED2_PIN);
	}
}
