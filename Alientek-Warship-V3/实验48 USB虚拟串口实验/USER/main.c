#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验48
 USB虚拟串口实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{
 	u16 t;
	u16 len;	
	u16 times=0;    
	u8 usbstatus=0;	
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口
	LED_Init();							//初始化LED	
 	LCD_Init();           				//初始化LCD FSMC接口
	
 	POINT_COLOR=RED;					//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"USB Virtual USART TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/6/5");  
 	LCD_ShowString(30,130,200,16,16,"USB Connecting...");//提示USB开始连接
	delay_ms(1800);
	USB_Port_Set(0); 	//USB先断开
	delay_ms(700);
	USB_Port_Set(1);	//USB再次连接
 	Set_USBClock();   
 	USB_Interrupts_Config();    
 	USB_Init();	    
	while(1)
	{
		if(usbstatus!=bDeviceState)//USB连接状态发生了改变.
		{
			usbstatus=bDeviceState;//记录新的状态
			if(usbstatus==CONFIGURED)
			{
				POINT_COLOR=BLUE;
				LCD_ShowString(30,130,200,16,16,"USB Connected    ");//提示USB连接成功
				LED1=0;//DS1亮
			}else
			{
				POINT_COLOR=RED;
				LCD_ShowString(30,130,200,16,16,"USB disConnected ");//提示USB断开
				LED1=1;//DS1灭
			}
		}
		if(USB_USART_RX_STA&0x8000)
		{					   
			len=USB_USART_RX_STA&0x3FFF;//得到此次接收到的数据长度
			usb_printf("\r\n您发送的消息为:%d\r\n\r\n",len);
			for(t=0;t<len;t++)
			{
				USB_USART_SendData(USB_USART_RX_BUF[t]);//以字节方式,发送给USB 
			}
			usb_printf("\r\n\r\n");//插入换行
			USB_USART_RX_STA=0;
		}else
		{
			times++;
			if(times%5000==0)
			{
				usb_printf("\r\n战舰STM32开发板USB虚拟串口实验\r\n");
				usb_printf("正点原子@ALIENTEK\r\n\r\n");
			}
			if(times%200==0)usb_printf("请输入数据,以回车键结束\r\n");  
			if(times%30==0)LED0=!LED0;//闪烁LED,提示系统正在运行.
			delay_ms(10);   
		}
	}
}

