#include "adc3.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//ADC3��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/5/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

ADC_HandleTypeDef ADC3_Handler;		//ADC���

//��ʼ��ADC
//ch: ADC_channels 
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
void ADC3_Init(void)
{ 
	RCC_PeriphCLKInitTypeDef ADC_CLKInit;
	
	ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;			//ADC����ʱ��
	ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;			//��Ƶ����6ʱ��Ϊ72M/6=12MHz
	HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					//����ADCʱ��
	
    ADC3_Handler.Instance=ADC3;
    ADC3_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //�Ҷ���
    ADC3_Handler.Init.ScanConvMode=DISABLE;                      //��ɨ��ģʽ
    ADC3_Handler.Init.ContinuousConvMode=DISABLE;                //�ر�����ת��
    ADC3_Handler.Init.NbrOfConversion=1;                         //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC3_Handler.Init.DiscontinuousConvMode=DISABLE;             //��ֹ����������ģʽ
    ADC3_Handler.Init.NbrOfDiscConversion=0;                     //����������ͨ����Ϊ0
    ADC3_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //�������
    HAL_ADC_Init(&ADC3_Handler);                                 //��ʼ�� 
	
	HAL_ADCEx_Calibration_Start(&ADC3_Handler);					 //У׼ADC
}

//ADC�ײ��������������ã�ʱ��ʹ��
//�˺����ᱻHAL_ADC_Init()����
//hadc:ADC���
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    __HAL_RCC_ADC3_CLK_ENABLE();            //ʹ��ADC1ʱ��
}

//���ADCֵ
//ch: ͨ��ֵ 0~16��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
//����ֵ:ת�����
u16 Get_Adc3(u32 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                   //ͨ��
    ADC1_ChanConf.Rank=1;                                       //��1�����У�����1
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_239CYCLES_5;      //����ʱ��Ϊ239.5����	              
    HAL_ADC_ConfigChannel(&ADC3_Handler,&ADC1_ChanConf);        //ͨ������
	
    HAL_ADC_Start(&ADC3_Handler);                               //����ADC
	
    HAL_ADC_PollForConversion(&ADC3_Handler,10);                //��ѯת��
 
	return (u16)HAL_ADC_GetValue(&ADC3_Handler);	        	//�������һ��ADC1�������ת�����
}
