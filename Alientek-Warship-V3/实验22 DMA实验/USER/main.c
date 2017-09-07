#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "dma.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��22
 DMAʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

#define SEND_BUF_SIZE 8200	//�������ݳ���,��õ���sizeof(TEXT_TO_SEND)+2��������.

u8 SendBuff[SEND_BUF_SIZE];	//�������ݻ�����
const u8 TEXT_TO_SEND[]={"ALIENTEK WarShip STM32F1 DMA ����ʵ��"};

int main(void)
{	
	u16 i;
	u8 t=0;
	u8 j,mask=0;
	float pro=0;
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������
	usmart_dev.init(84); 		   		//��ʼ��USMART	
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
 	LCD_Init();           				//��ʼ��LCD FSMC�ӿ�
    
	MYDMA_Config(DMA1_Channel4);		//��ʼ��DMA1ͨ��4
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"DMA TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/5/29");	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Start"); 
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
    //��ʾ��ʾ��Ϣ	
	j=sizeof(TEXT_TO_SEND);	   
	for(i=0;i<SEND_BUF_SIZE;i++)//���ASCII�ַ�������
    {
		if(t>=j)//���뻻�з�
		{
			if(mask)
			{
				SendBuff[i]=0x0a;
				t=0;
			}else 
			{
				SendBuff[i]=0x0d;
				mask++;
			}	
		}else//����TEXT_TO_SEND���
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		}   	   
    }   
	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	i=0;
       while(1)
	{
        t=KEY_Scan(0);
		if(t==KEY0_PRES)  //KEY0����
		{
			printf("\r\nDMA DATA:\r\n"); 	    
			LCD_ShowString(30,150,200,16,16,"Start Transimit....");
			LCD_ShowString(30,170,200,16,16,"   %") ;     //��ʾ�ٷֺ�      
		    
			HAL_UART_Transmit_DMA(&UART1_Handler,SendBuff,SEND_BUF_SIZE);//��������
			//ʹ�ܴ���1��DMA���� //�ȴ�DMA������ɣ���ʱ������������һЩ�£����
		    //ʵ��Ӧ���У����������ڼ䣬����ִ�����������
		    while(1)
		    {
                if(__HAL_DMA_GET_FLAG(&UART1TxDMA_Handler,DMA_FLAG_TC4))//�ȴ�DMA1ͨ��4�������
                {
                    __HAL_DMA_CLEAR_FLAG(&UART1TxDMA_Handler,DMA_FLAG_TC4);//���DMA1ͨ��4������ɱ�־
                    HAL_UART_DMAStop(&UART1_Handler);      //��������Ժ�رմ���DMA
					break; 
                }
				pro=__HAL_DMA_GET_COUNTER(&UART1TxDMA_Handler);//�õ���ǰ��ʣ����ٸ�����
				pro=1-pro/SEND_BUF_SIZE;    //�õ��ٷֱ�	  
				pro*=100;      			    //����100��
				LCD_ShowNum(30,170,pro,3,16);	    
		    }
			LCD_ShowNum(30,170,100,3,16);//��ʾ100%	  
		    LCD_ShowString(30,150,200,16,16,"Transimit Finished!");//��ʾ�������
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0=!LED0;//��ʾϵͳ��������	
			i=0;
		}		   
	}
}

