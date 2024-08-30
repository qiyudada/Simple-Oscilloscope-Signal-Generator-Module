#include "adc.h"
//#include "oscilloscope.h"

#define ADC_VALUE_NUM 300U

uint16_t adc_value[ADC_VALUE_NUM];

/*
*   �������ݣ��õ�ADCֵ
*   ����������value--�����±�
*   ����ֵ��  ��
*/
uint16_t Get_ADC_Value(uint16_t value)
{
    uint16_t returnValue=0;
    if(value>ADC_VALUE_NUM)
    {
        value=0;
    }
    returnValue=adc_value[value];
    adc_value[value]=0;
    return returnValue;
}
/*
*   �������ݣ���ʼ��ADC
*   ������������
*   ����ֵ��    ��
*/
void Init_ADC(void)
{
    //ʹ������
    rcu_periph_clock_enable(RCU_GPIOA);
    
    //ʹ��ADCʱ��
    rcu_periph_clock_enable(RCU_ADC);
    
    //ʹ��ʱ�����ã����28M
    rcu_adc_clock_config(RCU_ADCCK_AHB_DIV9);
    
    //�������ã�PA3��ģ�����룬��������
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_3);
    
    //ADC��������ʹ��
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE); 

    //ADCɨ�蹦��ʧ�ܣ������һ��ͨ��
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);

    //ADCע�����Զ�ת��ģʽʧ�ܣ���������ע����
    adc_special_function_config(ADC_INSERTED_CHANNEL_AUTO, DISABLE);    
    
    //ADC����У׼����
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    
    //ADCͨ����������
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);   

    //ADC����ͨ������--PA3��˳����0��ͨ��3������ʱ��55.5��ʱ������
    adc_regular_channel_config(0, ADC_CHANNEL_3, ADC_SAMPLETIME_239POINT5);    
    
    //ADC���������ã��������
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE); 
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
    
    //ʹ��ADC
    adc_enable();
    delay_1ms(1U);
    
    //ʹ��У׼�͸�λ
    adc_calibration_enable();
    
    //DMAģʽʹ��
    adc_dma_mode_enable();
    
    //ADC�������ʹ��
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}

void ADC_DMA_Init(void)
{
    //DMAʱ��ʹ��
    rcu_periph_clock_enable(RCU_DMA);
    
    //DMA�����ṹ��
    dma_parameter_struct dma_data_parameter;
    
    //ʹ��DMA�ж�
    nvic_irq_enable(DMA_Channel0_IRQn, 0U);
    
    //ͨ��0��λ
    dma_deinit(DMA_CH0);
    
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA);       //�������ַ
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;  //�����ַ������
    dma_data_parameter.memory_addr  = (uint32_t)(&adc_value);       //�ڴ��ַ
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;   //�ڴ��ַ����
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;   //����λ��
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;       //�ڴ�λ��
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;     //���赽�ڴ�
    dma_data_parameter.number       = ADC_VALUE_NUM;                //����
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;            //�����ȼ�
    
    dma_init(DMA_CH0, &dma_data_parameter);                         //DMAͨ��0��ʼ��
    
    dma_circulation_enable(DMA_CH0);                               //DMAѭ��ģʽʹ��
    
    dma_channel_enable(DMA_CH0);                                    //DMAͨ��0ʹ��
    
    //ʹ��DMA��������ж�
    dma_interrupt_enable(DMA_CH0, DMA_CHXCTL_FTFIE);
}

DMAIntHandler dmaHandler = NULL;
void Set_DMA_Int_Handler(DMAIntHandler handler)
{
    dmaHandler = handler;
}
void DMA_Channel0_IRQHandler(void)
{
    if(dma_interrupt_flag_get(DMA_CH0, DMA_INT_FLAG_FTF)){
        oscilloscope.showbit=1;
        if (dmaHandler != NULL) {
            dmaHandler();
        }
        dma_channel_disable(DMA_CH0);
        //����жϱ�־λ
        dma_interrupt_flag_clear(DMA_CH0, DMA_INT_FLAG_G);
    }
}


////////////////////////

