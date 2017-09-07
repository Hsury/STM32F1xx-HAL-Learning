#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "touch.h"
#include "lcd.h"
#include "includes.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��53
 UCOSII-�ź���������-HAL��汾
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//START ����
//�����������ȼ�
#define START_TASK_PRIO			10  //��ʼ��������ȼ�Ϊ���
//���������ջ��С
#define START_STK_SIZE			128
//���������ջ
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);

//����������
//�����������ȼ�
#define TOUCH_TASK_PRIO         7
//���������ջ��С
#define TOUCH_STK_SIZE  	    128
//�����ջ	
OS_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//������
void touch_task(void *pdata);

//LED����
//�����������ȼ�
#define LED_TASK_PRIO			6
//���������ջ��С
#define LED_STK_SIZE			128
//�����ջ
OS_STK LED_TASK_STK[LED_STK_SIZE];
//������
void led_task(void *pdata);

//����������
//�����������ȼ�
#define BEEP_TASK_PRIO          5 
//���������ջ��С
#define BEEP_STK_SIZE  	        128
//���������ջ�ռ�	
OS_STK BEEP_TASK_STK[BEEP_STK_SIZE];
//�������ӿ�
void beep_task(void *pdata);

//������
//�����������ȼ�
#define MAIN_TASK_PRIO          4 
//���������ջ��С
#define MAIN_STK_SIZE  	        128
//�����ջ	
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//������
void main_task(void *pdata);

//����ɨ������
//�����������ȼ�
#define KEY_TASK_PRIO       	3
//���������ջ��С
#define KEY_STK_SIZE  	        128
//���������ջ�ռ�	
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//�������ӿ�
void key_task(void *pdata);

//////////////////////////////////////////////////////////////////////////////
OS_EVENT * msg_key;			//���������¼���ָ��
OS_EVENT * sem_beep;		//�������ź���ָ��	 	  
//����������   
void ucos_load_main_ui(void)
{
	LCD_Clear(WHITE);	//����
 	POINT_COLOR=RED;	//��������Ϊ��ɫ 
	LCD_ShowString(30,10,200,16,16,"WarShip STM32");
	LCD_ShowString(30,30,200,16,16,"UCOSII TEST2");	
	LCD_ShowString(30,50,200,16,16,"ATOM@ALIENTEK");
   	LCD_ShowString(30,75,200,16,16,"KEY0:DS1 KEY_UP:ADJUST");	
   	LCD_ShowString(30,95,200,16,16,"KEY1:BEEP  KEY2:CLEAR"); 
	LCD_ShowString(80,210,200,16,16,"Touch Area");	
	LCD_DrawLine(0,120,lcddev.width-1,120);
	LCD_DrawLine(0,70,lcddev.width-1,70);
	LCD_DrawLine(170,0,170,70);
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
  	LCD_ShowString(180,30,200,16,16,"CPU:   %");	
   	LCD_ShowString(180,50,200,16,16,"SEM:000");	
}


