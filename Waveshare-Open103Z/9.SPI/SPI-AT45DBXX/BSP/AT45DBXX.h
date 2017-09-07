/*********************************************************************************************************
*
* File                : AT45DBXX.h
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 5.15
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2015, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#ifndef _AT45DBXX_H
#define _AT45DBXX_H

#include "stm32f1xx.h"
#include "spi.h"

#define BUF1_WRITE 0x84
#define BUF2_WRITE 0x87
#define BUF1_READ 0xD4
#define BUF2_READ 0xD6
#define BBUF1_TO_MM_PAGE_PROG_WITH_ERASE 0x83
#define BBUF2_TO_MM_PAGE_PROG_WITH_ERASE 0x86
#define MM_PAGE_TO_B1_XFER 0x53
#define MM_PAGE_TO_B2_XFER 0x55
#define PAGE_ERASE 0x81   // 512/528 bytes per page
#define SECTOR_ERASE 0x7C // 128k bytes per sector
#define READ_STATE_REGISTER 0xD7
#define READ_ID 0x9F

#define AT45DBXX_Enable()        HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
#define AT45DBXX_Disable()       HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);



void AT45DBXX_Read_ID(uint8_t *Data);
void AT45DBXX_Write(uint16_t BufferOffset, uint8_t Data);
uint8_t AT45DBXX_Read(uint16_t BufferOffset);


#endif /*_AT45DBXX_H*/

