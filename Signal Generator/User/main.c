#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "Key.h"
#include "LED.h"
#include "OLED.h"
#include "PWM.h"
#include "IC.h"

#define Default_PSC (719)
#define Default_CCR (50)

static int Key_Value = 0;
int main(void)
{

	OLED_Init();
	PWM_Init();
	IC_Init();
	Key_Init();
	LED_Init();

	OLED_ShowString(1, 1, "Freq:00000Hz");
	OLED_ShowString(2, 1, "Duty:00%");
	OLED_ShowString(3, 1, "Freq:00000Hz");
	OLED_ShowString(4, 1, "Duty:00%");

	/*Default value*/
	TIM2_PWM_SetPrescaler(Default_PSC); // Freq = 72M / (PSC + 1) / 100
	TIM2_PWM_SetCompare1(Default_CCR);	// Duty = CCR / 100

	Pwm_Component.PSC1 = Default_PSC;
	Pwm_Component.CCR1 = Default_CCR;

	while (1)
	{
		/*Output control*/
		Key_Value = Key_GetNum();
		if (Key_Value == KEY_ADD)
		{
			LED1_Show();
			Pwm_Component.PSC1 += 100;
			TIM2_PWM_SetPrescaler(Pwm_Component.PSC1);
		}
		if (Key_Value == KEY_SUB)
		{
			LED2_Show();
			Pwm_Component.PSC1 -= 100;
			if(Pwm_Component.PSC1<=0)
			{
				Pwm_Component.PSC1= Default_PSC;
			}
			TIM2_PWM_SetPrescaler(Pwm_Component.PSC1);
		}

		if (Key_Value == KEY_DEF)
		{
			LED2_Show();
			Pwm_Component.PSC1 = Default_PSC;
			Pwm_Component.CCR1 = Default_CCR;
			TIM2_PWM_SetPrescaler(Pwm_Component.PSC1);
			TIM2_PWM_SetCompare1(Pwm_Component.CCR1);
		}
		if (Key_Value == KEY_DUTY)
		{
			LED1_Show();
			Pwm_Component.CCR1 +=5;
			TIM2_PWM_SetCompare1(Pwm_Component.CCR1);
			if (Pwm_Component.CCR1 >= 100)
			{
				Pwm_Component.CCR1 = 5;
				TIM2_PWM_SetCompare1(Pwm_Component.CCR1);
				
			}
		}
		Key_Value = 0;
		/*Input Capture*/
		OLED_ShowNum(1, 6, IC_GetFreq(), 5);
		OLED_ShowNum(2, 6, IC_GetDuty(), 2);
		OLED_ShowNum(3, 6, TIM2_PWM_GetCompare1(), 5);
		OLED_ShowNum(4, 6, TIM2_PWM_GetDuty(), 2);
		Delay_ms(50);
	}
}
