#include "ec11.h"
#include "led.h"

/*
*   函数内容：初始化EC11引脚,DE与B引脚与普通引脚一样
*   函数参数：无
*   返回值：  无
*/
void Init_EC11_GPIO(void)
{
    //使能时钟
    rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_CFGCMP);
    
    //设置输出模式，上拉
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3|GPIO_PIN_9);
    
	//中断使能
	nvic_irq_enable(EXTI4_15_IRQn,1U);
    
    //配置中断线
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN9);
    
	//初始化中断线，设置为中断模式，上升沿触发
	exti_init(EXTI_9,EXTI_INTERRUPT,EXTI_TRIG_RISING);
    //中断线使能
    exti_interrupt_enable(EXTI_9);
    
	//中断标志位清除
	exti_interrupt_flag_clear(EXTI_9);    

}

/*
*   函数内容：初始化EC11引脚,旋转判断引脚A需要使用中断
*   函数参数：无
*   返回值：  无
*/
void Init_EC11_EXITGPIO(void)
{
	//使能时钟GPIOB，CMP
    rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_CFGCMP);
	
	//设置引脚模式，上拉
	gpio_mode_set(GPIOB,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_PIN_4);
	
	//中断使能
	nvic_irq_enable(EXTI4_15_IRQn,1U);
	
	//配置中断线
	syscfg_exti_line_config(EXTI_SOURCE_GPIOB,EXTI_SOURCE_PIN4);
	
	//初始化中断线，设置为中断模式，上升沿触发
	//exti_init(EXTI_4,EXTI_INTERRUPT,EXTI_TRIG_RISING);
	exti_init(EXTI_4,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    
    //中断线使能
    exti_interrupt_enable(EXTI_4);
    
	//中断标志位清除
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
*   函数内容：中断线4-15中断服务函数，这里为了防止误触，做了旋转计数判断，需要沿一个方向多旋转几次才算成功
              注意中断服务函数中不要做操作时间长的操作
*   函数参数：无
*   返回值：  无
*/
void EXTI4_15_IRQHandler(void)
{
    // 旋转编码器
    if(RESET != exti_interrupt_flag_get(EXTI_4))  
    {
        // 没有回调不处理
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
        
        if(gpio_input_bit_get(GPIOB,GPIO_PIN_9)==SET) //顺时针转两格
        {
            
            delay_1ms(1);
            if(gpio_input_bit_get(GPIOB,GPIO_PIN_9)==SET) //顺时针转两格
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