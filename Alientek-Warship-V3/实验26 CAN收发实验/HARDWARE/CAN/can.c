#include "can.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//CAN��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/5/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									////////////////////////////////////////////////
  
////////////////  //////////////////
CAN_HandleTypeDef   CAN1_Handler;   //CAN1���
CanTxMsgTypeDef     TxMessage;      //������Ϣ
CanRxMsgTypeDef     RxMessage;      //������Ϣ

////CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1TQ~CAN_SJW_4TQ
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1TQ~CAN_BS2_8TQ;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1TQ~CAN_BS1_16TQ
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+tbs2+1)*brp); ����tbs1��tbs2����ֻ�ù�ע��ʶ���ϱ�־����ţ�����CAN_BS2_1TQ�����Ǿ���Ϊtbs2=1�����㼴�ɡ�
//mode:CAN_MODE_NORMAL,��ͨģʽ;CAN_MODE_LOOPBACK,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_MODE_LOOPBACK);
//������Ϊ:36M/((8+9+1)*4)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 
u8 CAN1_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode)
{
    CAN_FilterConfTypeDef  CAN1_FilerConf;
    
    CAN1_Handler.Instance=CAN1; 
    CAN1_Handler.pTxMsg=&TxMessage;     //������Ϣ
    CAN1_Handler.pRxMsg=&RxMessage;     //������Ϣ
    CAN1_Handler.Init.Prescaler=brp;    //��Ƶϵ��(Fdiv)Ϊbrp+1
    CAN1_Handler.Init.Mode=mode;        //ģʽ���� 
    CAN1_Handler.Init.SJW=tsjw;         //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1TQ~CAN_SJW_4TQ
    CAN1_Handler.Init.BS1=tbs1;         //tbs1��ΧCAN_BS1_1TQ~CAN_BS1_16TQ
    CAN1_Handler.Init.BS2=tbs2;         //tbs2��ΧCAN_BS2_1TQ~CAN_BS2_8TQ
    CAN1_Handler.Init.TTCM=DISABLE;     //��ʱ�䴥��ͨ��ģʽ 
    CAN1_Handler.Init.ABOM=DISABLE;     //����Զ����߹���
    CAN1_Handler.Init.AWUM=DISABLE;     //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
    CAN1_Handler.Init.NART=ENABLE;      //��ֹ�����Զ����� 
    CAN1_Handler.Init.RFLM=DISABLE;     //���Ĳ�����,�µĸ��Ǿɵ� 
    CAN1_Handler.Init.TXFP=DISABLE;     //���ȼ��ɱ��ı�ʶ������ 
	
    if(HAL_CAN_Init(&CAN1_Handler)!=HAL_OK) return 1;   //��ʼ��
    
    CAN1_FilerConf.FilterIdHigh=0X0000;     //32λID
    CAN1_FilerConf.FilterIdLow=0X0000;
    CAN1_FilerConf.FilterMaskIdHigh=0X0000; //32λMASK
    CAN1_FilerConf.FilterMaskIdLow=0X0000;  
    CAN1_FilerConf.FilterFIFOAssignment=CAN_FILTER_FIFO0;//������0������FIFO0
    CAN1_FilerConf.FilterNumber=0;          //������0
    CAN1_FilerConf.FilterMode=CAN_FILTERMODE_IDMASK;
    CAN1_FilerConf.FilterScale=CAN_FILTERSCALE_32BIT;
    CAN1_FilerConf.FilterActivation=ENABLE; //�����˲���0
    CAN1_FilerConf.BankNumber=14;
	
    if(HAL_CAN_ConfigFilter(&CAN1_Handler,&CAN1_FilerConf)!=HAL_OK) return 2;//�˲�����ʼ��

    return 0;
}

//CAN�ײ��������������ã�ʱ�����ã��ж�����
//�˺����ᱻHAL_CAN_Init()����
//hcan:CAN���
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_CAN1_CLK_ENABLE();                //ʹ��CAN1ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			    //����GPIOAʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_12;   			//PA12
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //���츴��
    GPIO_Initure.Pull=GPIO_PULLUP;              //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //��ʼ��
	
	GPIO_Initure.Pin=GPIO_PIN_11;   			//PA11
	GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;     	//���츴��
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //��ʼ��
    
#if CAN1_RX0_INT_ENABLE
    __HAL_CAN_ENABLE_IT(&CAN1_Handler,CAN_IT_FMP0);	//FIFO0��Ϣ�����ж�����.	  
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn,1,2);    	//��ռ���ȼ�1�������ȼ�2
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);          	//ʹ���ж�
#endif	
}

#if CAN1_RX0_INT_ENABLE                         	//ʹ��RX0�ж�
//CAN�жϷ�����
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&CAN1_Handler);//�˺��������CAN_Receive_IT()��������
}

//CAN�жϴ������
//�˺����ᱻCAN_Receive_IT()����
//hcan:CAN���
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    int i=0;

    __HAL_CAN_ENABLE_IT(&CAN1_Handler,CAN_IT_FMP0);//���¿���FIF00��Ϣ�Һ��ж�
    printf("id:%d\r\n",CAN1_Handler.pRxMsg->StdId);
    printf("ide:%d\r\n",CAN1_Handler.pRxMsg->IDE);
    printf("rtr:%d\r\n",CAN1_Handler.pRxMsg->RTR);
    printf("len:%d\r\n",CAN1_Handler.pRxMsg->DLC);
    for(i=0;i<8;i++)
    printf("rxbuf[%d]:%d\r\n",i,CAN1_Handler.pRxMsg->Data[i]);
}
#endif	

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN1_Send_Msg(u8* msg,u8 len)
{	
    u16 i=0;
    CAN1_Handler.pTxMsg->StdId=0X12;        //��׼��ʶ��
    CAN1_Handler.pTxMsg->ExtId=0x12;        //��չ��ʶ��(29λ)
    CAN1_Handler.pTxMsg->IDE=CAN_ID_STD;    //ʹ�ñ�׼֡
    CAN1_Handler.pTxMsg->RTR=CAN_RTR_DATA;  //����֡
    CAN1_Handler.pTxMsg->DLC=len;                
    for(i=0;i<len;i++)
    CAN1_Handler.pTxMsg->Data[i]=msg[i];
    if(HAL_CAN_Transmit(&CAN1_Handler,10)!=HAL_OK) return 1;     //����
    return 0;		
}

//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
    if(HAL_CAN_Receive(&CAN1_Handler,CAN_FIFO0,0)!=HAL_OK) return 0;//�������ݣ���ʱʱ������Ϊ0	
    for(i=0;i<CAN1_Handler.pRxMsg->DLC;i++)
    buf[i]=CAN1_Handler.pRxMsg->Data[i];
	return CAN1_Handler.pRxMsg->DLC;	
}
