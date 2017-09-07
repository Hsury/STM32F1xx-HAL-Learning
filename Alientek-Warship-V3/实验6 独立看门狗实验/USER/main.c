#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "iwdg.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��6
 �������Ź�ʵ��-HAL�⺯����
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
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
	delay_ms(100);                  	//��ʱ100ms�ٳ�ʼ�����Ź�,LED0�ı仯"�ɼ�"
    IWDG_Init(IWDG_PRESCALER_64,500);  	//��Ƶ��Ϊ64,����ֵΪ500,���ʱ��Ϊ1s	
    LED0=0;
	
    while(1)
    {
        if(KEY_Scan(0)==WKUP_PRES)  	//���WK_UP���£�ι��
        {
            IWDG_Feed();    			//ι��
        }
        delay_ms(10); 
	}
}
