#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sram.h"
#include "malloc.h" 
#include "sdio_sdcard.h"
#include "w25qxx.h"
#include "ov7670.h" 
#include "timer.h"
#include "exti.h"
#include "beep.h" 
#include "ff.h"
#include "exfuns.h"
#include "string.h"
#include "usmart.h"
#include "fontupd.h"
#include "text.h"	
#include "piclib.h"	
#include "string.h"		
#include "math.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��42
 �����ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/  
  
extern u8 ov_sta;	//��exit.c���涨��
extern u8 ov_frame;	//��timer.c���涨��	   
//����LCD��ʾ
void camera_refresh(void)
{
	u32 j;
 	u16 color;	 
	if(ov_sta)//��֡�жϸ��£�
	{
		LCD_Scan_Dir(U2D_L2R);		//���ϵ���,������  
		if(lcddev.id==0X1963)LCD_Set_Window((lcddev.width-240)/2,(lcddev.height-320)/2,240,320);//����ʾ�������õ���Ļ����
		else if(lcddev.id==0X5510||lcddev.id==0X5310)LCD_Set_Window((lcddev.width-320)/2,(lcddev.height-240)/2,320,240);//����ʾ�������õ���Ļ����
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
		OV7670_RRST=0;				//��ʼ��λ��ָ�� 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//��λ��ָ����� 
		OV7670_RCK_H;
		for(j=0;j<76800;j++)
		{
			OV7670_RCK_L;
			color=GPIOC->IDR&0XFF;	//������
			OV7670_RCK_H; 
			color<<=8;  
			OV7670_RCK_L;
			color|=GPIOC->IDR&0XFF;	//������
			OV7670_RCK_H; 
			LCD->LCD_RAM=color;    
		}   							  
 		ov_sta=0;					//����֡�жϱ��
		ov_frame++; 
		LCD_Scan_Dir(DFT_SCAN_DIR);	//�ָ�Ĭ��ɨ�跽�� 
	} 
}	   
//�ļ������������⸲�ǣ�
//��ϳ�:����"0:PHOTO/PIC13141.bmp"���ļ���
void camera_new_pathname(u8 *pname)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		sprintf((char*)pname,"0:PHOTO/PIC%05d.bmp",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//���Դ�����ļ�
		if(res==FR_NO_FILE)break;		//���ļ���������=����������Ҫ��.
		index++;
	}
}

int main(void)
{	
	u8 res;							 
	u8 *pname;							//��·�����ļ��� 
	u8 key;								//��ֵ		   
	u8 i;						 
	u8 sd_ok=1;							//0,sd��������;1,SD������.
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������	
	usmart_dev.init(72);				//��ʼ��USMART	
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
	BEEP_Init();						//��ʼ��������
 	LCD_Init();           				//��ʼ��LCD FSMC�ӿ� 
	SRAM_Init();						//��ʼ���ⲿSRAM  
	W25QXX_Init();						//��ʼ��W25Q128
	my_mem_init(SRAMIN);				//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);				//��ʼ���ⲿ�ڴ��
	exfuns_init();						//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 				//����SD�� 
 	f_mount(fs[1],"1:",1); 				//����FLASH.
	
	POINT_COLOR=RED;           
 	while(font_init()) 					//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
	}  	 
 	Show_Str(30,50,200,16,"ս��STM32F1������",16,0);				    	 
	Show_Str(30,70,200,16,"�����ʵ��",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:����",16,0);				    	 
	Show_Str(30,110,200,16,"����ԭ��@ALIENTEK",16,0);				    	 
	Show_Str(30,130,200,16,"2017��6��2��",16,0);
	res=f_mkdir("0:/PHOTO");		//����PHOTO�ļ���
	if(res!=FR_EXIST&&res!=FR_OK) 	//�����˴���
	{		    
		Show_Str(30,150,240,16,"SD������!",16,0);
		delay_ms(200);				  
		Show_Str(30,170,240,16,"���չ��ܽ�������!",16,0);
		sd_ok=0;  	
	}else
	{
		Show_Str(30,150,240,16,"SD������!",16,0);
		delay_ms(200);				  
		Show_Str(30,170,240,16,"KEY0:����",16,0);
		sd_ok=1;  	  
	}										   						    
 	pname=mymalloc(SRAMIN,30);	//Ϊ��·�����ļ�������30���ֽڵ��ڴ�		    
 	while(pname==NULL)			//�ڴ�������
 	{	    
		Show_Str(30,190,240,16,"�ڴ����ʧ��!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,190,240,146,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}   											  
	while(OV7670_Init())//��ʼ��OV7670
	{
		Show_Str(30,190,240,16,"OV7670 ����!",16,0);
		delay_ms(200);
	    LCD_Fill(30,190,239,206,WHITE);
		delay_ms(200);
	}
 	Show_Str(30,190,200,16,"OV7670 ����",16,0);
	delay_ms(1500);	 		 
	TIM6_Init(10000,7199);				//10Khz����Ƶ��,1�����ж�									  
	EXTI8_Init();						//ʹ�ܶ�ʱ������
	OV7670_Window_Set(12,176,240,320);	//���ô���	  
  	OV7670_CS=0;	
	LCD_Clear(BLACK);
 	while(1)
	{	
		key=KEY_Scan(0);//��֧������
		if(key==KEY0_PRES)
		{
			if(sd_ok)
			{
				LED1=0;	//����DS1,��ʾ��������
				camera_new_pathname(pname);//�õ��ļ���		    
				if(bmp_encode(pname,(lcddev.width-240)/2,(lcddev.height-320)/2,240,320,0))//��������
				{
					Show_Str(40,130,240,12,"д���ļ�����!",12,0);		 
				}else 
				{
					Show_Str(40,130,240,12,"���ճɹ�!",12,0);
					Show_Str(40,150,240,12,"����Ϊ:",12,0);
 					Show_Str(40+42,150,240,12,pname,12,0);		    
 					BEEP=1;	//�������̽У���ʾ�������
					delay_ms(100);
		 		}
			}else //��ʾSD������
			{					    
				Show_Str(40,130,240,12,"SD������!",12,0);
 				Show_Str(40,150,240,12,"���չ��ܲ�����!",12,0);			    
 			}
 		 	BEEP=0;//�رշ�����
			LED1=1;//�ر�DS1
			delay_ms(1800);//�ȴ�1.8����
			LCD_Clear(BLACK);
		}else delay_ms(5);
 		camera_refresh();//������ʾ
		i++;
		if(i==40)//DS0��˸.
		{
			i=0;
			LED0=!LED0;
 		}
	}	
}

