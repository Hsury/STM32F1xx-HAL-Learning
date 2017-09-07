#include "usb_lib.h" 
#include "mass_mal.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "led.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板V3
//USB-hw_config 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
 
 
 /////////////////////////////////////////////////////////////////////////////////
//各USB例程通用部分代码,ST各各USB例程,此部分代码都可以共用.
//此部分代码一般不需要修改!

//USB唤醒中断服务函数
void USBWakeUp_IRQHandler(void) 
{
	__HAL_USB_WAKEUP_EXTI_CLEAR_FLAG();//清除USB唤醒中断挂起位
} 

//USB中断处理函数
void USB_LP_CAN1_RX0_IRQHandler(void) 
{
	USB_Istr();
} 

//USB时钟配置函数,USBclk=48Mhz@HCLK=72Mhz
void Set_USBClock(void)
{
	RCC_PeriphCLKInitTypeDef USBClkInit;

	USBClkInit.PeriphClockSelection=RCC_PERIPHCLK_USB; 		//USB外设时钟
	USBClkInit.UsbClockSelection=RCC_USBCLKSOURCE_PLL_DIV1_5;
	HAL_RCCEx_PeriphCLKConfig(&USBClkInit);//USBclk=PLLclk/1.5=48Mhz	
	
	__HAL_RCC_USB_CLK_ENABLE();	//USB时钟使能
} 

//USB进入低功耗模式
//当USB进入suspend模式时,MCU进入低功耗模式
//需自行添加低功耗代码(比如关时钟等)
void Enter_LowPowerMode(void)
{
 	printf("usb enter low power mode\r\n");
	bDeviceState=SUSPENDED;
} 

//USB退出低功耗模式
//用户可以自行添加相关代码(比如重新配置时钟等)
void Leave_LowPowerMode(void)
{
	DEVICE_INFO *pInfo=&Device_Info;
	printf("leave low power mode\r\n"); 
	if (pInfo->Current_Configuration!=0)bDeviceState=CONFIGURED; 
	else bDeviceState = ATTACHED; 
} 

//USB中断配置
void USB_Interrupts_Config(void)
{ 
	/* Configure the EXTI line 18 connected internally to the USB IP */
	__HAL_USB_WAKEUP_EXTI_CLEAR_FLAG();			//清除USB唤醒中断挂起位
											  
    __HAL_USB_WAKEUP_EXTI_ENABLE_RISING_EDGE();	//上升沿
    __HAL_USB_WAKEUP_EXTI_ENABLE_IT();			//  开启线18上的中断 

	/* Enable the USB interrupt */
	HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 1, 0);//组2，优先级次之 
	HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	
	/* Enable the USB Wake-up interrupt */
	HAL_NVIC_EnableIRQ(USBWakeUp_IRQn);
    HAL_NVIC_SetPriority(USBWakeUp_IRQn,0,0);  
}	

//USB接口配置(配置1.5K上拉电阻,战舰V3不需要配置,恒上拉)
//NewState:DISABLE,不上拉
//         ENABLE,上拉
void USB_Cable_Config (FunctionalState NewState)
{ 
	if (NewState!=DISABLE)printf("usb pull up enable\r\n"); 
	else printf("usb pull up disable\r\n"); 
}

//USB使能连接/断线
//enable:0,断开
//       1,允许连接	   
void USB_Port_Set(u8 enable)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();         //开启GPIOA时钟		 
	if(enable)_SetCNTR(_GetCNTR()&(~(1<<1)));//退出断电模式
	else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  // 断电模式
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		PAout(12)=0;	    		  
	}
}  

//获取STM32的唯一ID
//用于USB配置信息
void Get_SerialNum(void)
{
	u32 Device_Serial0, Device_Serial1, Device_Serial2;
	Device_Serial0 = *(u32*)(0x1FFFF7E8);
	Device_Serial1 = *(u32*)(0x1FFFF7EC);
	Device_Serial2 = *(u32*)(0x1FFFF7F0);
	Device_Serial0 += Device_Serial2;
	if (Device_Serial0 != 0)
	{
		IntToUnicode (Device_Serial0,&MASS_StringSerial[2] , 8);
		IntToUnicode (Device_Serial1,&MASS_StringSerial[18], 4);
	}
} 

//将32位的值转换成unicode.
//value,要转换的值(32bit)
//pbuf:存储地址
//len:要转换的长度
void IntToUnicode (u32 value , u8 *pbuf , u8 len)
{
	u8 idx = 0;
	for( idx = 0 ; idx < len ; idx ++)
	{
		if( ((value >> 28)) < 0xA )
		{
			pbuf[ 2* idx] = (value >> 28) + '0';
		}
		else
		{
			pbuf[2* idx] = (value >> 28) + 'A' - 10; 
		} 
		value = value << 4; 
		pbuf[ 2* idx + 1] = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////////

//标记USB正在进行读/写操作
void Led_RW_ON(void)
{
	LED1=0;
}
 
//标记USB读/写操作已经完成
void Led_RW_OFF(void)
{
	LED1=1;
}
//提示USB还未完成配置
void USB_NotConfigured_LED(void)
{
	printf("USB NotConfigured\r\n");
}




















