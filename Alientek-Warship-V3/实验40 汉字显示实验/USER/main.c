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
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��40
 ������ʾʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/  
    
int main(void)
{	
	u32 fontcnt;		  
	u8 i,j;
	u8 fontx[2];						//gbk��
	u8 key,t;
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������	
	usmart_dev.init(72);				//��ʼ��USMART	
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
 	LCD_Init();           				//��ʼ��LCD FSMC�ӿ� 
	SRAM_Init();						//��ʼ���ⲿSRAM  
	my_mem_init(SRAMIN);				//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);				//��ʼ���ⲿ�ڴ��
	
	exfuns_init();						//Ϊfatfs��ر��������ڴ�  
  	f_mount(fs[0],"0:",1); 				//����SD�� 
 	f_mount(fs[1],"1:",1); 				//����FLASH.
	while(font_init()) 					//����ֿ�
	{
UPD:    
		LCD_Clear(WHITE);		   		//����
 		POINT_COLOR=RED;				//��������Ϊ��ɫ	   	   	  
		LCD_ShowString(30,50,200,16,16,"WarShip STM32");
		while(SD_Init())				//���SD��
		{
			LCD_ShowString(30,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,70,200+30,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(30,70,200,16,16,"SD Card OK");
		LCD_ShowString(30,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16,"0:");//�����ֿ�
		while(key)//����ʧ��		
		{			 		  
			LCD_ShowString(30,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,110,200,16,16,"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(WHITE);//����	       
	}  
	POINT_COLOR=RED;       
	Show_Str(30,50,200,16,"ս��STM32F103������",16,0);				    	 
	Show_Str(30,70,200,16,"GBK�ֿ���Գ���",16,0);				    	 
	Show_Str(30,90,200,16,"����ԭ��@ALIENTEK",16,0);				    	 
	Show_Str(30,110,200,16,"2017��6��1��",16,0);
	Show_Str(30,130,200,16,"��KEY0,�����ֿ�",16,0);
 	POINT_COLOR=BLUE;  
	Show_Str(30,150,200,16,"������ֽ�:",16,0);				    	 
	Show_Str(30,170,200,16,"������ֽ�:",16,0);				    	 
	Show_Str(30,190,200,16,"���ּ�����:",16,0);

	Show_Str(30,220,200,24,"��Ӧ����Ϊ:",24,0); 
	Show_Str(30,244,200,16,"��Ӧ����(16*16)Ϊ:",16,0);			 
	Show_Str(30,260,200,12,"��Ӧ����(12*12)Ϊ:",12,0);			 
	while(1)
	{
		fontcnt=0;
		for(i=0x81;i<0xff;i++)
		{		
			fontx[0]=i;
			LCD_ShowNum(118,150,i,3,16);		//��ʾ������ֽ�    
			for(j=0x40;j<0xfe;j++)
			{
				if(j==0x7f)continue;
				fontcnt++;
				LCD_ShowNum(118,170,j,3,16);	//��ʾ������ֽ�	 
				LCD_ShowNum(118,190,fontcnt,5,16);//���ּ�����ʾ	 
			 	fontx[1]=j;
				Show_Font(30+132,220,fontx,24,0);	  
				Show_Font(30+144,244,fontx,16,0);	  		 		 
				Show_Font(30+108,260,fontx,12,0);	  		 		 
				t=200;
				while(t--)//��ʱ,ͬʱɨ�谴��
				{
					delay_ms(1);
					key=KEY_Scan(0);
					if(key==KEY0_PRES)goto UPD;
				}
				LED0=!LED0;
			}   
		}	
	}
}

