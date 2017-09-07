#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "usmart.h"
#include "rtc.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验15
 RTC实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{	
	u8 t;
	
	SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */ 
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口
	LED_Init();							//初始化LED	
 	LCD_Init();           				//初始化LCD FSMC接口
	usmart_dev.init(84); 		   		//初始化USMART	
    RTC_Init();                     	//初始化RTC 
   
	POINT_COLOR=RED;		//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32F103 ^_^");	
	LCD_ShowString(30,70,200,16,16,"RTC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/5/27");	
	while(RTC_Init())		//RTC初始化	，一定要初始化成功
	{ 
		LCD_ShowString(30,130,200,16,16,"RTC ERROR!   ");	
		delay_ms(800);
		LCD_ShowString(30,130,200,16,16,"RTC Trying...");	
	}		    						
	//显示时间
	POINT_COLOR=BLUE;//设置字体为蓝色					 
	LCD_ShowString(30,130,200,16,16,"    -  -     ");	   
	LCD_ShowString(30,166,200,16,16,"  :  :  ");	 		    
	while(1)
	{								    
		if(t!=calendar.sec)
		{
			t=calendar.sec;
			LCD_ShowNum(30,130,calendar.w_year,4,16);									  
			LCD_ShowNum(70,130,calendar.w_month,2,16);									  
			LCD_ShowNum(94,130,calendar.w_date,2,16);	 
			switch(calendar.week)
			{
				case 0:
					LCD_ShowString(30,148,200,16,16,"Sunday   ");
					break;
				case 1:
					LCD_ShowString(30,148,200,16,16,"Monday   ");
					break;
				case 2:
					LCD_ShowString(30,148,200,16,16,"Tuesday  ");
					break;
				case 3:
					LCD_ShowString(30,148,200,16,16,"Wednesday");
					break;
				case 4:
					LCD_ShowString(30,148,200,16,16,"Thursday ");
					break;
				case 5:
					LCD_ShowString(30,148,200,16,16,"Friday   ");
					break;
				case 6:
					LCD_ShowString(30,148,200,16,16,"Saturday ");
					break;  
			}
			LCD_ShowNum(30,166,calendar.hour,2,16);									  
			LCD_ShowNum(54,166,calendar.min,2,16);									  
			LCD_ShowNum(78,166,calendar.sec,2,16);
			LED0=!LED0;
		}	
		delay_ms(10);								  
	}; 
}

