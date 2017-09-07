#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "can.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��26
 CANʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
	
int main(void)
{	
   	u8 key;
	u8 i=0,t=0;
	u8 cnt=0;
	u8 canbuf[8];
	u8 res;
	u8 mode=1; 
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������
	usmart_dev.init(84); 		   		//��ʼ��USMART	
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
 	LCD_Init();           				//��ʼ��LCD FSMC�ӿ�
    
 	CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_9TQ,4,CAN_MODE_LOOPBACK); //CAN��ʼ��,������500Kbps      
    POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"CAN TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/5/30"); 		
	LCD_ShowString(30,130,200,16,16,"LoopBack Mode");	 
	LCD_ShowString(30,150,200,16,16,"KEY0:Send WK_UP:Mode");//��ʾ��ʾ��Ϣ		
  	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	LCD_ShowString(30,170,200,16,16,"Count:");			//��ʾ��ǰ����ֵ	
	LCD_ShowString(30,190,200,16,16,"Send Data:");		//��ʾ���͵�����	
	LCD_ShowString(30,250,200,16,16,"Receive Data:");	//��ʾ���յ�������		
    while(1)
    {
        key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0����,����һ������
		{
			for(i=0;i<8;i++)
			{
				canbuf[i]=cnt+i;//��䷢�ͻ�����
				if(i<4)LCD_ShowxNum(30+i*32,210,canbuf[i],3,16,0X80);	//��ʾ����
				else LCD_ShowxNum(30+(i-4)*32,230,canbuf[i],3,16,0X80);	//��ʾ����
 			}
			res=CAN1_Send_Msg(canbuf,8);//����8���ֽ� 
			if(res)LCD_ShowString(30+80,190,200,16,16,"Failed");		//��ʾ����ʧ��
			else LCD_ShowString(30+80,190,200,16,16,"OK    ");	 		//��ʾ���ͳɹ�								   
		}else if(key==WKUP_PRES)//WK_UP���£��ı�CAN�Ĺ���ģʽ
		{	   
			mode=!mode;
            if(mode==0)  CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_9TQ,4,CAN_MODE_NORMAL);        //�ػ�ģʽ,������500Kbps
            else if(mode==1) CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_9TQ,4,CAN_MODE_LOOPBACK);  //�ػ�ģʽ,������500Kbps
  			POINT_COLOR=RED;//��������Ϊ��ɫ 
			if(mode==0)//��ͨģʽ����Ҫ2��������
			{
				LCD_ShowString(30,130,200,16,16,"Nnormal Mode ");	    
			}else //�ػ�ģʽ,һ��������Ϳ��Բ�����.
			{
 				LCD_ShowString(30,130,200,16,16,"LoopBack Mode");
			}
 			POINT_COLOR=BLUE;//��������Ϊ��ɫ 
		}		 
		key=CAN1_Receive_Msg(canbuf);
		if(key)//���յ�������
		{			
			LCD_Fill(30,270,160,310,WHITE);//���֮ǰ����ʾ
 			for(i=0;i<key;i++)
			{									    
				if(i<4)LCD_ShowxNum(30+i*32,270,canbuf[i],3,16,0X80);	//��ʾ����
				else LCD_ShowxNum(30+(i-4)*32,290,canbuf[i],3,16,0X80);	//��ʾ����
 			}
		}
		t++; 
		delay_ms(10);
		if(t==20)
		{
			LED0=!LED0;//��ʾϵͳ��������	
			t=0;
			cnt++;
			LCD_ShowxNum(30+48,170,cnt,3,16,0X80);	//��ʾ����
		}		   
	} 
}

