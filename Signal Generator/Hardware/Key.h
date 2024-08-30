#ifndef __KEY_H
#define __KEY_H

#define KEY_ADD_PORT GPIOB
#define KEY_ADD_PIN GPIO_Pin_12
#define KEY_DEF_PORT GPIOB
#define KEY_DEF_PIN GPIO_Pin_15
#define KEY_SUB_PORT GPIOB
#define KEY_SUB_PIN GPIO_Pin_14
#define KEY_DUTY_PORT GPIOB
#define KEY_DUTY_PIN GPIO_Pin_13

#define KEY_NONE 0
#define KEY_ADD 1
#define KEY_DEF 2
#define KEY_SUB 3
#define KEY_DUTY 4

void Key_Init(void);
int Key_GetNum(void);

#endif
