#include "adc3.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103开发板
//ADC3驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/5/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

ADC_HandleTypeDef ADC3_Handler;		//ADC句柄

//初始化ADC
//ch: ADC_channels 
//通道值 0~16取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
void ADC3_Init(void)
{ 
	RCC_PeriphCLKInitTypeDef ADC_CLKInit;
	
	ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;			//ADC外设时钟
	ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;			//分频因子6时钟为72M/6=12MHz
	HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					//设置ADC时钟
	
    ADC3_Handler.Instance=ADC3;
    ADC3_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //右对齐
    ADC3_Handler.Init.ScanConvMode=DISABLE;                      //非扫描模式
    ADC3_Handler.Init.ContinuousConvMode=DISABLE;                //关闭连续转换
    ADC3_Handler.Init.NbrOfConversion=1;                         //1个转换在规则序列中 也就是只转换规则序列1 
    ADC3_Handler.Init.DiscontinuousConvMode=DISABLE;             //禁止不连续采样模式
    ADC3_Handler.Init.NbrOfDiscConversion=0;                     //不连续采样通道数为0
    ADC3_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //软件触发
    HAL_ADC_Init(&ADC3_Handler);                                 //初始化 
	
	HAL_ADCEx_Calibration_Start(&ADC3_Handler);					 //校准ADC
}

//ADC底层驱动，引脚配置，时钟使能
//此函数会被HAL_ADC_Init()调用
//hadc:ADC句柄
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    __HAL_RCC_ADC3_CLK_ENABLE();            //使能ADC1时钟
}

//获得ADC值
//ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
//返回值:转换结果
u16 Get_Adc3(u32 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                   //通道
    ADC1_ChanConf.Rank=1;                                       //第1个序列，序列1
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_239CYCLES_5;      //采样时间为239.5周期	              
    HAL_ADC_ConfigChannel(&ADC3_Handler,&ADC1_ChanConf);        //通道配置
	
    HAL_ADC_Start(&ADC3_Handler);                               //开启ADC
	
    HAL_ADC_PollForConversion(&ADC3_Handler,10);                //轮询转换
 
	return (u16)HAL_ADC_GetValue(&ADC3_Handler);	        	//返回最近一次ADC1规则组的转换结果
}
