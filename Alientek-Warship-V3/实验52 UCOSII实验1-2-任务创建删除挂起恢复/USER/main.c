#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "includes.h"
/************************************************
 ALIENTEK ս��STM32F103������ ʵ��52
 UCOSIIʵ��1-2 ���񴴽���ɾ�������𣬻ָ� -�������-HAL��汾
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

//LED����
//�����������ȼ�
#define LED_TASK_PRIO			7
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

//����ɨ������
//�����������ȼ�
#define KEY_TASK_PRIO       	3
//���������ջ��С
#define KEY_STK_SIZE  	        128
//���������ջ�ռ�	
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//�������ӿ�
void key_task(void *pdata);

int main(void)
{ 
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	LED_Init();							//��ʼ��LED	
	KEY_Init();                    		//��ʼ������
	BEEP_Init();						//��ʼ��������
	
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

//��ʼ����
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //����ͳ������
	
	OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
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
	//BEEP����
    OSTaskCreateExt((void(*)(void*) )beep_task,                 
                    (void*          )0,
                    (OS_STK*        )&BEEP_TASK_STK[BEEP_STK_SIZE-1],
                    (INT8U          )BEEP_TASK_PRIO,            
                    (INT16U         )BEEP_TASK_PRIO,            
                    (OS_STK*        )&BEEP_TASK_STK[0],         
                    (INT32U         )BEEP_STK_SIZE,             
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
	while(1)
	{  
        LED0=!LED0;
        LED1=!LED1;
        delay_ms(500);
	}									 
}	   

//����������
void beep_task(void *pdata)
{
	while(1)
	{  	
        if(OSTaskDelReq(OS_PRIO_SELF)==OS_ERR_TASK_DEL_REQ) //�ж��Ƿ���ɾ������  
        {
            OSTaskDel(OS_PRIO_SELF);			            //ɾ��������TaskLed
        }
		BEEP=1;												//�򿪷�����
        delay_ms(60);
		BEEP=0;												//�رշ�����                       
        delay_ms(940);
	}									 
}

//����ɨ������
void key_task(void *pdata)
{	
	u8 key;		    						 
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
            OSTaskSuspend(LED_TASK_PRIO);   //����LED����LEDֹͣ��˸
		}
		else if (key==KEY2_PRES)
		{
            OSTaskResume(LED_TASK_PRIO);	//�ָ�LED����LED�ָ���˸
		}
		else if (key==WKUP_PRES)
		{
            OSTaskDelReq(BEEP_TASK_PRIO);	//����ɾ��BEEP������������˯�ߣ��޷��ָ�
		}
		else if(key==KEY1_PRES)
		{				   				   
            //���´�������beep	 
            OSTaskCreateExt((void(*)(void*) )beep_task,                 
                            (void*          )0,
                            (OS_STK*        )&BEEP_TASK_STK[BEEP_STK_SIZE-1],
                            (INT8U          )BEEP_TASK_PRIO,            
                            (INT16U         )BEEP_TASK_PRIO,            
                            (OS_STK*        )&BEEP_TASK_STK[0],         
                            (INT32U         )BEEP_STK_SIZE,             
                            (void*          )0,                         
                            (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
        }   
 		delay_ms(10);
	}
}
