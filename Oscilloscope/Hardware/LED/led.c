#include "led.h"

/*
*   �������ݣ���ʼ��LED����
*   ������������
*   ����ֵ��  ��
*/
void Init_LED_GPIO(void)
{
    //ʹ��ʱ��
    rcu_periph_clock_enable(RCU_GPIOC);
    //�������ģʽ����������
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_14);
    //�������ģʽ����������
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    //����������ͣ����������50Mhz
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    //����������ͣ����������50Mhz
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    //Ĭ�ϲ�����
    CLose_LED(led2);
    CLose_LED(led3);
}

/*
*   �������ݣ��򿪶�ӦLED��
*   ������������
*   ����ֵ��  ��
*/
void Open_LED(uint8_t value)
{
    switch(value)
    {
        case 1:
            gpio_bit_reset(GPIOC,GPIO_PIN_14);
            break;
        case 2:
            gpio_bit_reset(GPIOC,GPIO_PIN_15);
            break;
        default:
            break;
    }
}

/*
*   �������ݣ��رն�ӦLED��
*   ������������
*   ����ֵ��  ��
*/
void CLose_LED(uint8_t value)
{
    switch(value)
    {
        case 1:
            gpio_bit_set(GPIOC,GPIO_PIN_14);
            break;
        case 2:
            gpio_bit_set(GPIOC,GPIO_PIN_15);
            break;
        default:
            break;
    }
}