#ifndef _SDMMC_SDCARD_H
#define _SDMMC_SDCARD_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F746������
//SD����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

#define SD_TIMEOUT ((uint32_t)100000000)            //��ʱʱ��  

extern SD_HandleTypeDef        SDCARD_Handler;     //SD�����
extern HAL_SD_CardInfoTypedef  SDCardInfo;         //SD����Ϣ�ṹ��

u8 SD_Init(void);
u8 SD_GetCardInfo(HAL_SD_CardInfoTypedef *cardinfo);
u8 SD_ReadDisk(u8* buf,u32 sector,u32 cnt);
u8 SD_WriteDisk(u8 *buf,u32 sector,u32 cnt);
u8 SD_ReadBlocks_DMA(uint32_t *buf,uint64_t sector,uint32_t blocksize,uint32_t cnt);
u8 SD_WriteBlocks_DMA(uint32_t *buf,uint64_t sector,uint32_t blocksize,uint32_t cnt);
#endif
