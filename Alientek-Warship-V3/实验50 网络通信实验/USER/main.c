#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sram.h"
#include "rtc.h"
#include "beep.h"
#include "timer.h"
#include "adc.h"
#include "malloc.h" 
#include "dm9000.h"
#include "string.h"
#include "usmart.h"	
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "udp_demo.h"
#include "tcp_client_demo.h"
#include "tcp_server_demo.h"
#include "httpd.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验50
 网络通信实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/  

/*ALIENTEK为LWIP学习专门编写手册《ALIENTEK STM32F1 LWIP使用教程.pdf》，详细说明请参考手册。*/
 
extern u8 udp_demo_flag;  //UDP 测试全局状态标记变量
//加载UI
//mode:
//bit0:0,不加载;1,加载前半部分UI
//bit1:0,不加载;1,加载后半部分UI
void lwip_test_ui(u8 mode)
{
	u8 speed;
	u8 buf[30]; 
	POINT_COLOR=RED;
	if(mode&1<<0)
	{
		LCD_Fill(30,30,lcddev.width,110,WHITE);	//清除显示
		LCD_ShowString(30,30,200,16,16,"WarShip STM32F1");
		LCD_ShowString(30,50,200,16,16,"Ethernet lwIP Test");
		LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
		LCD_ShowString(30,90,200,16,16,"2017/6/5"); 	
	}
	if(mode&1<<1)
	{
		LCD_Fill(30,110,lcddev.width,lcddev.height,WHITE);	//清除显示
		LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
		if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印动态IP地址
		else sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印静态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		speed=DM9000_Get_SpeedAndDuplex();//得到网速
		if(speed&1<<1)LCD_ShowString(30,150,200,16,16,"Ethernet Speed:10M");
		else LCD_ShowString(30,150,200,16,16,"Ethernet Speed:100M");
		LCD_ShowString(30,170,200,16,16,"KEY0:TCP Server Test");
		LCD_ShowString(30,190,200,16,16,"KEY1:TCP Client Test");
		LCD_ShowString(30,210,200,16,16,"KEY2:UDP Test");
	}
}

int main(void)
{	
	u8 t;
	u8 key;
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口	
	usmart_dev.init(72);				//初始化USMART		 
	LED_Init();							//初始化LED	
	KEY_Init();							//初始化按键
 	LCD_Init();           				//初始化LCD FSMC接口 
	KEY_Init();	 						//初始化按键
	BEEP_Init();						//蜂鸣器初始化
	RTC_Init();							//RTC初始化
	MY_ADC_Init();                  	//初始化ADC1通道1
	TIM3_Init(999,719);					//定时器3频率为100hz
	SRAM_Init();						//初始化外部SRAM
	my_mem_init(SRAMIN);				//初始化内部内存池
	my_mem_init(SRAMEX);				//初始化外部内存池
	POINT_COLOR = RED; 		
	lwip_test_ui(1);					//加载前半部分UI		    
	while(lwip_comm_init()) 			//lwip初始化
	{
		LCD_ShowString(30,110,200,20,16,"LWIP Init Falied!");
		delay_ms(1200);
		LCD_Fill(30,110,230,130,WHITE); //清除显示
		LCD_ShowString(30,110,200,16,16,"Retrying...");  
	}
	LCD_ShowString(30,110,200,20,16,"LWIP Init Success!");
 	LCD_ShowString(30,130,200,16,16,"DHCP IP configing...");
#if LWIP_DHCP   //使用DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
	{
		lwip_periodic_handle();	//LWIP内核需要定时处理的函数
		lwip_pkt_handle();
	}
#endif
	lwip_test_ui(2);		//加载后半部分UI 
	httpd_init();  			//Web Server模式
  	while(1)
	{	
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES://TCP Server模式
				tcp_server_test();
				lwip_test_ui(3);//重新加载UI
				break;
			case KEY1_PRES://TCP Client模式
				tcp_client_test();
				lwip_test_ui(3);//重新加载UI
				break; 
			case KEY2_PRES://UDP模式
				udp_demo_test();
				lwip_test_ui(3);//重新加载UI
				break; 
		}
		lwip_periodic_handle();
		lwip_pkt_handle();
		delay_ms(2);
		t++;
		if(t==100)LCD_ShowString(30,230,200,16,16,"Please choose a mode!");
		if(t==200)
		{ 
			t=0;
			LCD_Fill(30,230,230,230+16,WHITE);//清除显示
			LED0=!LED0;
		} 
	}
}

