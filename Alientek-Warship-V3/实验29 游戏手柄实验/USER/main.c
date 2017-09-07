#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "joypad.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��29
 ��Ϸ�ֱ�ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

const u8*JOYPAD_SYMBOL_TBL[8]={	"Right",	"Left",		"Down",		"Up",	
								"Start",	"Select",	"A",		"B"};//�ֱ��������Ŷ���
int main(void)
{	
	u8 key;
	u8 t=0,i=0;	
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������	
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
 	LCD_Init();           				//��ʼ��LCD FSMC�ӿ�
    
	JOYPAD_Init();						//�ֱ���ʼ��
 	POINT_COLOR=RED;					//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"JOYPAD TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/5/31");
	LCD_ShowString(30,130,200,16,16,"KEYVAL:");	      
	LCD_ShowString(30,150,200,16,16,"SYMBOL:");	      
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	while(1)
	{
		key=JOYPAD_Read();
		if(key)
		{
			LCD_ShowNum(116,130,key,3,16);//��ʾ��ֵ
			for(i=0;i<8;i++)
			{
				if(key&(0X80>>i))
				{
					LCD_Fill(30+56,150,30+56+48,150+16,WHITE);//���֮ǰ����ʾ
					LCD_ShowString(30+56,150,200,16,16,(u8*)JOYPAD_SYMBOL_TBL[i]);//��ʾ����
				}		
			}		    
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

