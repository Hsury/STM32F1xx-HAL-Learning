#include "rs485.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//RS485��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/5/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

UART_HandleTypeDef USART2_RS485Handler;  //USART2���(����RS485)

#if EN_USART2_RX   		//���ʹ���˽���   	  
//���ջ����� 	
u8 RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
u8 RS485_RX_CNT=0;  

void USART2_IRQHandler(void)
{
    u8 res;	  
    if((__HAL_UART_GET_FLAG(&USART2_RS485Handler,UART_FLAG_RXNE)!=RESET))  //�����ж�
	{	 	
		HAL_UART_Receive(&USART2_RS485Handler,&res,1,1000);
		if(RS485_RX_CNT<64)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;		//��¼���յ���ֵ
			RS485_RX_CNT++;						//������������1 
		} 
	} 
}    
#endif

//��ʼ��IO ����2
//bound:������
void RS485_Init(u32 bound)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
	__HAL_RCC_GPIOD_CLK_ENABLE();			//ʹ��GPIODʱ��
	__HAL_RCC_USART2_CLK_ENABLE();			//ʹ��USART2ʱ��
	
	GPIO_Initure.Pin=GPIO_PIN_2; 			//PA2
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;		//����
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA2
	
	GPIO_Initure.Pin=GPIO_PIN_3; 			//PA3
	GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//��������
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA3
	
	//PD7���������485ģʽ����  
    GPIO_Initure.Pin=GPIO_PIN_7; 			//PD7
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
    
    //USART ��ʼ������
	USART2_RS485Handler.Instance=USART2;			        //USART2
	USART2_RS485Handler.Init.BaudRate=bound;		        //������
	USART2_RS485Handler.Init.WordLength=UART_WORDLENGTH_8B;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART2_RS485Handler.Init.StopBits=UART_STOPBITS_1;		//һ��ֹͣλ
	USART2_RS485Handler.Init.Parity=UART_PARITY_NONE;		//����żУ��λ
	USART2_RS485Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//��Ӳ������
	USART2_RS485Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&USART2_RS485Handler);			        //HAL_UART_Init()��ʹ��USART2
    
  __HAL_UART_DISABLE_IT(&USART2_RS485Handler,UART_IT_TC);
#if EN_USART2_RX
	__HAL_UART_ENABLE_IT(&USART2_RS485Handler,UART_IT_RXNE);//���������ж�
	HAL_NVIC_EnableIRQ(USART2_IRQn);				        //ʹ��USART1�ж�
	HAL_NVIC_SetPriority(USART2_IRQn,3,3);			        //��ռ���ȼ�3�������ȼ�3
#endif	
	RS485_TX_EN=0;											//Ĭ��Ϊ����ģʽ		
}

//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(u8 *buf,u8 len)
{
	RS485_TX_EN=1;			//����Ϊ����ģʽ
	HAL_UART_Transmit(&USART2_RS485Handler,buf,len,1000);//����2��������
	RS485_RX_CNT=0;	  
	RS485_TX_EN=0;				//����Ϊ����ģʽ	
}
//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==RS485_RX_CNT&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//��¼�������ݳ���
		RS485_RX_CNT=0;		//����
	}
} 
