#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��48
 USB���⴮��ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{
 	u16 t;
	u16 len;	
	u16 times=0;    
	u8 usbstatus=0;	
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������
	LED_Init();							//��ʼ��LED	
 	LCD_Init();           				//��ʼ��LCD FSMC�ӿ�
	
 	POINT_COLOR=RED;					//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"USB Virtual USART TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/6/5");  
 	LCD_ShowString(30,130,200,16,16,"USB Connecting...");//��ʾUSB��ʼ����
	delay_ms(1800);
	USB_Port_Set(0); 	//USB�ȶϿ�
	delay_ms(700);
	USB_Port_Set(1);	//USB�ٴ�����
 	Set_USBClock();   
 	USB_Interrupts_Config();    
 	USB_Init();	    
	while(1)
	{
		if(usbstatus!=bDeviceState)//USB����״̬�����˸ı�.
		{
			usbstatus=bDeviceState;//��¼�µ�״̬
			if(usbstatus==CONFIGURED)
			{
				POINT_COLOR=BLUE;
				LCD_ShowString(30,130,200,16,16,"USB Connected    ");//��ʾUSB���ӳɹ�
				LED1=0;//DS1��
			}else
			{
				POINT_COLOR=RED;
				LCD_ShowString(30,130,200,16,16,"USB disConnected ");//��ʾUSB�Ͽ�
				LED1=1;//DS1��
			}
		}
		if(USB_USART_RX_STA&0x8000)
		{					   
			len=USB_USART_RX_STA&0x3FFF;//�õ��˴ν��յ������ݳ���
			usb_printf("\r\n�����͵���ϢΪ:%d\r\n\r\n",len);
			for(t=0;t<len;t++)
			{
				USB_USART_SendData(USB_USART_RX_BUF[t]);//���ֽڷ�ʽ,���͸�USB 
			}
			usb_printf("\r\n\r\n");//���뻻��
			USB_USART_RX_STA=0;
		}else
		{
			times++;
			if(times%5000==0)
			{
				usb_printf("\r\nս��STM32������USB���⴮��ʵ��\r\n");
				usb_printf("����ԭ��@ALIENTEK\r\n\r\n");
			}
			if(times%200==0)usb_printf("����������,�Իس�������\r\n");  
			if(times%30==0)LED0=!LED0;//��˸LED,��ʾϵͳ��������.
			delay_ms(10);   
		}
	}
}

