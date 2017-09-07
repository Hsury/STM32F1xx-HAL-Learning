#include "sdio_sdcard.h"
#include "string.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//SD����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/1
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

SD_HandleTypeDef        SDCARD_Handler;     //SD�����
HAL_SD_CardInfoTypedef  SDCardInfo;         //SD����Ϣ�ṹ��
DMA_HandleTypeDef SDTxDMAHandler,SDRxDMAHandler;    //SD��DMA���ͺͽ��վ��

//SD_ReadDisk/SD_WriteDisk����ר��buf,�����������������ݻ�������ַ����4�ֽڶ����ʱ��,
//��Ҫ�õ�������,ȷ�����ݻ�������ַ��4�ֽڶ����.
__align(4) u8 SDIO_DATA_BUFFER[512];

//SD����ʼ��
//����ֵ:0 ��ʼ����ȷ������ֵ����ʼ������
u8 SD_Init(void)
{
    u8 SD_Error;
    
    //��ʼ��ʱ��ʱ�Ӳ��ܴ���400KHZ 
    SDCARD_Handler.Instance=SDIO;
    SDCARD_Handler.Init.ClockEdge=SDIO_CLOCK_EDGE_RISING;          //������     
    SDCARD_Handler.Init.ClockBypass=SDIO_CLOCK_BYPASS_DISABLE;     //��ʹ��bypassģʽ��ֱ����HCLK���з�Ƶ�õ�SDIO_CK
    SDCARD_Handler.Init.ClockPowerSave=SDIO_CLOCK_POWER_SAVE_DISABLE;    //����ʱ���ر�ʱ�ӵ�Դ
    SDCARD_Handler.Init.BusWide=SDIO_BUS_WIDE_1B;                        //1λ������
    SDCARD_Handler.Init.HardwareFlowControl=SDIO_HARDWARE_FLOW_CONTROL_DISABLE;//�ر�Ӳ������
    SDCARD_Handler.Init.ClockDiv=SDIO_TRANSFER_CLK_DIV;            //SD����ʱ��Ƶ�����25MHZ
    
    SD_Error=HAL_SD_Init(&SDCARD_Handler,&SDCardInfo);
    if(SD_Error!=SD_OK) return 1;
    
    SD_Error=HAL_SD_WideBusOperation_Config(&SDCARD_Handler,SDIO_BUS_WIDE_4B);//ʹ�ܿ�����ģʽ
    if(SD_Error!=SD_OK) return 2;
    return 0;
}

//SDMMC�ײ�������ʱ��ʹ�ܣ��������ã�DMA����
//�˺����ᱻHAL_SD_Init()����
//hsd:SD�����
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    DMA_HandleTypeDef TxDMAHandler,RxDMAHandler;
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_SDIO_CLK_ENABLE();    //ʹ��SDIOʱ��
    __HAL_RCC_DMA2_CLK_ENABLE();    //ʹ��DMA2ʱ�� 
    __HAL_RCC_GPIOC_CLK_ENABLE();   //ʹ��GPIOCʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();   //ʹ��GPIODʱ��
    
    //PC8,9,10,11,12
    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //���츴��
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //��ʼ��
    
    //PD2
    GPIO_Initure.Pin=GPIO_PIN_2;            
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     //��ʼ��
}

//�õ�����Ϣ
//cardinfo:����Ϣ�洢��
//����ֵ:����״̬
u8 SD_GetCardInfo(HAL_SD_CardInfoTypedef *cardinfo)
{
    u8 sta;
    sta=HAL_SD_Get_CardInfo(&SDCARD_Handler,cardinfo);
    return sta;
}
 
//��SD��
//buf:�����ݻ�����
//sector:������ַ
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;
u8 SD_ReadDisk(u8* buf,u32 sector,u32 cnt)
{
    u8 sta=SD_OK;
    long long lsector=sector;
    u8 n;
    lsector<<=9;
    INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
    if((u32)buf%4!=0)
    {
        for(n=0;n<cnt;n++)
        {
            sta=HAL_SD_ReadBlocks(&SDCARD_Handler,(uint32_t*)SDIO_DATA_BUFFER,lsector+512*n,512,1);//����sector�Ķ�����
            memcpy(buf,SDIO_DATA_BUFFER,512);
            buf+=512;
        }
    }else
    {
        sta=HAL_SD_ReadBlocks(&SDCARD_Handler,(uint32_t*)buf,lsector,512,cnt);//����sector�Ķ�����
    }
    INTX_ENABLE();//�������ж�
    return sta;
}  

//дSD��
//buf:д���ݻ�����
//sector:������ַ
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;	
u8 SD_WriteDisk(u8 *buf,u32 sector,u32 cnt)
{   
    u8 sta=SD_OK;
    long long lsector=sector;
    u8 n;
    lsector<<=9;
    INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
    if((u32)buf%4!=0)
    {
        for(n=0;n<cnt;n++)
        {
            memcpy(SDIO_DATA_BUFFER,buf,512);
            sta=HAL_SD_WriteBlocks(&SDCARD_Handler,(uint32_t*)SDIO_DATA_BUFFER,lsector+512*n,512,1);//����sector��д����
            buf+=512;
        }
    }else
    {
        sta=HAL_SD_WriteBlocks(&SDCARD_Handler,(uint32_t*)buf,lsector,512,cnt);//���sector��д����
    }
	INTX_ENABLE();//�������ж�
    return sta;
}
