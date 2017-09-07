#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��3
 ��������ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{
	u8 key;
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
	
    while(1)
    {
        key=KEY_Scan(0);            //����ɨ��
		switch(key)
		{				 
			case WKUP_PRES:			//����LED0,LED1�������
				LED1=!LED1;
				LED0=!LED1;
				break;
			case KEY2_PRES:			//����LED0��ת
				LED0=!LED0;
				break;
			case KEY1_PRES:			//����LED1��ת	 
				LED1=!LED1;
				break;
			case KEY0_PRES:			//ͬʱ����LED0,LED1��ת 
				LED0=!LED0;
				LED1=!LED1;
				break;
		}
        delay_ms(10);
	}
}
