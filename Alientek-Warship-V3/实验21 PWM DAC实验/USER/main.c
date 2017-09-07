#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "adc.h"
#include "pwmdac.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验21
 PWM DAC实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//设置输出电压
//vol:0~330,代表0~3.3V
void PWM_DAC_Set(u16 vol)
{
	double temp=vol;
	temp/=100;
	temp=temp*256/3.3;
	TIM_SetTIM1Compare1(temp);	   
}

int main(void)
{	
	u16 adcx;
	float temp;
	u8 t=0;	 
	u16 pwmval=0;
	u8 key;
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口
	usmart_dev.init(84); 		   		//初始化USMART	
	LED_Init();							//初始化LED	
	KEY_Init();							//初始化按键
 	LCD_Init();           				//初始化LCD FSMC接口
	MY_ADC_Init();                  	//初始化ADC1通道1
    TIM1_CH1_PWM_Init(255,1);       	//TIM1 PWM初始化, Fpwm=72M/256=281.25Khz.
	
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"PWM DAC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/5/29");	 
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	  
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V"); 	
    
	TIM_SetTIM1Compare1(pwmval);	//初始值为0 	
    while(1)
	{
		t++;
		key=KEY_Scan(0);			  
		if(key==WKUP_PRES)
		{		 
			if(pwmval<250)pwmval+=10;
            TIM_SetTIM1Compare1(pwmval);	//输出		
		}else if(key==KEY1_PRES)	
		{
			if(pwmval>10)pwmval-=10;
			else pwmval=0;
            TIM_SetTIM1Compare1(pwmval);	//输出	
		}	 
		if(t==10||key==KEY1_PRES||key==WKUP_PRES) 	//WKUP/KEY1按下了,或者定时时间到了
		{	  
			adcx=HAL_TIM_ReadCapturedValue(&TIM1_Handler,TIM_CHANNEL_1);
			LCD_ShowxNum(94,150,adcx,3,16,0);     	//显示DAC寄存器值
			temp=(float)adcx*(3.3/256);;			//得到DAC电压值
			adcx=temp;
			LCD_ShowxNum(94,170,temp,1,16,0);     	//显示电压值整数部分
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0x80); 	//显示电压值的小数部分
			adcx=Get_Adc_Average(ADC_CHANNEL_1,20); //得到ADC转换值	  
			temp=(float)adcx*(3.3/4096);			//得到ADC电压值
			adcx=temp;
			LCD_ShowxNum(94,190,temp,1,16,0);     	//显示电压值整数部分
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0x80); 	//显示电压值的小数部分
			t=0;
			LED0=!LED0;	   
		 }	    
		delay_ms(10); 
	}
}

