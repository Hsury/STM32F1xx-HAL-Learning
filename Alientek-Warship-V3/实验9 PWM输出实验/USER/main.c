#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "timer.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��9
 PWM���ʵ��-HAL�⺯���� 
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{
	u8 dir=1;
    u16 led0pwmval=0; 
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
    TIM3_PWM_Init(500-1,72-1);       	//72M/72=1M�ļ���Ƶ�ʣ��Զ���װ��Ϊ500����ôPWMƵ��Ϊ1M/500=2kHZ
    while(1)
    {
		delay_ms(10);	 	
		if(dir)led0pwmval++;				//dir==1 led0pwmval����
		else led0pwmval--;					//dir==0 led0pwmval�ݼ� 
		if(led0pwmval>300)dir=0;			//led0pwmval����300�󣬷���Ϊ�ݼ�
		if(led0pwmval==0)dir=1;				//led0pwmval�ݼ���0�󣬷����Ϊ����
		TIM_SetTIM3Compare2(led0pwmval);	//�޸ıȽ�ֵ���޸�ռ�ձ�
    }
}
