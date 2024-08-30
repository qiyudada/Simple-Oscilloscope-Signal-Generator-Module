#include "key.h"
#include "timer.h"
#include "tft.h"

static uint8_t keyValue=0;

/*
*   函数内容：初始化按键GPIO
*   函数参数：无
*   返回值：  无
*/
void Init_Key_GPIO(void)
{
	//使能时钟
    rcu_periph_clock_enable(RCU_GPIOB);
	
	//设置输出模式，上拉
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3|GPIO_PIN_9|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

    //中断线使能
    nvic_irq_enable(EXTI4_15_IRQn,3U);
    
    //配置中断线
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN13);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN14);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN15);
    
    //初始化中断线，设置为中断模式，上升沿触发
    exti_init(EXTI_13,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    exti_init(EXTI_14,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    exti_init(EXTI_15,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    
    //中断标志位清除
    exti_interrupt_flag_clear(EXTI_13);
    exti_interrupt_flag_clear(EXTI_14);
    exti_interrupt_flag_clear(EXTI_15);
}

/*
*   函数内容：初始化EC11 GPIO
*   函数参数：无
*   返回值：  无
*/
//void Init_EC11_GPIO(void)
//{
//	//使能时钟GPIOB，CMP
//    rcu_periph_clock_enable(RCU_GPIOB);
//	rcu_periph_clock_enable(RCU_CFGCMP);
//	
//	//设置引脚模式，上拉
//	gpio_mode_set(GPIOB,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_PIN_4);
//	
//	//中断线使能
//	nvic_irq_enable(EXTI4_15_IRQn,3U);
//	
//	//配置中断线
//	syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN4);
//	
//	//初始化中断线，设置为中断模式，上升沿触发
//	exti_init(EXTI_4,EXTI_INTERRUPT,EXTI_TRIG_RISING);
//	
//	//中断标志位清除
//	exti_interrupt_flag_clear(EXTI_4);

//    //中断线使能
//    exti_interrupt_enable(EXTI_4);
//    
//	//中断标志位清除
//	exti_interrupt_flag_clear(EXTI_4);    
//}



//extern volatile struct Oscilloscope oscilloscope;

//void EXTI4_15_IRQHandler(void)
//{
//    static uint8_t clockwiseNum=0;
//    static uint8_t anticlockwiseNum=0;
//    if(RESET != exti_interrupt_flag_get(EXTI_4)) 
//    {
//        if(gpio_input_bit_get(GPIOB,GPIO_PIN_3)==RESET) //顺时针转两格
//        {
//            delay_1ms(5);
//            anticlockwiseNum=0;
//            if(gpio_input_bit_get(GPIOB,GPIO_PIN_3)==RESET) //顺时针转两格
//            {
//                clockwiseNum++;
//                if(clockwiseNum>=2)
//                {
//                    clockwiseNum=0;
//                    oscilloscope.keyValue=KEYB; 
//                }            
//            }
//        }
//        else                                            //逆时针转两格
//        {
//            delay_1ms(5);
//            clockwiseNum=0;
//            if(gpio_input_bit_get(GPIOB,GPIO_PIN_3)==SET) //逆时针转两格
//            {
//                anticlockwiseNum++;
//                if(anticlockwiseNum>=2)
//                {
//                    anticlockwiseNum=0;
//                    oscilloscope.keyValue=KEYA;
//                }
//            }
//        }
//        exti_interrupt_flag_clear(EXTI_4);
//    }
//    if(RESET != exti_interrupt_flag_get(EXTI_9))
//    {
//        delay_1ms(5);
//        if(gpio_input_bit_get(GPIOB,GPIO_PIN_9)==RESET)
//        {
//            oscilloscope.keyValue=KEYD;
//        }
//        exti_interrupt_flag_clear(EXTI_9);
//    }
//    if(RESET != exti_interrupt_flag_get(EXTI_13))
//    {
//        delay_1ms(5);
//        if(gpio_input_bit_get(GPIOB,GPIO_PIN_13)==RESET)
//        {    
//            oscilloscope.keyValue=KEY1;
//        }
//        exti_interrupt_flag_clear(EXTI_13);
//    }
//    if(RESET != exti_interrupt_flag_get(EXTI_14))
//    {
//        delay_1ms(5);
//        if(gpio_input_bit_get(GPIOB,GPIO_PIN_14)==RESET)
//        {
//            oscilloscope.keyValue=KEY2;
//        }
//        exti_interrupt_flag_clear(EXTI_14);
//    }
//    if(RESET != exti_interrupt_flag_get(EXTI_15))
//    {
//        delay_1ms(5);
//        if(gpio_input_bit_get(GPIOB,GPIO_PIN_15)==RESET)
//        {
//            oscilloscope.keyValue=KEY3;
//        }
//        exti_interrupt_flag_clear(EXTI_15);
//    }
//}
