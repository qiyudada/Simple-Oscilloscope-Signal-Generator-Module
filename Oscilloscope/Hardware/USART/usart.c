#include "usart.h"
#include "string.h"

#define USART0_RDATA_ADDRESS      ((uint32_t)&USART_RDATA(USART0))

//������ɱ�־λ
static __IO uint8_t receive_flag = 0;
//������������
static uint8_t rx_count = 0;
//�������ݻ�����
static uint8_t rxbuffer[256];

/*
*   �������ݣ���ʼ����ӡ����
*   ����������bound--������
*   ����ֵ��  ��
*/
void Init_USART(uint32_t bound)
{
    //ʹ��ʱ��
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART0);
    
    //���ù�������
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);
    
    //�˿�ģʽ����
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);
    
    //�˿�ģʽ����
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);
    
    //��λUSART0
    usart_deinit(USART0);
    usart_word_length_set(USART0, USART_WL_8BIT);       //8λ����λ��
    usart_stop_bit_set(USART0, USART_STB_1BIT);         //1λֹͣλ
    usart_parity_config(USART0, USART_PM_NONE);         //��У��λ
    usart_baudrate_set(USART0, bound);                  //������
    usart_receive_config(USART0, USART_RECEIVE_ENABLE); //����ʹ��
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);//����ʹ��
    usart_dma_receive_config(USART0, USART_DENR_ENABLE);//DAM����ʹ��
    //ʹ�ܴ���0
    usart_enable(USART0);
    
    //�ȴ����ڿ���
    while(RESET == usart_flag_get(USART0, USART_FLAG_IDLE));
    //������б�־λ
    usart_flag_clear(USART0, USART_FLAG_IDLE);
    //�����ж�ʹ��
    usart_interrupt_enable(USART0,USART_INT_IDLE);
}

/*
*   �������ݣ���ʼ������0����DMA
*   ������������
*   ����ֵ��  ��
*/
void Init_USART0RecDMA(void)
{
    //DMA�����ṹ��
    dma_parameter_struct dma_init_struct;
    
    //�ж�ʹ��
    nvic_irq_enable(USART0_IRQn, 0); 
    
    //DMAʱ��ʹ��
    rcu_periph_clock_enable(RCU_DMA);
    
    //��λDMAͨ����DMAͨ��2��Ӧ����0�Ľ���
    dma_deinit(DMA_CH2);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;       //�����赽�ڴ�
    dma_init_struct.memory_addr = (uint32_t)rxbuffer;           //�ڴ��ַ
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;    //�ڴ��ַ����
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;       //�ڴ��ַ���-8bit
    dma_init_struct.number = 256;                               //DMA���������
    dma_init_struct.periph_addr = USART0_RDATA_ADDRESS;         //�����ַ
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;   //�����ַ������
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;   //�����ַ���
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;         //�ж����ȼ���
    dma_init(DMA_CH2, &dma_init_struct);                        //��ʼ��DMAͨ��2
    
    //DAMѭ��ģʽʧ��
    dma_circulation_disable(DMA_CH2);
    
    //ʹ��DMAͨ��2
    dma_channel_enable(DMA_CH2);
}

/*
*   �������ݣ�USART0���ͺ���
*   ����������char *SendData-��������ָ��
*   ����ֵ��  ��
*/
void USART0_SendData(char *SendData)
{
    usart_flag_clear(USART0,USART_FLAG_TC);
    while(*SendData != '\0')
    {
       usart_data_transmit(USART0, (uint8_t)*SendData);
       while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
       usart_flag_clear(USART0,USART_FLAG_TC);
       SendData++;
    }
}


/*
*   �������ݣ�fputc�˿��ض���
*   ������������
*   ����ֵ��  ��
*/
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t) ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}

/*
*   �������ݣ�����0�жϷ�����
*   ��������: ��
*   ����ֵ��  ��
*/
void USART0_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)){
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_IDLE);
        
        //������������
        rx_count = 256 - (dma_transfer_number_get(DMA_CH2));
        //������ɱ�־λ
        receive_flag = 1;
        
        //ʧ��DMAͨ��
        dma_channel_disable(DMA_CH2);
        //��������DMA����
        dma_transfer_number_config(DMA_CH2, 256);
        //ʹ��DMAͨ��
        dma_channel_enable(DMA_CH2);
    }
}

/*
*   �������ݣ�����0�������ݴ�����
*   ��������: ��
*   ����ֵ��  ��
*/
void USART0_RecHandle(void)
{
    //������ݽ������
    if(receive_flag == 1)
    {
        printf("%s\r\n",rxbuffer);
        memset(rxbuffer,0,256);
        receive_flag=0;
        rx_count=0;
    }
  
}