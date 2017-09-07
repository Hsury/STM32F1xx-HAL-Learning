#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "24l01.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验32
 NRF24L01无线通信实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{	
	u8 key,mode;
	u16 t=0;			 
	u8 tmp_buf[33];	
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口	
	LED_Init();							//初始化LED	
	KEY_Init();							//初始化按键
 	LCD_Init();           				//初始化LCD FSMC接口 
	NRF24L01_Init();    		    	//初始化NRF24L01 
	
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"NRF24L01 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/6/1");	 		
	while(NRF24L01_Check())
	{
		LCD_ShowString(30,130,200,16,16,"NRF24L01 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}
	LCD_ShowString(30,130,200,16,16,"NRF24L01 OK"); 	 
 	while(1)
	{	
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			mode=0;   
			break;
		}else if(key==KEY1_PRES)
		{
			mode=1;
			break;
		}
		t++;
		if(t==100)LCD_ShowString(10,150,230,16,16,"KEY0:RX_Mode  KEY1:TX_Mode"); //闪烁显示提示信息
 		if(t==200)
		{	
			LCD_Fill(10,150,230,150+16,WHITE);
			t=0; 
		}
		delay_ms(5);	  
	}   
 	LCD_Fill(10,150,240,166,WHITE);//清空上面的显示		  
 	POINT_COLOR=BLUE;//设置字体为蓝色	   
	if(mode==0)//RX模式
	{
		LCD_ShowString(30,150,200,16,16,"NRF24L01 RX_Mode");	
		LCD_ShowString(30,170,200,16,16,"Received DATA:");	
		NRF24L01_RX_Mode();		  
		while(1)
		{	  		    		    				 
			if(NRF24L01_RxPacket(tmp_buf)==0)//一旦接收到信息,则显示出来.
			{
				tmp_buf[32]=0;//加入字符串结束符
				LCD_ShowString(0,190,lcddev.width-1,32,16,tmp_buf);    
			}else delay_us(100);	   
			t++;
			if(t==10000)//大约1s钟改变一次状态
			{
				t=0;
				LED0=!LED0;
			} 				    
		};	
	}else//TX模式
	{							    
		LCD_ShowString(30,150,200,16,16,"NRF24L01 TX_Mode");	
		NRF24L01_TX_Mode();
		mode=' ';//从空格键开始  
		while(1)
		{	  		   				 
			if(NRF24L01_TxPacket(tmp_buf)==TX_OK)
			{
				LCD_ShowString(30,170,239,32,16,"Sended DATA:");	
				LCD_ShowString(0,190,lcddev.width-1,32,16,tmp_buf); 
				key=mode;
				for(t=0;t<32;t++)
				{
					key++;
					if(key>('~'))key=' ';
					tmp_buf[t]=key;	
				}
				mode++; 
				if(mode>'~')mode=' ';  	  
				tmp_buf[32]=0;//加入结束符		   
			}else
			{										   	
 				LCD_Fill(0,170,lcddev.width,170+16*3,WHITE);//清空显示			   
				LCD_ShowString(30,170,lcddev.width-1,32,16,"Send Failed "); 
			};
			LED0=!LED0;
			delay_ms(1500);				    
		};
	} 
}

