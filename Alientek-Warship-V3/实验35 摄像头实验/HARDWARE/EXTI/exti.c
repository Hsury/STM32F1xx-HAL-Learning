#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "ov7670.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//�ⲿ�ж���������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/5/25
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//�ⲿ�жϳ�ʼ��
void EXTI8_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();               //����GPIOAʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_8;                //PA8
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //�����ش���
    GPIO_Initure.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    //�ж���8-PA8
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,0,0);       //��ռ���ȼ�Ϊ2�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);             //ʹ���ж���0
}

//�жϷ�����
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);		//�����жϴ����ú���
}

u8 ov_sta;	//֡�жϱ��
//�жϷ����������Ҫ��������
//��HAL�������е��ⲿ�жϷ�����������ô˺���
//GPIO_Pin:�ж����ź�
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin==GPIO_PIN_8)
	{
		OV7670_WRST=0;	//��λдָ��		  		 
		OV7670_WRST=1;	
		OV7670_WREN=1;	//����д��FIFO 	 
		ov_sta++;		//֡�жϼ�1 
	}
}