/*
*   �������ݣ���ʼ������Ƶ�׵�2K ADC
*   ������������
*   ����ֵ��    ��
*/
void Init_Spectro_ADC(void)
{
    //ʹ������
    rcu_periph_clock_enable(RCU_GPIOA);
    
    //ʹ��ADCʱ��
    rcu_periph_clock_enable(RCU_ADC);
    
    //ʹ��ʱ�����ã����28M;  72 / 9 = 8MHz
    rcu_adc_clock_config(RCU_ADCCK_AHB_DIV9);
    
    //�������ã�PA3��ģ�����룬��������
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_3);
    
    //ADC��������ʧ�ܣ����δ���
    adc_special_function_config(ADC_CONTINUOUS_MODE, DISABLE); 

    //ADCɨ�蹦��ʧ�ܣ������һ��ͨ��
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);

    //ADCע�����Զ�ת��ģʽʧ�ܣ���������ע����
    adc_special_function_config(ADC_INSERTED_CHANNEL_AUTO, DISABLE);    
    
    //ADC����У׼����
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    
    //ADCͨ����������
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);   

    //ADC����ͨ������--PA3��˳����0��ͨ��3������ʱ��55.5��ʱ������
    adc_regular_channel_config(0, ADC_CHANNEL_3, ADC_SAMPLETIME_239POINT5);    
    
    //ADC���������ã��������, ͨ�� timer0 channel0 ����
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_T0_CH0); 
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
    
    //ʹ��ADC
    adc_enable();
    delay_1ms(1U);
    
    //ʹ��У׼�͸�λ
    adc_calibration_enable();
    
    //DMAģʽʹ��
    adc_dma_mode_enable();
    
    //ADC�������ʹ��
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}

void Init_Spectro_ADC_DMA(void)
{
    //DMAʱ��ʹ��
    rcu_periph_clock_enable(RCU_DMA);
    
    //DMA�����ṹ��
    dma_parameter_struct dma_data_parameter;
    
    //ʹ��DMA�ж�
    nvic_irq_enable(DMA_Channel0_IRQn, 0U);
    
    //ͨ��0��λ
    dma_deinit(DMA_CH0);
    
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA);       //�������ַ
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;  //�����ַ������
    dma_data_parameter.memory_addr  = (uint32_t)(&adc_value);       //�ڴ��ַ
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;   //�ڴ��ַ����
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;   //����λ��
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;       //�ڴ�λ��
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;     //���赽�ڴ�
    dma_data_parameter.number       = 64; //FFT_N;                  //����
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;            //�����ȼ�
    
    dma_init(DMA_CH0, &dma_data_parameter);                         //DMAͨ��0��ʼ��
    
    dma_circulation_enable(DMA_CH0);                               //DMAѭ��ģʽʹ��
    
    dma_channel_enable(DMA_CH0);                                    //DMAͨ��0ʹ��
    
    //ʹ��DMA��������ж�
    dma_interrupt_enable(DMA_CH0, DMA_CHXCTL_FTFIE);
}

void Init_Spector_ADC_Timer()
{
	//ʹ�ܶ�ʱ��0
	rcu_periph_clock_enable(RCU_TIMER0);
    
    //��ʱ����������ṹ��
	timer_oc_parameter_struct timer_ocinitpara;
	
	//��ʱ����ʼ�������ṹ��
    timer_parameter_struct timer_initpara;
	
	//��λ��ʱ��0
	timer_deinit(TIMER0);
	
	//��ʼ����ʱ���ṹ�����
	timer_struct_para_init(&timer_initpara);
	
	timer_initpara.prescaler         = 719;									//Ԥ��Ƶ������ 72 / 720 = 0.1Mhz = 100,000Hz
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;	                //���ض���
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;		            //���ϼ���
	timer_initpara.period            = 49;								    //����    100,000Hz / 50 = 2,000Hz = 2kHz
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;		            //ʱ�ӷ�Ƶ
	timer_initpara.repetitioncounter = 0;								    //��װ��ֵ
	timer_init(TIMER0, &timer_initpara);
	
	//��ʼ����ʱ��ͨ����������ṹ��
	timer_channel_output_struct_para_init(&timer_ocinitpara);
	
	timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;				//���״̬�������ͨ������
	timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;			    //�������״̬�ر�
	timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_LOW;	        //�������Ϊ��
	timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;        //�����������Ϊ��
	timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_HIGH;        //����״̬ͨ�����
	timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_HIGH;       //����״̬����ͨ�����
	
	timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);
	
	//����Ƚ�ֵ
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 25);
	
	//���ģʽ0������ʱ��С�ڱȽ�ֵʱ�������Ч��ƽ��Ϊ�ߣ����ڱȽ���ֵʱ���Ϊ��
	timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM1);
	
	//Ӱ��ģʽ����ر�
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
	
	//ʹ���Զ���װ��
	timer_auto_reload_shadow_enable(TIMER0);
	
	//���ö�ʱ��Ϊ��Ҫ�������������ͨ��ʹ��
	timer_primary_output_config(TIMER0, ENABLE);
	
	//ʹ�ܶ�ʱ��
	timer_enable(TIMER0);
}
