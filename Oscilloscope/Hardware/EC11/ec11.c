#include "ec11.h"
#include "led.h"

/*
*   �������ݣ���ʼ��EC11����,DE��B��������ͨ����һ��
*   ������������
*   ����ֵ��  ��
*/
void Init_EC11_GPIO(void)
{
    //ʹ��ʱ��
    rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_CFGCMP);
    
    //�������ģʽ������
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3|GPIO_PIN_9);
    
	//�ж�ʹ��
	nvic_irq_enable(EXTI4_15_IRQn,1U);
    
    //�����ж���
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN9);
    
	//��ʼ���ж��ߣ�����Ϊ�ж�ģʽ�������ش���
	exti_init(EXTI_9,EXTI_INTERRUPT,EXTI_TRIG_RISING);
    //�ж���ʹ��
    exti_interrupt_enable(EXTI_9);
    
	//�жϱ�־λ���
	exti_interrupt_flag_clear(EXTI_9);    

}

/*
*   �������ݣ���ʼ��EC11����,��ת�ж�����A��Ҫʹ���ж�
*   ������������
*   ����ֵ��  ��
*/
void Init_EC11_EXITGPIO(void)
{
	//ʹ��ʱ��GPIOB��CMP
    rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_CFGCMP);
	
	//��������ģʽ������
	gpio_mode_set(GPIOB,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_PIN_4);
	
	//�ж�ʹ��
	nvic_irq_enable(EXTI4_15_IRQn,1U);
	
	//�����ж���
	syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN4);
	
	//��ʼ���ж��ߣ�����Ϊ�ж�ģʽ�������ش���
	//exti_init(EXTI_4,EXTI_INTERRUPT,EXTI_TRIG_RISING);
	exti_init(EXTI_4,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    
    //�ж���ʹ��
    exti_interrupt_enable(EXTI_4);
    
	//�жϱ�־λ���
	exti_interrupt_flag_clear(EXTI_4);    
}


Ec11RotateHandler ec11_rotate_handler = NULL;
Ec11ClickHandler ec11_click_handler = NULL;
Key1ClickHander key1_click_handler = NULL;
Key2ClickHander key2_click_handler = NULL;
Key3ClickHander key3_click_handler = NULL;

int EC11_clockwise = 1;
int EC11_anticlockwise = -1;
int valueWhenFalling = RESET;
int currentValue = RESET;
int currentMode = EXTI_TRIG_FALLING;
void Set_EC11_Rotate_Handler(Ec11RotateHandler rotate_handler) 
{
    ec11_rotate_handler = rotate_handler;
}

void Set_EC11_Click_Handler(Ec11ClickHandler click_handler) 
{
    ec11_click_handler = click_handler;
}


void Set_Key1_Click_Handler(Key1ClickHander click_handler) 
{
    key1_click_handler = click_handler;
}



void Set_Key2_Click_Handler(Key2ClickHander click_handler) 
{
    key2_click_handler = click_handler;
}



void Set_Key3_Click_Handler(Key3ClickHander click_handler) 
{
    key3_click_handler = click_handler;
}


/*
*   �������ݣ��ж���4-15�жϷ�����������Ϊ�˷�ֹ�󴥣�������ת�����жϣ���Ҫ��һ���������ת���β���ɹ�
              ע���жϷ������в�Ҫ������ʱ�䳤�Ĳ���
*   ������������
*   ����ֵ��  ��
*/
void EXTI4_15_IRQHandler(void)
{
    // ��ת������
    if(RESET != exti_interrupt_flag_get(EXTI_4))  
    {
        // û�лص�������
        if (ec11_rotate_handler == NULL)
        {
            exti_interrupt_flag_clear(EXTI_4);
            return;
        }
        
        
        if (currentMode == EXTI_TRIG_FALLING)
        {
            valueWhenFalling = gpio_input_bit_get(GPIOB,GPIO_PIN_3);
            exti_interrupt_flag_clear(EXTI_4);
            exti_init(EXTI_4,EXTI_INTERRUPT,EXTI_TRIG_RISING);
            currentMode = EXTI_TRIG_RISING;
        }
        else
        {
            currentValue = gpio_input_bit_get(GPIOB,GPIO_PIN_3);
            if (valueWhenFalling != currentValue)
            {
                if (currentValue == RESET)
                {
                    ec11_rotate_handler(EC11_anticlockwise);
                }
                else
                {
                    ec11_rotate_handler(EC11_clockwise);
                
                }
                exti_init(EXTI_4,EXTI_INTERRUPT, EXTI_TRIG_FALLING);
                currentMode = EXTI_TRIG_FALLING;
            
            }
        
        
        }
        exti_interrupt_flag_clear(EXTI_4);
    }
    ////////////    
    if(RESET != exti_interrupt_flag_get(EXTI_9))
    {
        if (ec11_click_handler == NULL)
        {
            exti_interrupt_flag_clear(EXTI_9);
            return;
        }
        
        if(gpio_input_bit_get(GPIOB,GPIO_PIN_9)==SET) //˳ʱ��ת����
        {
            
            delay_1ms(1);
            if(gpio_input_bit_get(GPIOB,GPIO_PIN_9)==SET) //˳ʱ��ת����
            {            
                ec11_click_handler();
            }
        }
        exti_interrupt_flag_clear(EXTI_9);
    }
    
    /////////
    if(RESET != exti_interrupt_flag_get(EXTI_13))
    {
        delay_1ms(5);
        if(gpio_input_bit_get(GPIOB,GPIO_PIN_13)==RESET)
        {    
            //oscilloscope.keyValue=KEY1;
            if (key1_click_handler !=NULL)
            {
                key1_click_handler();
            }
        }
        exti_interrupt_flag_clear(EXTI_13);
    }
    ///////
    if(RESET != exti_interrupt_flag_get(EXTI_14))
    {
        delay_1ms(5);
        if(gpio_input_bit_get(GPIOB,GPIO_PIN_14)==RESET)
        {
            //oscilloscope.keyValue=KEY2;
            if (key2_click_handler !=NULL)
            {
                key2_click_handler();
            }
        }
        exti_interrupt_flag_clear(EXTI_14);
    }
    /////////
    if(RESET != exti_interrupt_flag_get(EXTI_15))
    {
        delay_1ms(5);
        if(gpio_input_bit_get(GPIOB,GPIO_PIN_15)==RESET)
        {
            //oscilloscope.keyValue=KEY3;
            if (key3_click_handler !=NULL)
            {
                key3_click_handler();
            }
        }
        exti_interrupt_flag_clear(EXTI_15);
    }
}