#include "key.h"
#include "timer.h"
#include "tft.h"

static uint8_t keyValue=0;

/*
*   �������ݣ���ʼ������GPIO
*   ������������
*   ����ֵ��  ��
*/
void Init_Key_GPIO(void)
{
	//ʹ��ʱ��
    rcu_periph_clock_enable(RCU_GPIOB);
	
	//�������ģʽ������
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3|GPIO_PIN_9|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

    //�ж���ʹ��
    nvic_irq_enable(EXTI4_15_IRQn,3U);
    
    //�����ж���
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN13);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN14);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN15);
    
    //��ʼ���ж��ߣ�����Ϊ�ж�ģʽ�������ش���
    exti_init(EXTI_13,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    exti_init(EXTI_14,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    exti_init(EXTI_15,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    
    //�жϱ�־λ���
    exti_interrupt_flag_clear(EXTI_13);
    exti_interrupt_flag_clear(EXTI_14);
    exti_interrupt_flag_clear(EXTI_15);
}

/*
*   �������ݣ���ʼ��EC11 GPIO
*   ������������
*   ����ֵ��  ��
*/
//void Init_EC11_GPIO(void)
//{
//	//ʹ��ʱ��GPIOB��CMP
//    rcu_periph_clock_enable(RCU_GPIOB);
//	rcu_periph_clock_enable(RCU_CFGCMP);
//	
//	//��������ģʽ������
//	gpio_mode_set(GPIOB,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_PIN_4);
//	
//	//�ж���ʹ��
//	nvic_irq_enable(EXTI4_15_IRQn,3U);
//	
//	//�����ж���
//	syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN4);
//	
//	//��ʼ���ж��ߣ�����Ϊ�ж�ģʽ�������ش���
//	exti_init(EXTI_4,EXTI_INTERRUPT,EXTI_TRIG_RISING);
//	
//	//�жϱ�־λ���
//	exti_interrupt_flag_clear(EXTI_4);

//    //�ж���ʹ��
//    exti_interrupt_enable(EXTI_4);
//    
//	//�жϱ�־λ���
//	exti_interrupt_flag_clear(EXTI_4);    
//}



//extern volatile struct Oscilloscope oscilloscope;

//void EXTI4_15_IRQHandler(void)
//{
//    static uint8_t clockwiseNum=0;
//    static uint8_t anticlockwiseNum=0;
//    if(RESET != exti_interrupt_flag_get(EXTI_4)) 
//    {
//        if(gpio_input_bit_get(GPIOB,GPIO_PIN_3)==RESET) //˳ʱ��ת����
//        {
//            delay_1ms(5);
//            anticlockwiseNum=0;
//            if(gpio_input_bit_get(GPIOB,GPIO_PIN_3)==RESET) //˳ʱ��ת����
//            {
//                clockwiseNum++;
//                if(clockwiseNum>=2)
//                {
//                    clockwiseNum=0;
//                    oscilloscope.keyValue=KEYB; 
//                }            
//            }
//        }
//        else                                            //��ʱ��ת����
//        {
//            delay_1ms(5);
//            clockwiseNum=0;
//            if(gpio_input_bit_get(GPIOB,GPIO_PIN_3)==SET) //��ʱ��ת����
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
