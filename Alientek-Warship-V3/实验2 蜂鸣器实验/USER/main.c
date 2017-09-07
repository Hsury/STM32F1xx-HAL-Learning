#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验2
 蜂鸣器实验-HAL库版本
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
	LED_Init();							//初始化LED	
	BEEP_Init();						//初始化蜂鸣器
	
	while(1)
	{
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); 	//DS0拉低，亮   等同LED0=0;
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);   	//BEEP引脚拉低， 等同BEEP=0;
        delay_ms(300);											//延时300ms
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);   	//DS0拉高，灭   等同LED0=1;
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); 	//BEEP引脚拉高， 等同BEEP=1;
        delay_ms(300);                                      	//延时300ms 
	}
}
