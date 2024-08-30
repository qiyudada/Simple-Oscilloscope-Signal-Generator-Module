#include "stm32f10x.h" // Device header
#include "Key.h"
#include "Delay.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY_ADD_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_ADD_PORT, &GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY_DEF_PIN | KEY_SUB_PIN| KEY_DUTY_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_SUB_PORT, &GPIO_InitStructure);

	GPIO_SetBits(KEY_ADD_PORT,KEY_ADD_PIN);
	GPIO_SetBits(KEY_SUB_PORT,KEY_DEF_PIN | KEY_SUB_PIN| KEY_DUTY_PIN);
}

int Key_GetNum(void)
{
	int KeyNum = KEY_NONE;
	if (GPIO_ReadInputDataBit(KEY_ADD_PORT, KEY_ADD_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY_ADD_PORT, KEY_ADD_PIN) == 0)
		{
		}
		Delay_ms(20);
		KeyNum = KEY_ADD;
	}
	if (GPIO_ReadInputDataBit(KEY_DEF_PORT, KEY_DEF_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY_DEF_PORT, KEY_DEF_PIN) == 0)
		{
		}
		Delay_ms(20);
		KeyNum = KEY_DEF;
	}
	if (GPIO_ReadInputDataBit(KEY_SUB_PORT, KEY_SUB_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY_SUB_PORT, KEY_SUB_PIN) == 0)
		{
		}
		Delay_ms(20);
		KeyNum = KEY_SUB;
	}
	if (GPIO_ReadInputDataBit(KEY_DUTY_PORT, KEY_DUTY_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY_DUTY_PORT, KEY_DUTY_PIN) == 0)
		{
		}
		Delay_ms(20);
		KeyNum = KEY_DUTY;
	}
	return KeyNum;
}
