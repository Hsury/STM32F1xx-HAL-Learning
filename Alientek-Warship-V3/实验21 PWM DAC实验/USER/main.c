#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "adc.h"
#include "pwmdac.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��21
 PWM DACʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//���������ѹ
//vol:0~330,����0~3.3V
void PWM_DAC_Set(u16 vol)
{
	double temp=vol;
	temp/=100;
	temp=temp*256/3.3;
	TIM_SetTIM1Compare1(temp);	   
}

int main(void)
{	
	u16 adcx;
	float temp;
	u8 t=0;	 
	u16 pwmval=0;
	u8 key;
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������
	usmart_dev.init(84); 		   		//��ʼ��USMART	
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
 	LCD_Init();           				//��ʼ��LCD FSMC�ӿ�
	MY_ADC_Init();                  	//��ʼ��ADC1ͨ��1
    TIM1_CH1_PWM_Init(255,1);       	//TIM1 PWM��ʼ��, Fpwm=72M/256=281.25Khz.
	
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"PWM DAC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/5/29");	 
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	  
	POINT_COLOR=BLUE;//��������Ϊ��ɫ      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V"); 	
    
	TIM_SetTIM1Compare1(pwmval);	//��ʼֵΪ0 	
    while(1)
	{
		t++;
		key=KEY_Scan(0);			  
		if(key==WKUP_PRES)
		{		 
			if(pwmval<250)pwmval+=10;
            TIM_SetTIM1Compare1(pwmval);	//���		
		}else if(key==KEY1_PRES)	
		{
			if(pwmval>10)pwmval-=10;
			else pwmval=0;
            TIM_SetTIM1Compare1(pwmval);	//���	
		}	 
		if(t==10||key==KEY1_PRES||key==WKUP_PRES) 	//WKUP/KEY1������,���߶�ʱʱ�䵽��
		{	  
			adcx=HAL_TIM_ReadCapturedValue(&TIM1_Handler,TIM_CHANNEL_1);
			LCD_ShowxNum(94,150,adcx,3,16,0);     	//��ʾDAC�Ĵ���ֵ
			temp=(float)adcx*(3.3/256);;			//�õ�DAC��ѹֵ
			adcx=temp;
			LCD_ShowxNum(94,170,temp,1,16,0);     	//��ʾ��ѹֵ��������
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0x80); 	//��ʾ��ѹֵ��С������
			adcx=Get_Adc_Average(ADC_CHANNEL_1,20); //�õ�ADCת��ֵ	  
			temp=(float)adcx*(3.3/4096);			//�õ�ADC��ѹֵ
			adcx=temp;
			LCD_ShowxNum(94,190,temp,1,16,0);     	//��ʾ��ѹֵ��������
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0x80); 	//��ʾ��ѹֵ��С������
			t=0;
			LED0=!LED0;	   
		 }	    
		delay_ms(10); 
	}
}

