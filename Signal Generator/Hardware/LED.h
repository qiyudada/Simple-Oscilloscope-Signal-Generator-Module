#ifndef __LED_H
#define __LED_H

#define LED1_PORT GPIOC
#define LED1_PIN GPIO_Pin_13
#define LED2_PORT GPIOC
#define LED2_PIN GPIO_Pin_14

void LED_Init(void);
void LED1_ON(void);
void LED1_OFF(void);
void LED1_Toggle(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED2_Toggle(void);
void LED1_Show(void);
void LED2_Show(void);



#endif
