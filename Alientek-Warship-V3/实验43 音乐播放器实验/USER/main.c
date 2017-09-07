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
#include "ff.h"
#include "exfuns.h"
#include "string.h"
#include "usmart.h"
#include "fontupd.h"
#include "text.h"	
#include "vs10xx.h"
#include "mp3player.h"	
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��43
 ���ֲ�����ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/  
    
int main(void)
{	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������	
	usmart_dev.init(72);				//��ʼ��USMART	
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
 	LCD_Init();           				//��ʼ��LCD FSMC�ӿ� 
	SRAM_Init();						//��ʼ���ⲿSRAM  
 	VS_Init();	  						//��ʼ��VS1053 
 	my_mem_init(SRAMIN);				//��ʼ���ڲ��ڴ��
	exfuns_init();						//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 				//����SD�� 
 	f_mount(fs[1],"1:",1); 				//����FLASH.
	POINT_COLOR=RED;       
 	while(font_init()) 					//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
	}
 	Show_Str(30,50,200,16,"ս�� STM32������",16,0);				    	 
	Show_Str(30,70,200,16,"���ֲ�����ʵ��",16,0);				    	 
	Show_Str(30,90,200,16,"����ԭ��@ALIENTEK",16,0);				    	 
	Show_Str(30,110,200,16,"2017��6��2��",16,0);
	Show_Str(30,130,200,16,"KEY0:NEXT   KEY2:PREV",16,0);
	Show_Str(30,150,200,16,"KEY_UP:VOL+ KEY1:VOL-",16,0);
	while(1)
	{
  		LED1=0; 	  
		Show_Str(30,170,200,16,"�洢������...",16,0);
		printf("Ram Test:0X%04X\r\n",VS_Ram_Test());//��ӡRAM���Խ��	    
		Show_Str(30,170,200,16,"���Ҳ�����...",16,0); 	 	 
 		VS_Sine_Test();	   
		Show_Str(30,170,200,16,"<<���ֲ�����>>",16,0); 		 
		LED1=1;
		mp3_play();
	} 
}

