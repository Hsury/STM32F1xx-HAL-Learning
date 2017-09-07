/**
  ******************************************************************************
  * @file    stm32_bsp_nand.h
  * @author  Yehui
  * @version V1.1
  * @date    13-Mar-2017
  * @brief   Header file of NAND HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 Waveshare</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Waveshare nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_BSP_NAND_H
#define __STM32_BSP_NAND_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Initialization/de-initialization functions  ********************************/
HAL_StatusTypeDef  BSP_NAND_Init(NAND_HandleTypeDef *hnand, FSMC_NAND_PCC_TimingTypeDef *ComSpace_Timing, FSMC_NAND_PCC_TimingTypeDef *AttSpace_Timing);

/**
  * @}
  */
  
/** @addtogroup NAND_Exported_Functions_Group2 Input and Output functions 
  * @{
  */

/* IO operation functions  ****************************************************/
HAL_StatusTypeDef  BSP_NAND_Read_ID(NAND_HandleTypeDef *hnand, NAND_IDTypeDef *pNAND_ID);
HAL_StatusTypeDef  BSP_NAND_Reset(NAND_HandleTypeDef *hnand);
HAL_StatusTypeDef  BSP_NAND_Read_Page(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumPageToRead);
HAL_StatusTypeDef  BSP_NAND_Write_Page(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumPageToWrite);
HAL_StatusTypeDef  BSP_NAND_Read_SpareArea(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumSpareAreaToRead);
HAL_StatusTypeDef  BSP_NAND_Write_SpareArea(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumSpareAreaTowrite);
HAL_StatusTypeDef  BSP_NAND_Erase_Block(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress);
uint32_t           BSP_NAND_Read_Status(NAND_HandleTypeDef *hnand);
uint32_t           BSP_NAND_Address_Inc(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress);

/* NAND State functions *******************************************************/
HAL_NAND_StateTypeDef BSP_NAND_GetState(NAND_HandleTypeDef *hnand);
uint32_t              BSP_NAND_Read_Status(NAND_HandleTypeDef *hnand);

/**
  * @}
  */
/**
  * @}
  */ 

/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif /* __STM32_BSP_NAND_H */

/************************ (C) COPYRIGHT Waveshare *****END OF FILE****/
