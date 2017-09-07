#include "wkup.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//�������� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/5/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//ϵͳ�������ģʽ
void Sys_Enter_Standby(void)
{
    __HAL_RCC_APB2_FORCE_RESET();       //��λ����IO�� 
   	__HAL_RCC_PWR_CLK_ENABLE();         //ʹ��PWRʱ��
			  	
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  //���Wake_UP��־
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);           //����WKUP���ڻ���
    HAL_PWR_EnterSTANDBYMode();                         //�������ģʽ     
}
 
     
//���WKUP�ŵ��ź�
//����ֵ1:��������3s����
//      0:����Ĵ���	
u8 Check_WKUP(void) 
{
	u8 t=0;	//��¼���µ�ʱ��
	LED0=0; //����DS0 
	while(1)
	{
		if(WKUP_KD)
		{
			t++;			//�Ѿ������� 
			delay_ms(30);
			if(t>=100)		//���³���3����
			{
				LED0=0;	 	//����DS0 
				return 1; 	//����3s������
			}
		}else 
		{ 
			LED0=1;
			return 0; //���²���3��
		}
	}
}  

//�ⲿ�ж���0�жϷ�����
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

//�ж���0�жϴ������
//�˺����ᱻHAL_GPIO_EXTI_IRQHandler()����
//GPIO_Pin:����
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==GPIO_PIN_0)//PA0
    {
        if(Check_WKUP())//�ػ�
        {
            Sys_Enter_Standby();//�������ģʽ
        }
    }    
}

//PA0 WKUP���ѳ�ʼ��
void WKUP_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;  //�ж�,������
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    //����Ƿ�����������
    if(Check_WKUP()==0)
    {
        Sys_Enter_Standby();//���ǿ������������ģʽ
    }

    HAL_NVIC_SetPriority(EXTI0_IRQn,0x02,0x02);//��ռ���ȼ�2�������ȼ�2
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
