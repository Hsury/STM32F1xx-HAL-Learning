#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sram.h"
#include "malloc.h" 
#include "sdio_sdcard.h"
#include "w25qxx.h"
#include "ov7670.h" 
#include "timer.h"
#include "exti.h"
#include "beep.h" 
#include "ff.h"
#include "exfuns.h"
#include "string.h"
#include "usmart.h"
#include "fontupd.h"
#include "text.h"	
#include "piclib.h"	
#include "string.h"		
#include "math.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验42
 照相机实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/  
  
extern u8 ov_sta;	//在exit.c里面定义
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
//文件名自增（避免覆盖）
//组合成:形如"0:PHOTO/PIC13141.bmp"的文件名
void camera_new_pathname(u8 *pname)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		sprintf((char*)pname,"0:PHOTO/PIC%05d.bmp",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//尝试打开这个文件
		if(res==FR_NO_FILE)break;		//该文件名不存在=正是我们需要的.
		index++;
	}
}

int main(void)
{	
	u8 res;							 
	u8 *pname;							//带路径的文件名 
	u8 key;								//键值		   
	u8 i;						 
	u8 sd_ok=1;							//0,sd卡不正常;1,SD卡正常.
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口	
	usmart_dev.init(72);				//初始化USMART	
	LED_Init();							//初始化LED	
	KEY_Init();							//初始化按键
	BEEP_Init();						//初始化蜂鸣器
 	LCD_Init();           				//初始化LCD FSMC接口 
	SRAM_Init();						//初始化外部SRAM  
	W25QXX_Init();						//初始化W25Q128
	my_mem_init(SRAMIN);				//初始化内部内存池
	my_mem_init(SRAMEX);				//初始化外部内存池
	exfuns_init();						//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 				//挂载SD卡 
 	f_mount(fs[1],"1:",1); 				//挂载FLASH.
	
	POINT_COLOR=RED;           
 	while(font_init()) 					//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
	}  	 
 	Show_Str(30,50,200,16,"战舰STM32F1开发板",16,0);				    	 
	Show_Str(30,70,200,16,"照相机实验",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:拍照",16,0);				    	 
	Show_Str(30,110,200,16,"正点原子@ALIENTEK",16,0);				    	 
	Show_Str(30,130,200,16,"2017年6月2日",16,0);
	res=f_mkdir("0:/PHOTO");		//创建PHOTO文件夹
	if(res!=FR_EXIST&&res!=FR_OK) 	//发生了错误
	{		    
		Show_Str(30,150,240,16,"SD卡错误!",16,0);
		delay_ms(200);				  
		Show_Str(30,170,240,16,"拍照功能将不可用!",16,0);
		sd_ok=0;  	
	}else
	{
		Show_Str(30,150,240,16,"SD卡正常!",16,0);
		delay_ms(200);				  
		Show_Str(30,170,240,16,"KEY0:拍照",16,0);
		sd_ok=1;  	  
	}										   						    
 	pname=mymalloc(SRAMIN,30);	//为带路径的文件名分配30个字节的内存		    
 	while(pname==NULL)			//内存分配出错
 	{	    
		Show_Str(30,190,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,190,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}   											  
	while(OV7670_Init())//初始化OV7670
	{
		Show_Str(30,190,240,16,"OV7670 错误!",16,0);
		delay_ms(200);
	    LCD_Fill(30,190,239,206,WHITE);
		delay_ms(200);
	}
 	Show_Str(30,190,200,16,"OV7670 正常",16,0);
	delay_ms(1500);	 		 
	TIM6_Init(10000,7199);				//10Khz计数频率,1秒钟中断									  
	EXTI8_Init();						//使能定时器捕获
	OV7670_Window_Set(12,176,240,320);	//设置窗口	  
  	OV7670_CS=0;	
	LCD_Clear(BLACK);
 	while(1)
	{	
		key=KEY_Scan(0);//不支持连按
		if(key==KEY0_PRES)
		{
			if(sd_ok)
			{
				LED1=0;	//点亮DS1,提示正在拍照
				camera_new_pathname(pname);//得到文件名		    
				if(bmp_encode(pname,(lcddev.width-240)/2,(lcddev.height-320)/2,240,320,0))//拍照有误
				{
					Show_Str(40,130,240,12,"写入文件错误!",12,0);		 
				}else 
				{
					Show_Str(40,130,240,12,"拍照成功!",12,0);
					Show_Str(40,150,240,12,"保存为:",12,0);
 					Show_Str(40+42,150,240,12,pname,12,0);		    
 					BEEP=1;	//蜂鸣器短叫，提示拍照完成
					delay_ms(100);
		 		}
			}else //提示SD卡错误
			{					    
				Show_Str(40,130,240,12,"SD卡错误!",12,0);
 				Show_Str(40,150,240,12,"拍照功能不可用!",12,0);			    
 			}
 		 	BEEP=0;//关闭蜂鸣器
			LED1=1;//关闭DS1
			delay_ms(1800);//等待1.8秒钟
			LCD_Clear(BLACK);
		}else delay_ms(5);
 		camera_refresh();//更新显示
		i++;
		if(i==40)//DS0闪烁.
		{
			i=0;
			LED0=!LED0;
 		}
	}	
}

