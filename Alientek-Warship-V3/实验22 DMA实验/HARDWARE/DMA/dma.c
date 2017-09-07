#include "dma.h"
#include "lcd.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103开发板
//DMA驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/5/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

DMA_HandleTypeDef  UART1TxDMA_Handler;      //DMA句柄

//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//chx:DMA通道选择,DMA1_Channel1~DMA1_Channel7
void MYDMA_Config(DMA_Channel_TypeDef *chx)
{ 
	__HAL_RCC_DMA1_CLK_ENABLE();			//DMA1时钟使能 

    __HAL_LINKDMA(&UART1_Handler,hdmatx,UART1TxDMA_Handler);    		//将DMA与USART1联系起来(发送DMA)
    
    //Tx DMA配置
    UART1TxDMA_Handler.Instance=chx;                            		//通道选择
    UART1TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;             //存储器到外设
    UART1TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设非增量模式
    UART1TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //存储器增量模式
    UART1TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //外设数据长度:8位
    UART1TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //存储器数据长度:8位
    UART1TxDMA_Handler.Init.Mode=DMA_NORMAL;                            //外设普通模式
    UART1TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //中等优先级
    
    HAL_DMA_DeInit(&UART1TxDMA_Handler);   
    HAL_DMA_Init(&UART1TxDMA_Handler);
} 


//开启一次DMA传输
//huart:串口句柄
//pData：传输的数据指针
//Size:传输的数据量
void MYDMA_USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    HAL_DMA_Start(huart->hdmatx, (u32)pData, (uint32_t)&huart->Instance->DR, Size);//开启DMA传输
    
    huart->Instance->CR3 |= USART_CR3_DMAT;//使能串口DMA发送
}	  

 
