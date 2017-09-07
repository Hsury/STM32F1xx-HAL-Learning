#include "remote.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//����ң�ؽ�����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/5/31
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

TIM_HandleTypeDef TIM4_Handler;      //��ʱ��4���

//����ң�س�ʼ��
//����IO�Լ�TIM4_CH4�����벶��
void Remote_Init(void)
{  
    TIM_IC_InitTypeDef TIM4_CH1Config;  
    
    TIM4_Handler.Instance=TIM4;                          //ͨ�ö�ʱ��4
    TIM4_Handler.Init.Prescaler=(72-1);                	 //Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.
    TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM4_Handler.Init.Period=10000;                      //�Զ�װ��ֵ
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM4_Handler);
    
    //��ʼ��TIM1���벶�����
    TIM4_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //�����ز���
    TIM4_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI4��
    TIM4_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM4_CH1Config.ICFilter=0x03;                       //IC4F=0003 8����ʱ��ʱ�������˲�
    HAL_TIM_IC_ConfigChannel(&TIM4_Handler,&TIM4_CH1Config,TIM_CHANNEL_4);//����TIM4ͨ��4
    HAL_TIM_IC_Start_IT(&TIM4_Handler,TIM_CHANNEL_4);   //��ʼ����TIM4��ͨ��4
    __HAL_TIM_ENABLE_IT(&TIM4_Handler,TIM_IT_UPDATE);   //ʹ�ܸ����ж�
}

//��ʱ��1�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_IC_Init()����
//htim:��ʱ��1���
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM4_CLK_ENABLE();            //ʹ��TIM4ʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_9;            //PB9
    GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;  	//��������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM4_IRQn,1,3); 	//�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
    HAL_NVIC_EnableIRQ(TIM4_IRQn);       	//����ITM4�ж�
}

//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������								   
//[3:0]:�����ʱ��
u8 	RmtSta=0;	  	  
u16 Dval;		//�½���ʱ��������ֵ
u32 RmtRec=0;	//������յ�������	   		    
u8  RmtCnt=0;	//�������µĴ���	 

//��ʱ��4�ж˷�����
void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM4_Handler);//��ʱ�����ô�����
} 

//��ʱ�����£�������жϻص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM4)
	{
 		if(RmtSta&0x80)//�ϴ������ݱ����յ���
		{	
			RmtSta&=~0X10;						//ȡ���������Ѿ���������
			if((RmtSta&0X0F)==0X00)RmtSta|=1<<6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);//���������ʶ
				RmtSta&=0XF0;	//��ռ�����	
			}						 	   	
		}	
	}
}

//��ʱ�����벶���жϻص�����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//�����жϷ���ʱִ��
{
	if(htim->Instance==TIM4)
	{
		if(RDATA)//�����ز���
		{
			TIM_RESET_CAPTUREPOLARITY(&TIM4_Handler,TIM_CHANNEL_4);   //һ��Ҫ�����ԭ�������ã���
			TIM_SET_CAPTUREPOLARITY(&TIM4_Handler,TIM_CHANNEL_4,TIM_ICPOLARITY_FALLING);//CC4P=1 ����Ϊ�½��ز���
			__HAL_TIM_SET_COUNTER(&TIM4_Handler,0);  	//��ն�ʱ��ֵ   	  
		  	RmtSta|=0X10;							//����������Ѿ�������
		}else //�½��ز���
		{
			Dval=HAL_TIM_ReadCapturedValue(&TIM4_Handler,TIM_CHANNEL_4);//��ȡCCR4Ҳ������CC4IF��־λ
			TIM_RESET_CAPTUREPOLARITY(&TIM4_Handler,TIM_CHANNEL_4);   	//һ��Ҫ�����ԭ�������ã���
			TIM_SET_CAPTUREPOLARITY(&TIM4_Handler,TIM_CHANNEL_4,TIM_ICPOLARITY_RISING);//����TIM4ͨ��4�����ز���
			if(RmtSta&0X10)							//���һ�θߵ�ƽ���� 
			{
 				if(RmtSta&0X80)//���յ���������
				{
					
					if(Dval>300&&Dval<800)			//560Ϊ��׼ֵ,560us
					{
						RmtRec<<=1;					//����һλ.
						RmtRec|=0;					//���յ�0	   
					}else if(Dval>1400&&Dval<1800)	//1680Ϊ��׼ֵ,1680us
					{
						RmtRec<<=1;					//����һλ.
						RmtRec|=1;					//���յ�1
					}else if(Dval>2200&&Dval<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
					{
						RmtCnt++; 					//������������1��
						RmtSta&=0XF0;				//��ռ�ʱ��		
					}
 				}else if(Dval>4200&&Dval<4700)		//4500Ϊ��׼ֵ4.5ms
				{
					RmtSta|=1<<7;					//��ǳɹ����յ���������
					RmtCnt=0;						//�����������������
				}						 
			}
			RmtSta&=~(1<<4);
		}
	}
}

//����������
//����ֵ:
//	 0,û���κΰ�������
//����,���µİ�����ֵ.
u8 Remote_Scan(void)
{        
	u8 sta=0;       
	u8 t1,t2;  
	if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
	{ 
	    t1=RmtRec>>24;			//�õ���ַ��
	    t2=(RmtRec>>16)&0xff;	//�õ���ַ���� 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//��ֵ��ȷ	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//�������ݴ���/ң���Ѿ�û�а�����
		{
		 	RmtSta&=~(1<<6);//������յ���Ч������ʶ
			RmtCnt=0;		//�����������������
		}
	}  
    return sta;
}
