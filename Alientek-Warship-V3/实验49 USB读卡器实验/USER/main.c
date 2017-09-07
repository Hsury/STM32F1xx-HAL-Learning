#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "mass_mal.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "memory.h"	    
#include "usb_bot.h" 
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验49
 USB读卡器实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

extern u8 Max_Lun;	//支持的磁盘个数,0表示1个,1表示2个.

int main(void)
{
	u8 offline_cnt=0;
	u8 tct=0;
	u8 USB_STA;
	u8 Divece_STA; 
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口
	LED_Init();							//初始化LED	
 	LCD_Init();           				//初始化LCD FSMC接口
	W25QXX_Init();						//初始化W25Q128
 	my_mem_init(SRAMIN);				//初始化内部内存池
	
 	POINT_COLOR=RED;					//设置字体为红色	   
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"USB Card Reader TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/6/5");	
 	W25QXX_Init();
	if(SD_Init())
	{
		Max_Lun=0;											//SD卡错误,则仅只有一个磁盘.
		LCD_ShowString(30,130,200,16,16,"SD Card Error!");	//检测SD卡错误
	}else //SD 卡正常
	{   															  
		LCD_ShowString(30,130,200,16,16,"SD Card Size:     MB");
 	    Mass_Memory_Size[1]=SDCardInfo.CardCapacity;		//得到SD卡容量（字节），当SD卡容量超过4G的时候,需要用到两个u32来表示
	    Mass_Block_Size[1] =512;							//因为我们在Init里面设置了SD卡的操作字节为512个,所以这里一定是512个字节.
	    Mass_Block_Count[1]=Mass_Memory_Size[1]/Mass_Block_Size[1];
 		LCD_ShowNum(134,130,Mass_Memory_Size[1]>>20,5,16);	//显示SD卡容量
 	}
	if(W25QXX_TYPE!=W25Q128)LCD_ShowString(30,130,200,16,16,"W25Q128 Error!");	//检测SD卡错误
	else //SPI FLASH 正常
	{   															  
 	   	Mass_Memory_Size[0]=1024*1024*12;	//前12M字节
	    Mass_Block_Size[0] =512;			//设置SPI FLASH的操作扇区大小为512
	    Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];
		LCD_ShowString(30,150,200,16,16,"SPI FLASH Size:12MB");	 
	} 
	delay_ms(1800);
	USB_Port_Set(0); 	//USB先断开
	delay_ms(700);
	USB_Port_Set(1);	//USB再次连接 
	LCD_ShowString(30,170,200,16,16,"USB Connecting...");	//提示USB开始连接	 
  	Data_Buffer=mymalloc(SRAMIN,BULK_MAX_PACKET_SIZE*2*4);	//为USB数据缓存区申请内存
	Bulk_Data_Buff=mymalloc(SRAMIN,BULK_MAX_PACKET_SIZE);	//申请内存
 	//USB配置
 	USB_Interrupts_Config();    
 	Set_USBClock();   
 	USB_Init();	    
	delay_ms(1800);	   	    
	while(1)
	{	
		delay_ms(1);				  
		if(USB_STA!=USB_STATUS_REG)//状态改变了 
		{	 						   
			LCD_Fill(30,190,240,190+16,WHITE);//清除显示			  	   
			if(USB_STATUS_REG&0x01)//正在写		  
			{
				LCD_ShowString(30,190,200,16,16,"USB Writing...");//提示USB正在写入数据	 
			}
			if(USB_STATUS_REG&0x02)//正在读
			{
				LCD_ShowString(30,190,200,16,16,"USB Reading...");//提示USB正在读出数据  		 
			}	 										  
			if(USB_STATUS_REG&0x04)LCD_ShowString(30,210,200,16,16,"USB Write Err ");//提示写入错误
			else LCD_Fill(30,210,240,210+16,WHITE);//清除显示	  
			if(USB_STATUS_REG&0x08)LCD_ShowString(30,230,200,16,16,"USB Read  Err ");//提示读出错误
			else LCD_Fill(30,230,240,230+16,WHITE);//清除显示    
			USB_STA=USB_STATUS_REG;//记录最后的状态
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==CONFIGURED)LCD_ShowString(30,170,200,16,16,"USB Connected    ");//提示USB连接已经建立
			else LCD_ShowString(30,170,200,16,16,"USB DisConnected ");//提示USB被拔出了
			Divece_STA=bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct=0;
			LED0=!LED0;//提示系统在运行
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB连接了,则清除offline计数器
				bDeviceState=CONFIGURED;
			}else//没有得到轮询 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=UNCONNECTED;//2s内没收到在线标记,代表USB被拔出了
			}
			USB_STATUS_REG=0;
		}
	};
}

