#include  "joypad.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32F103������
//��Ϸ�ֱ����� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2017/5/31
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//��ʼ���ֱ��ӿ�.	 
void JOYPAD_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOB_CLK_ENABLE();           	//����GPIOBʱ��
	__HAL_RCC_GPIOD_CLK_ENABLE();           	//����GPIODʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_11; 				//PB5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//�������
    GPIO_Initure.Pull=GPIO_PULLUP;          	//����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;    	 	//����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);

	GPIO_Initure.Pin=GPIO_PIN_3; 				//PD3
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_10; 				//PB10
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  		//����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);	//PB5��1��Ĭ�ϳ�ʼ�������
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET);	//PE5��1��Ĭ�ϳ�ʼ�������
	
	
	
// 	GPIO_InitTypeDef  GPIO_InitStructure;
// 	
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PB,PD�˿�ʱ��
//	
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 //
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��GPIO
// 	GPIO_SetBits(GPIOB,GPIO_Pin_11);	//����

// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_Init(GPIOD, &GPIO_InitStructure); //��ʼ��GPIO
// 	GPIO_SetBits(GPIOD,GPIO_Pin_3);	//����
//	
//	
// 	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
// 	GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��GPIO
//	GPIO_SetBits(GPIOB,GPIO_Pin_10);	//����

}

//�ֱ��ӳٺ���
//t:Ҫ�ӳٵ�ʱ��
void JOYPAD_Delay(u16 t)
{
	while(t--);
}

//��ȡ�ֱ�����ֵ.
//FC�ֱ����������ʽ:
//ÿ��һ������,���һλ����,���˳��:
//A->B->SELECT->START->UP->DOWN->LEFT->RIGHT.
//�ܹ�8λ,������C��ť���ֱ�,����C��ʵ�͵���A+Bͬʱ����.
//������1,�ɿ���0.
//����ֵ:
//[7]:��
//[6]:��
//[5]:��
//[4]:��
//[3]:Start
//[2]:Select
//[1]:B
//[0]:A
u8 JOYPAD_Read(void)
{
	vu8 temp=0;
 	u8 t;
	JOYPAD_LAT=1;	//���浱ǰ״̬
 	JOYPAD_Delay(80);
	JOYPAD_LAT=0; 
	for(t=0;t<8;t++)
	{
		temp>>=1;	 
		if(JOYPAD_DAT==0)temp|=0x80;//LOAD֮�󣬾͵õ���һ������
		JOYPAD_CLK=1;			   	//ÿ��һ�����壬�յ�һ������
		JOYPAD_Delay(80);
		JOYPAD_CLK=0;	
		JOYPAD_Delay(80); 
	}
	return temp;
}


