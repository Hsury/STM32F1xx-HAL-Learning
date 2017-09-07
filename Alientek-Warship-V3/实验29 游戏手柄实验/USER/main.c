#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "joypad.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验29
 游戏手柄实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

const u8*JOYPAD_SYMBOL_TBL[8]={	"Right",	"Left",		"Down",		"Up",	
								"Start",	"Select",	"A",		"B"};//手柄按键符号定义
int main(void)
{	
	u8 key;
	u8 t=0,i=0;	
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口	
	LED_Init();							//初始化LED	
	KEY_Init();							//初始化按键
 	LCD_Init();           				//初始化LCD FSMC接口
    
	JOYPAD_Init();						//手柄初始化
 	POINT_COLOR=RED;					//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"JOYPAD TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/5/31");
	LCD_ShowString(30,130,200,16,16,"KEYVAL:");	      
	LCD_ShowString(30,150,200,16,16,"SYMBOL:");	      
 	POINT_COLOR=BLUE;//设置字体为红色 
	while(1)
	{
		key=JOYPAD_Read();
		if(key)
		{
			LCD_ShowNum(116,130,key,3,16);//显示键值
			for(i=0;i<8;i++)
			{
				if(key&(0X80>>i))
				{
					LCD_Fill(30+56,150,30+56+48,150+16,WHITE);//清除之前的显示
					LCD_ShowString(30+56,150,200,16,16,(u8*)JOYPAD_SYMBOL_TBL[i]);//显示符号
				}		
			}		    
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

