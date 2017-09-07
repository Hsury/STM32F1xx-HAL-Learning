#include "can.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103开发板
//CAN驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/5/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									////////////////////////////////////////////////
  
////////////////  //////////////////
CAN_HandleTypeDef   CAN1_Handler;   //CAN1句柄
CanTxMsgTypeDef     TxMessage;      //发送消息
CanRxMsgTypeDef     RxMessage;      //接收消息

////CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1TQ~CAN_SJW_4TQ
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1TQ~CAN_BS2_8TQ;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1TQ~CAN_BS1_16TQ
//brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+tbs2+1)*brp); 其中tbs1和tbs2我们只用关注标识符上标志的序号，例如CAN_BS2_1TQ，我们就认为tbs2=1来计算即可。
//mode:CAN_MODE_NORMAL,普通模式;CAN_MODE_LOOPBACK,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_MODE_LOOPBACK);
//则波特率为:36M/((8+9+1)*4)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败; 
u8 CAN1_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode)
{
    CAN_FilterConfTypeDef  CAN1_FilerConf;
    
    CAN1_Handler.Instance=CAN1; 
    CAN1_Handler.pTxMsg=&TxMessage;     //发送消息
    CAN1_Handler.pRxMsg=&RxMessage;     //接收消息
    CAN1_Handler.Init.Prescaler=brp;    //分频系数(Fdiv)为brp+1
    CAN1_Handler.Init.Mode=mode;        //模式设置 
    CAN1_Handler.Init.SJW=tsjw;         //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1TQ~CAN_SJW_4TQ
    CAN1_Handler.Init.BS1=tbs1;         //tbs1范围CAN_BS1_1TQ~CAN_BS1_16TQ
    CAN1_Handler.Init.BS2=tbs2;         //tbs2范围CAN_BS2_1TQ~CAN_BS2_8TQ
    CAN1_Handler.Init.TTCM=DISABLE;     //非时间触发通信模式 
    CAN1_Handler.Init.ABOM=DISABLE;     //软件自动离线管理
    CAN1_Handler.Init.AWUM=DISABLE;     //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN1_Handler.Init.NART=ENABLE;      //禁止报文自动传送 
    CAN1_Handler.Init.RFLM=DISABLE;     //报文不锁定,新的覆盖旧的 
    CAN1_Handler.Init.TXFP=DISABLE;     //优先级由报文标识符决定 
	
    if(HAL_CAN_Init(&CAN1_Handler)!=HAL_OK) return 1;   //初始化
    
    CAN1_FilerConf.FilterIdHigh=0X0000;     //32位ID
    CAN1_FilerConf.FilterIdLow=0X0000;
    CAN1_FilerConf.FilterMaskIdHigh=0X0000; //32位MASK
    CAN1_FilerConf.FilterMaskIdLow=0X0000;  
    CAN1_FilerConf.FilterFIFOAssignment=CAN_FILTER_FIFO0;//过滤器0关联到FIFO0
    CAN1_FilerConf.FilterNumber=0;          //过滤器0
    CAN1_FilerConf.FilterMode=CAN_FILTERMODE_IDMASK;
    CAN1_FilerConf.FilterScale=CAN_FILTERSCALE_32BIT;
    CAN1_FilerConf.FilterActivation=ENABLE; //激活滤波器0
    CAN1_FilerConf.BankNumber=14;
	
    if(HAL_CAN_ConfigFilter(&CAN1_Handler,&CAN1_FilerConf)!=HAL_OK) return 2;//滤波器初始化

    return 0;
}

//CAN底层驱动，引脚配置，时钟配置，中断配置
//此函数会被HAL_CAN_Init()调用
//hcan:CAN句柄
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_CAN1_CLK_ENABLE();                //使能CAN1时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();			    //开启GPIOA时钟
	
    GPIO_Initure.Pin=GPIO_PIN_12;   			//PA12
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //推挽复用
    GPIO_Initure.Pull=GPIO_PULLUP;              //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //初始化
	
	GPIO_Initure.Pin=GPIO_PIN_11;   			//PA11
	GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;     	//推挽复用
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //初始化
    
#if CAN1_RX0_INT_ENABLE
    __HAL_CAN_ENABLE_IT(&CAN1_Handler,CAN_IT_FMP0);	//FIFO0消息挂起中断允许.	  
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn,1,2);    	//抢占优先级1，子优先级2
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);          	//使能中断
#endif	
}

#if CAN1_RX0_INT_ENABLE                         	//使能RX0中断
//CAN中断服务函数
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&CAN1_Handler);//此函数会调用CAN_Receive_IT()接收数据
}

//CAN中断处理过程
//此函数会被CAN_Receive_IT()调用
//hcan:CAN句柄
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    int i=0;

    __HAL_CAN_ENABLE_IT(&CAN1_Handler,CAN_IT_FMP0);//重新开启FIF00消息挂号中断
    printf("id:%d\r\n",CAN1_Handler.pRxMsg->StdId);
    printf("ide:%d\r\n",CAN1_Handler.pRxMsg->IDE);
    printf("rtr:%d\r\n",CAN1_Handler.pRxMsg->RTR);
    printf("len:%d\r\n",CAN1_Handler.pRxMsg->DLC);
    for(i=0;i<8;i++)
    printf("rxbuf[%d]:%d\r\n",i,CAN1_Handler.pRxMsg->Data[i]);
}
#endif	

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 CAN1_Send_Msg(u8* msg,u8 len)
{	
    u16 i=0;
    CAN1_Handler.pTxMsg->StdId=0X12;        //标准标识符
    CAN1_Handler.pTxMsg->ExtId=0x12;        //扩展标识符(29位)
    CAN1_Handler.pTxMsg->IDE=CAN_ID_STD;    //使用标准帧
    CAN1_Handler.pTxMsg->RTR=CAN_RTR_DATA;  //数据帧
    CAN1_Handler.pTxMsg->DLC=len;                
    for(i=0;i<len;i++)
    CAN1_Handler.pTxMsg->Data[i]=msg[i];
    if(HAL_CAN_Transmit(&CAN1_Handler,10)!=HAL_OK) return 1;     //发送
    return 0;		
}

//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
    if(HAL_CAN_Receive(&CAN1_Handler,CAN_FIFO0,0)!=HAL_OK) return 0;//接收数据，超时时间设置为0	
    for(i=0;i<CAN1_Handler.pRxMsg->DLC;i++)
    buf[i]=CAN1_Handler.pRxMsg->Data[i];
	return CAN1_Handler.pRxMsg->DLC;	
}
