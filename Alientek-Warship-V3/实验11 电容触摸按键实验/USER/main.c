#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "tpad.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��11
 ���벶��ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{
	u8 t=0; 
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������
	LED_Init();							//��ʼ��LED	
	TPAD_Init(6);                   	//��ʼ����������
	
	while(1)
    {
 		if(TPAD_Scan(0))				//�ɹ�������һ��������(�˺���ִ��ʱ������15ms)
		{
			LED1=!LED1;					//LED1ȡ��
		}
		t++;
		if(t==15)		 
		{
			t=0;
			LED0=!LED0;					//LED0ȡ��,��ʾ������������
		}
		delay_ms(10);
    }
}

