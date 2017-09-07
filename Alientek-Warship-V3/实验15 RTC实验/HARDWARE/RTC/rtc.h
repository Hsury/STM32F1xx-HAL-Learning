#ifndef __RTC_H
#define __RTC_H	  
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板V3
//RTC驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2017/5/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

extern RTC_HandleTypeDef RTC_Handler;  //RTC句柄
												    
//时间结构体
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//公历日月年周
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;	
}_calendar_obj;					 
extern _calendar_obj calendar;				//日历结构体

u8 RTC_Init(void);        					//初始化RTC,返回0,失败;1,成功;
u8 Is_Leap_Year(u16 year);					//平年,闰年判断
u8 RTC_Get(void);         					//获取时间   
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);		//设置时间	
u8 RTC_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);	//设置闹钟	
#endif



















