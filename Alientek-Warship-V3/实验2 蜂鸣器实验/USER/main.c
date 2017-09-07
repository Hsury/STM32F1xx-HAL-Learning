#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��2
 ������ʵ��-HAL��汾
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
	LED_Init();							//��ʼ��LED	
	BEEP_Init();						//��ʼ��������
	
	while(1)
	{
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); 	//DS0���ͣ���   ��ͬLED0=0;
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);   	//BEEP�������ͣ� ��ͬBEEP=0;
        delay_ms(300);											//��ʱ300ms
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);   	//DS0���ߣ���   ��ͬLED0=1;
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); 	//BEEP�������ߣ� ��ͬBEEP=1;
        delay_ms(300);                                      	//��ʱ300ms 
	}
}
