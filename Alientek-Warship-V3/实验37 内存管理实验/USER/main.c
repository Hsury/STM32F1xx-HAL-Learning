#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sram.h"
#include "malloc.h" 
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��37
 �ڴ����ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/  

int main(void)
{	
	u8 key;		 
 	u8 i=0;	    
	u8 *p=0;
	u8 *tp=0;
	u8 paddr[18];						//���P Addr:+p��ַ��ASCIIֵ
	u8 sramx=0;							//Ĭ��Ϊ�ڲ�sram
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������	
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
 	LCD_Init();           				//��ʼ��LCD FSMC�ӿ� 
	SRAM_Init();						//��ʼ���ⲿSRAM  
	my_mem_init(SRAMIN);				//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);				//��ʼ���ⲿ�ڴ��
	
	POINT_COLOR=RED;					//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"MALLOC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/6/1"); 
	LCD_ShowString(30,130,200,16,16,"KEY0:Malloc  KEY2:Free");
	LCD_ShowString(30,150,200,16,16,"KEY_UP:SRAMx KEY1:Write");

 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(30,170,200,16,16,"SRAMIN");
	LCD_ShowString(30,190,200,16,16,"SRAMIN USED:   %");
	LCD_ShowString(30,210,200,16,16,"SRAMEX USED:   %");

  	while(1)
	{	
		key=KEY_Scan(0);		//��֧������	
		switch(key)
		{
			case 0:				//û�а�������	
				break;
			case KEY0_PRES:		//KEY0����
				p=mymalloc(sramx,2048);//����2K�ֽ�
				if(p!=NULL)sprintf((char*)p,"Memory Malloc Test%03d",i);//��pд��һЩ����
				break;
			case KEY1_PRES:		//KEY1����	   
				if(p!=NULL)
				{
					sprintf((char*)p,"Memory Malloc Test%03d",i);//������ʾ���� 	 
					LCD_ShowString(30,250,200,16,16,p);			 //��ʾP������
				}
				break;
			case KEY2_PRES:		//KEY2����	  
				myfree(sramx,p);//�ͷ��ڴ�
				p=0;			//ָ��յ�ַ
				break;
			case WKUP_PRES:		//KEY UP���� 
 				sramx=!sramx;	//�л���ǰmalloc/free��������
				if(sramx)LCD_ShowString(30,170,200,16,16,"SRAMEX");
				else LCD_ShowString(30,170,200,16,16,"SRAMIN");
				break;
		}
		if(tp!=p)
		{
			tp=p;
			sprintf((char*)paddr,"P Addr:0X%08X",(u32)tp);
			LCD_ShowString(30,230,200,16,16,paddr);	//��ʾp�ĵ�ַ
			if(p)LCD_ShowString(30,250,200,16,16,p);//��ʾP������
		    else LCD_Fill(30,250,239,266,WHITE);	//p=0,�����ʾ
		}
		delay_ms(10);   
		i++;
		if((i%20)==0)//DS0��˸.
		{ 
			LCD_ShowNum(30+96,190,my_mem_perused(SRAMIN),3,16);//��ʾ�ڲ��ڴ�ʹ����
			LCD_ShowNum(30+96,210,my_mem_perused(SRAMEX),3,16);//��ʾ�ⲿ�ڴ�ʹ����
 			LED0=!LED0;
 		}
	}
}

