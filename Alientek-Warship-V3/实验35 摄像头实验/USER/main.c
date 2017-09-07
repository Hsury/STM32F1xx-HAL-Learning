#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "ov7670.h"
#include "timer.h"
#include "exti.h"
#include "tpad.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验34
 摄像头OV7670实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

const u8*LMODE_TBL[5]={"Auto","Sunny","Cloudy","Office","Home"};							//5种光照模式	    
const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7种特效 
extern u8 ov_sta;	//在exit.c里 面定义
extern u8 ov_frame;	//在timer.c里面定义	

//更新LCD显示
void camera_refresh(void)
{
	u32 j;
 	u16 color;	 
	if(ov_sta)//有帧中断更新？
	{
		LCD_Scan_Dir(U2D_L2R);		//从上到下,从左到右  
		if(lcddev.id==0X1963)LCD_Set_Window((lcddev.width-240)/2,(lcddev.height-320)/2,240,320);//将显示区域设置到屏幕中央
		else if(lcddev.id==0X5510||lcddev.id==0X5310)LCD_Set_Window((lcddev.width-320)/2,(lcddev.height-240)/2,320,240);//将显示区域设置到屏幕中央
		LCD_WriteRAM_Prepare();     //开始写入GRAM	
		OV7670_RRST=0;				//开始复位读指针 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//复位读指针结束 
		OV7670_RCK_H;
		for(j=0;j<76800;j++)
		{
			OV7670_RCK_L;
			color=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK_H; 
			color<<=8;  
			OV7670_RCK_L;
			color|=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK_H; 
			LCD->LCD_RAM=color;    
		}   							  
 		ov_sta=0;					//清零帧中断标记
		ov_frame++; 
		LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 
	} 
}

int main(void)
{	
	u8 key;
	u8 lightmode=0,saturation=2,brightness=2,contrast=2;
	u8 effect=0;	 
 	u8 i=0;	    
	u8 msgbuf[15];						//消息缓存区
	u8 tm=0; 
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口	
	LED_Init();							//初始化LED	
	KEY_Init();							//初始化按键
 	LCD_Init();           				//初始化LCD FSMC接口 
	TPAD_Init(6);						//触摸按键初始化 
	
 	POINT_COLOR=RED;					//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"OV7670 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/6/1"); 
	LCD_ShowString(30,130,200,16,16,"KEY0:Light Mode");
	LCD_ShowString(30,150,200,16,16,"KEY1:Saturation");
	LCD_ShowString(30,170,200,16,16,"KEY2:Brightness");
	LCD_ShowString(30,190,200,16,16,"KEY_UP:Contrast");
	LCD_ShowString(30,210,200,16,16,"TPAD:Effects");	 
  	LCD_ShowString(30,230,200,16,16,"OV7670 Init...");	  
	while(OV7670_Init())//初始化OV7670
	{
		LCD_ShowString(30,230,200,16,16,"OV7670 Error!!");
		delay_ms(200);
	    LCD_Fill(30,230,239,246,WHITE);
		delay_ms(200);
	}
 	LCD_ShowString(30,230,200,16,16,"OV7670 Init OK");
	delay_ms(1500);	 	   
	OV7670_Light_Mode(lightmode);
	OV7670_Color_Saturation(saturation);
	OV7670_Brightness(brightness);
	OV7670_Contrast(contrast);
 	OV7670_Special_Effects(effect);	 
	TIM6_Init(10000,7199);				//10Khz计数频率,1秒钟中断									  
	EXTI8_Init();						//使能定时器捕获
	OV7670_Window_Set(12,176,240,320);	//设置窗口	  
  	OV7670_CS=0;					
	LCD_Clear(BLACK);
 	while(1)
	{	
		key=KEY_Scan(0);//不支持连按
		if(key)
		{
			tm=20;
			switch(key)
			{				    
				case KEY0_PRES:	//灯光模式Light Mode
					lightmode++;
					if(lightmode>4)lightmode=0;
					OV7670_Light_Mode(lightmode);
					sprintf((char*)msgbuf,"%s",LMODE_TBL[lightmode]);
					break;
				case KEY1_PRES:	//饱和度Saturation
					saturation++;
					if(saturation>4)saturation=0;
					OV7670_Color_Saturation(saturation);
					sprintf((char*)msgbuf,"Saturation:%d",(signed char)saturation-2);
					break;
				case KEY2_PRES:	//亮度Brightness				 
					brightness++;
					if(brightness>4)brightness=0;
					OV7670_Brightness(brightness);
					sprintf((char*)msgbuf,"Brightness:%d",(signed char)brightness-2);
					break;
				case WKUP_PRES:	//对比度Contrast			    
					contrast++;
					if(contrast>4)contrast=0;
					OV7670_Contrast(contrast);
					sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-2);
					break;
			}
		}	 
		if(TPAD_Scan(0))//检测到触摸按键 
		{
			effect++;
			if(effect>6)effect=0;
			OV7670_Special_Effects(effect);//设置特效
	 		sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
			tm=20;
		} 
		camera_refresh();//更新显示
 		if(tm)
		{
			LCD_ShowString((lcddev.width-240)/2+30,(lcddev.height-320)/2+60,200,16,16,msgbuf);
			tm--;
		}
		i++;
		if(i==15)//DS0闪烁.
		{
			i=0;
			LED0=!LED0;
 		}
	}	
}

