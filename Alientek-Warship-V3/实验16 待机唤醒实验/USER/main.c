#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "usmart.h"
#include "wkup.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验16
 待机唤醒实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口
	usmart_dev.init(84); 		   		//初始化USMART	
	LED_Init();							//初始化LED	
 	LCD_Init();           				//初始化LCD FSMC接口
	WKUP_Init();	                	//待机唤醒初始化
	
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Warship STM32");	
	LCD_ShowString(30,70,200,16,16,"WKUP TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/5/29");	 		    
	while(1)
	{
		LED0=!LED0;
		delay_ms(250);
	} 
}

