#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H
#include "platform_config.h"
#include "usb_type.h" 
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
 
 
 
#define BULK_MAX_PACKET_SIZE  0x00000040	//包大小,最大64字节.



//USB通用代码函数声明
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void USB_Port_Set(u8 enable);
void IntToUnicode (u32 value,u8 *pbuf,u8 len);
void Get_SerialNum(void);


//不同USB功能添加的函数声明 
void Led_RW_ON(void);
void Led_RW_OFF(void);
void USB_NotConfigured_LED(void);

#endif  
