int main(void)
{ 
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);              	//��ʼ��USART
	LED_Init();							//��ʼ��LED	
	KEY_Init();                    		//��ʼ������
	BEEP_Init();						//��ʼ��������
	LCD_Init();                     	//��ʼ��LCD
    tp_dev.init();				    	//��ʼ��������
    ucos_load_main_ui();		    	//����������	 
	
	OSInit();                       	//UCOS��ʼ��
    OSTaskCreateExt((void(*)(void*) )start_task,                //������
                    (void*          )0,                         //���ݸ��������Ĳ���
                    (OS_STK*        )&START_TASK_STK[START_STK_SIZE-1],//�����ջջ��
                    (INT8U          )START_TASK_PRIO,           //�������ȼ�
                    (INT16U         )START_TASK_PRIO,           //����ID����������Ϊ�����ȼ�һ��
                    (OS_STK*        )&START_TASK_STK[0],        //�����ջջ��
                    (INT32U         )START_STK_SIZE,            //�����ջ��С
                    (void*          )0,                         //�û�����Ĵ洢��
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);//����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ
	OSStart(); //��ʼ����
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//��ˮƽ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//��ʵ��Բ
//x0,y0:����
//r:�뾶
//color:��ɫ
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//������֮��ľ���ֵ 
//x1,x2����ȡ��ֵ��������
//����ֵ��|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//��һ������
//(x1,y1),(x2,y2):��������ʼ����
//size�������Ĵ�ϸ�̶�
//color����������ɫ
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		gui_fill_circle(uRow,uCol,size,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//��ʼ����
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
    msg_key=OSMboxCreate((void*)0);	//������Ϣ����
	sem_beep=OSSemCreate(0);		//�����ź���	
	OSStatInit();  //����ͳ������
	OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
    //��������
    OSTaskCreateExt((void(*)(void*) )touch_task,                 
                    (void*          )0,
                    (OS_STK*        )&TOUCH_TASK_STK[TOUCH_STK_SIZE-1],
                    (INT8U          )TOUCH_TASK_PRIO,            
                    (INT16U         )TOUCH_TASK_PRIO,            
                    (OS_STK*        )&TOUCH_TASK_STK[0],         
                    (INT32U         )TOUCH_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
	//LED����
    OSTaskCreateExt((void(*)(void*) )led_task,                 
                    (void*          )0,
                    (OS_STK*        )&LED_TASK_STK[LED_STK_SIZE-1],
                    (INT8U          )LED_TASK_PRIO,            
                    (INT16U         )LED_TASK_PRIO,            
                    (OS_STK*        )&LED_TASK_STK[0],         
                    (INT32U         )LED_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
	//����������
    OSTaskCreateExt((void(*)(void*) )beep_task,                 
                    (void*          )0,
                    (OS_STK*        )&BEEP_TASK_STK[BEEP_STK_SIZE-1],
                    (INT8U          )BEEP_TASK_PRIO,          
                    (INT16U         )BEEP_TASK_PRIO,            
                    (OS_STK*        )&BEEP_TASK_STK[0],         
                    (INT32U         )BEEP_STK_SIZE,            
                    (void*          )0,                           
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
    //������
    OSTaskCreateExt((void(*)(void*) )main_task,                 
                    (void*          )0,
                    (OS_STK*        )&MAIN_TASK_STK[MAIN_STK_SIZE-1],
                    (INT8U          )MAIN_TASK_PRIO,          
                    (INT16U         )MAIN_TASK_PRIO,            
                    (OS_STK*        )&MAIN_TASK_STK[0],         
                    (INT32U         )MAIN_STK_SIZE,            
                    (void*          )0,                           
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
    //��������
    OSTaskCreateExt((void(*)(void*) )key_task,                 
                    (void*          )0,
                    (OS_STK*        )&KEY_TASK_STK[KEY_STK_SIZE-1],
                    (INT8U          )KEY_TASK_PRIO,          
                    (INT16U         )KEY_TASK_PRIO,            
                    (OS_STK*        )&KEY_TASK_STK[0],         
                    (INT32U         )KEY_STK_SIZE,            
                    (void*          )0,                           
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
    OS_EXIT_CRITICAL();             //�˳��ٽ���(���ж�)
	OSTaskSuspend(START_TASK_PRIO); //����ʼ����
}
 
//LED����
void led_task(void *pdata)
{
	u8 t;
	while(1)
	{
		t++;
		delay_ms(10);
		if(t==8)LED0=1;	//LED0��
		if(t==100)		//LED0��
		{
			t=0;
			LED0=0;
		}
	}									 
}   

//����������
void beep_task(void *pdata)
{
    u8 err;
	while(1)
	{  
        OSSemPend(sem_beep,0,&err);     //�����ź���  
		BEEP=1;							//�򿪷�����    
        delay_ms(60);
		BEEP=0;							//�رշ�����   
        delay_ms(940);
	}									 
}

//����������
void touch_task(void *pdata)
{	  	
	u32 cpu_sr;
 	u16 lastpos[2];		//���һ�ε����� 
	while(1)
	{
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)		//������������
		{	
		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height&&tp_dev.y[0]>120)
			{			
				if(lastpos[0]==0XFFFF)
				{
					lastpos[0]=tp_dev.x[0];
					lastpos[1]=tp_dev.y[0]; 
				}
				OS_ENTER_CRITICAL();//�����ٽ��,��ֹ��������,���LCD����,����Һ������.
				lcd_draw_bline(lastpos[0],lastpos[1],tp_dev.x[0],tp_dev.y[0],2,RED);//����
				OS_EXIT_CRITICAL();
				lastpos[0]=tp_dev.x[0];
				lastpos[1]=tp_dev.y[0];     
			}
		}else 
		{
			lastpos[0]=0XFFFF;
			delay_ms(10);	//û�а������µ�ʱ��
		}
	}
}

//������
void main_task(void *pdata)
{							 
	u32 key=0;	
	u8 err;	
	u8 semmask=0;
	u8 tcnt=0;						 
	while(1)
	{
		key=(u32)OSMboxPend(msg_key,10,&err);   
		switch(key)
		{
			case 1://����DS1
				LED1=!LED1;
				break;
			case 2://�����ź���
				semmask=1;
				OSSemPost(sem_beep);
				break;
			case 3://���
				LCD_Fill(0,121,lcddev.width-1,lcddev.height-1,WHITE);
				break;
			case 4://У׼
				OSTaskSuspend(TOUCH_TASK_PRIO);	//������������		 
 				if((tp_dev.touchtype&0X80)==0)TP_Adjust();   
 				OSTaskResume(TOUCH_TASK_PRIO);	//���
				ucos_load_main_ui();			//���¼���������		 
				break;
		}
   		if(semmask||sem_beep->OSEventCnt)//��Ҫ��ʾsem		
		{
			POINT_COLOR=BLUE;
			LCD_ShowxNum(212,50,sem_beep->OSEventCnt,3,16,0X80);//��ʾ�ź�����ֵ
			if(sem_beep->OSEventCnt==0)semmask=0;	//ֹͣ����
		} 
		if(tcnt==50)//0.5�����һ��CPUʹ����
		{
			tcnt=0;
			POINT_COLOR=BLUE;		  
			LCD_ShowxNum(212,30,OSCPUUsage,3,16,0);	//��ʾCPUʹ����   
		}
		tcnt++;
		delay_ms(10);
	}
} 
//����ɨ������
void key_task(void *pdata)
{	
	u8 key;		    						 
	while(1)
	{
		key=KEY_Scan(0);   
		if(key)OSMboxPost(msg_key,(void*)key);//������Ϣ
 		delay_ms(10);
	}
}
