#include "tpad.h"
#include "delay.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//TPAD��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/5/26
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
TIM_HandleTypeDef TIM5_Handler;         //��ʱ��5��� 

#define TPAD_ARR_MAX_VAL  0XFFFF		//����ARRֵ(TIM5��16λ��ʱ��)	  
vu16 tpad_default_val=0;				//���ص�ʱ��(û���ְ���),��������Ҫ��ʱ��

//��ʼ����������
//��ÿ��ص�ʱ����������ȡֵ.
//psc:��Ƶϵ��,ԽС,������Խ��.
//����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ��
u8 TPAD_Init(u8 psc)
{
	u16 buf[10];
	u16 temp;
	u8 j,i;
	TIM5_CH2_Cap_Init(TPAD_ARR_MAX_VAL,psc-1);//���÷�Ƶϵ��
	for(i=0;i<10;i++)//������ȡ10��
	{				 
		buf[i]=TPAD_Get_Val();
		delay_ms(10);	    
	}				    
	for(i=0;i<9;i++)//����
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	temp=0;
	for(i=2;i<8;i++)temp+=buf[i];//ȡ�м��8�����ݽ���ƽ��
	tpad_default_val=temp/6;
	printf("tpad_default_val:%d\r\n",tpad_default_val);	
	if(tpad_default_val>(vu16)TPAD_ARR_MAX_VAL/2)return 1;//��ʼ����������TPAD_ARR_MAX_VAL/2����ֵ,������!
	return 0;		     	    					   
}
//��λһ��
//�ͷŵ��ݵ������������ʱ���ļ���ֵ
void TPAD_Reset(void)
{
    GPIO_InitTypeDef GPIO_Initure;
	
    GPIO_Initure.Pin=GPIO_PIN_1;            //PA1
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);	//PA1���0���ŵ�
    delay_ms(5);
    __HAL_TIM_CLEAR_FLAG(&TIM5_Handler,TIM_FLAG_CC2|TIM_FLAG_UPDATE);   //�����־λ
    __HAL_TIM_SET_COUNTER(&TIM5_Handler,0); //������ֵ��0
    
    GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;  	//���츴������
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);         
}

//�õ���ʱ������ֵ
//�����ʱ,��ֱ�ӷ��ض�ʱ���ļ���ֵ.
//����ֵ������ֵ/����ֵ����ʱ������·��أ�
u16 TPAD_Get_Val(void)
{
    TPAD_Reset();
    while(__HAL_TIM_GET_FLAG(&TIM5_Handler,TIM_FLAG_CC2)==RESET) //�ȴ�����������
    {
        if(__HAL_TIM_GET_COUNTER(&TIM5_Handler)>TPAD_ARR_MAX_VAL-500) return __HAL_TIM_GET_COUNTER(&TIM5_Handler);//��ʱ�ˣ�ֱ�ӷ���CNT��ֵ
    };
    return HAL_TIM_ReadCapturedValue(&TIM5_Handler,TIM_CHANNEL_2);
}

//��ȡn��,ȡ���ֵ
//n��������ȡ�Ĵ���
//����ֵ��n�ζ������������������ֵ
u16 TPAD_Get_MaxVal(u8 n)
{ 
	u16 temp=0; 
	u16 res=0; 
	u8 lcntnum=n*2/3;//����2/3*n����Ч������,������Ч
	u8 okcnt=0;
	while(n--)
	{
		temp=TPAD_Get_Val();//�õ�һ��ֵ
		if(temp>(tpad_default_val*5/4))okcnt++;//���ٴ���Ĭ��ֵ��5/4������Ч
		if(temp>res)res=temp;
	}
	if(okcnt>=lcntnum)return res;//����2/3�ĸ���,Ҫ����Ĭ��ֵ��5/4������Ч
	else return 0;
}  

//ɨ�败������
//mode:0,��֧����������(����һ�α����ɿ����ܰ���һ��);1,֧����������(����һֱ����)
//����ֵ:0,û�а���;1,�а���;										  
u8 TPAD_Scan(u8 mode)
{
	static u8 keyen=0;	//0,���Կ�ʼ���;>0,�����ܿ�ʼ���	 
	u8 res=0;
	u8 sample=3;		//Ĭ�ϲ�������Ϊ3��	 
	u16 rval;
	if(mode)
	{
		sample=6;		//֧��������ʱ�����ò�������Ϊ6��
		keyen=0;		//֧������	  
	}
	rval=TPAD_Get_MaxVal(sample); 
	if(rval>(tpad_default_val*4/3)&&rval<(10*tpad_default_val))//����tpad_default_val+(1/3)*tpad_default_val,��С��10��tpad_default_val,����Ч
	{							 
		if(keyen==0)res=1;	//keyen==0,��Ч 
		//printf("r:%d\r\n",rval);		     	    					   
		keyen=3;				//����Ҫ�ٹ�3��֮����ܰ�����Ч   
	} 
	if(keyen)keyen--;		   							   		     	    					   
	return res;
}	

//��ʱ��5ͨ��2���벶������
//arr���Զ���װֵ(TIM2��16λ��!!)
//psc��ʱ��Ԥ��Ƶ��
void TIM5_CH2_Cap_Init(u32 arr,u16 psc)
{  
    TIM_IC_InitTypeDef TIM5_CH2Config;  
    
    TIM5_Handler.Instance=TIM5;                          //ͨ�ö�ʱ��5
    TIM5_Handler.Init.Prescaler=psc;                     //��Ƶ
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM5_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM5_Handler);
    
    TIM5_CH2Config.ICPolarity=TIM_ICPOLARITY_RISING;    //�����ز���
    TIM5_CH2Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI1��
    TIM5_CH2Config.ICPrescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM5_CH2Config.ICFilter=0;                          //���������˲��������˲�
    HAL_TIM_IC_ConfigChannel(&TIM5_Handler,&TIM5_CH2Config,TIM_CHANNEL_2);//����TIM5ͨ��2
    HAL_TIM_IC_Start(&TIM5_Handler,TIM_CHANNEL_2);      //��ʼ����TIM5��ͨ��2
}

//��ʱ��5�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_IC_Init()����
//htim:��ʱ��5���
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM5_CLK_ENABLE();            //ʹ��TIM5ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_1;            //PA1
    GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;  	//���츴������
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}
