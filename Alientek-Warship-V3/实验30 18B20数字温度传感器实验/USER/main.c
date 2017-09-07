#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "ds18b20.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验30
 DS18B20数字温度传感器-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{	
	u8 t=0;			    
	short temperature; 
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口	
	LED_Init();							//初始化LED	
	KEY_Init();							//初始化按键
 	LCD_Init();           				//初始化LCD FSMC接口
    
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"DS18B20 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/5/31");
	while(DS18B20_Init())	        //DS18B20初始化	
	{
		LCD_ShowString(30,130,200,16,16,"DS18B20 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
		delay_ms(200);
	}   
	LCD_ShowString(30,130,200,16,16,"DS18B20 OK");
	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(30,150,200,16,16,"Temp:   . C");	 
	while(1)
	{	    	    
		if(t%10==0)//每100ms读取一次
		{		
		    temperature=DS18B20_Get_Temp();	
			if(temperature<0)
			{
				LCD_ShowChar(30+40,150,'-',16,0);			//显示负号
				temperature=-temperature;					//转为正数
			}else LCD_ShowChar(30+40,150,' ',16,0);			//去掉负号
			LCD_ShowNum(30+40+8,150,temperature/10,2,16);	//显示正数部分	    
			LCD_ShowNum(30+40+32,150,temperature%10,1,16);	//显示小数部分 		   
		}				   
		delay_ms(10);
		t++;
		if(t==20)
		{
			t=0;
			LED0=!LED0;
		}
	}
}

