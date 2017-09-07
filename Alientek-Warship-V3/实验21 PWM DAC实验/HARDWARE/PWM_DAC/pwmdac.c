#include "pwmdac.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//��WM DAC��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/5/25
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
TIM_HandleTypeDef 	TIM1_Handler;      		//��ʱ����� 
TIM_OC_InitTypeDef 	TIM1_CH1Handler;		//��ʱ��1ͨ��1���

//TIM3 PWM���ֳ�ʼ�� 
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
void TIM1_CH1_PWM_Init(u16 arr,u16 psc)
{  
    TIM1_Handler.Instance=TIM1;          	//��ʱ��1
    TIM1_Handler.Init.Prescaler=psc;       	//��ʱ����Ƶ
    TIM1_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//���ϼ���ģʽ
    TIM1_Handler.Init.Period=arr;          	//�Զ���װ��ֵ
    TIM1_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM1_Handler);       	//��ʼ��PWM
    
    TIM1_CH1Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM2
    TIM1_CH1Handler.Pulse=arr/2;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
    TIM1_CH1Handler.OCPolarity=TIM_OCPOLARITY_HIGH; //����Ƚϼ���Ϊ��
    HAL_TIM_PWM_ConfigChannel(&TIM1_Handler,&TIM1_CH1Handler,TIM_CHANNEL_1);//����TIM1ͨ��1
	
    HAL_TIM_PWM_Start(&TIM1_Handler,TIM_CHANNEL_1);//����PWMͨ��1
	 	   
}

//��ʱ���ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_PWM_Init()����
//htim:��ʱ�����
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	
    if(htim->Instance==TIM1)
	{
		__HAL_RCC_TIM1_CLK_ENABLE();			//ʹ�ܶ�ʱ��1
		__HAL_AFIO_REMAP_TIM1_PARTIAL();		//TIM1ͨ�����Ų�����ӳ��ʹ��
		__HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
		
		GPIO_Initure.Pin=GPIO_PIN_8;           	//PA8
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;          //����
		GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
		HAL_GPIO_Init(GPIOA,&GPIO_Initure); 	
	}
}

//����TIMͨ��1��ռ�ձ�
//compare:�Ƚ�ֵ
void TIM_SetTIM1Compare1(u32 compare)
{
	TIM1->CCR1=compare; 
}

