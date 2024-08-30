#ifndef __PWM_H
#define __PWM_H

#define SYSCPUTICK 72000000

typedef struct
{
    int ARR1;
	int PSC1;
	int CCR1;
} PWM_TypeDef;

extern PWM_TypeDef Pwm_Component;

void PWM_Init(void);
void TIM2_PWM_SetCompare1(uint16_t Compare);
void TIM2_PWM_SetPrescaler(uint16_t Prescaler);
int TIM2_PWM_GetCompare1(void);
int TIM2_PWM_GetDuty(void);

#endif
