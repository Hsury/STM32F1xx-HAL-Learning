#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "ds18b20.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��30
 DS18B20�����¶ȴ�����-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{	
	u8 t=0;			    
	short temperature; 
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������	
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
 	LCD_Init();           				//��ʼ��LCD FSMC�ӿ�
    
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"DS18B20 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/5/31");
	while(DS18B20_Init())	        //DS18B20��ʼ��	
	{
		LCD_ShowString(30,130,200,16,16,"DS18B20 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
		delay_ms(200);
	}   
	LCD_ShowString(30,130,200,16,16,"DS18B20 OK");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(30,150,200,16,16,"Temp:   . C");	 
	while(1)
	{	    	    
		if(t%10==0)//ÿ100ms��ȡһ��
		{		
		    temperature=DS18B20_Get_Temp();	
			if(temperature<0)
			{
				LCD_ShowChar(30+40,150,'-',16,0);			//��ʾ����
				temperature=-temperature;					//תΪ����
			}else LCD_ShowChar(30+40,150,' ',16,0);			//ȥ������
			LCD_ShowNum(30+40+8,150,temperature/10,2,16);	//��ʾ��������	    
			LCD_ShowNum(30+40+32,150,temperature%10,1,16);	//��ʾС������ 		   
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

