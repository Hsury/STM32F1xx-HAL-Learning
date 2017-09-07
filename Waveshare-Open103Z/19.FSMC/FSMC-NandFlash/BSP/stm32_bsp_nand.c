/**
  ******************************************************************************
  * @file    stm32_bsp_nand.c
  * @author  Yehui
  * @version V1.1
  * @date    13-Mar-2016
  * @brief   NAND Flash driver for K9F1G08U0B/C/D/E.
  *         
  @verbatim
  ==============================================================================
                         ##### How to use this driver #####
  ==============================================================================    
    [..]
      This driver is a set of generic APIs which handle standard NAND flash operations.
      If a NAND flash device contains different operations and/or implementations, 
      it should be implemented separately.
      
      This driver contains different implementations for stm32f4xx_hal_nand.c.
      R/W functions generated in STM32CubeMX are not suit for K9F1G08U0B/C/D/E.
      The following functions are re-written:
        [+] BSP_NAND_Read_Page            (alternative of HAL_NAND_Read_Page      )
        [+] BSP_NAND_Write_Page           (alternative of HAL_NAND_Write_Page     )
        [+] BSP_NAND_Read_SpareArea       (alternative of HAL_NAND_Read_SpareArea )
        [+] BSP_NAND_Write_SpareArea      (alternative of HAL_NAND_Write_SpareArea)
        [+] BSP_NAND_Erase_Block          (alternative of HAL_NAND_Erase_Block    )

  @endverbatim
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32_bsp_nand.h"

#ifdef HAL_NAND_MODULE_ENABLED

    
/** @defgroup NAND_Exported_Functions_Group1 Initialization and de-initialization functions 
  * @brief    Initialization and Configuration functions 
  *
  @verbatim    
  ==============================================================================
            ##### NAND Initialization and de-initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to initialize/de-initialize
    the NAND memory
  
@endverbatim
  * @{
  */
    
/**
  * @brief  Perform NAND memory Initialization sequence
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  ComSpace_Timing: pointer to Common space timing structure
  * @param  AttSpace_Timing: pointer to Attribute space timing structure
  * @retval HAL status
  */
HAL_StatusTypeDef  BSP_NAND_Init(NAND_HandleTypeDef *hnand, FSMC_NAND_PCC_TimingTypeDef *ComSpace_Timing, FSMC_NAND_PCC_TimingTypeDef *AttSpace_Timing)
{
  return HAL_NAND_Reset(hnand);
}

/**
  * @brief  Perform NAND memory De-Initialization sequence
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval HAL status
  */
HAL_StatusTypeDef BSP_NAND_DeInit(NAND_HandleTypeDef *hnand)  
{
  return HAL_NAND_DeInit(hnand);
}

/**
  * @}
  */
  
/** @defgroup NAND_Exported_Functions_Group2 Input and Output functions 
  * @brief    Input Output and memory control functions 
  *
  @verbatim    
  ==============================================================================
                    ##### NAND Input and Output functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to use and control the NAND 
    memory
  
@endverbatim
  * @{
  */

/**
  * @brief  Read the NAND memory electronic signature
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pNAND_ID: NAND ID structure
  * @retval HAL status
  */
HAL_StatusTypeDef BSP_NAND_Read_ID(NAND_HandleTypeDef *hnand, NAND_IDTypeDef *pNAND_ID)
{
  return HAL_NAND_Read_ID(hnand, pNAND_ID);
}

/**
  * @brief  NAND memory reset
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval HAL status
  */
HAL_StatusTypeDef BSP_NAND_Reset(NAND_HandleTypeDef *hnand)
{
  return HAL_NAND_Reset(hnand);
}
  
/**
  * @brief  Read Page(s) from NAND memory block 
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress : pointer to NAND address structure
  * @param  pBuffer : pointer to destination read buffer
  * @param  NumPageToRead : number of pages to read from block 
  * @retval HAL status
  */
HAL_StatusTypeDef BSP_NAND_Read_Page(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumPageToRead)
{   
  __IO uint32_t index  = 0U;
  uint32_t deviceaddress = 0U, size = 0U, numpagesread = 0U, addressstatus = NAND_VALID_ADDRESS;
  NAND_AddressTypeDef nandaddress;
  
  uint16_t offsetinpage = 0x0000; /* Offset in a page, between 0-2111(page size + spare size),
                                  this func writes data from the beginning of a page, so the offset is 0 */
  uint32_t pagenum;               /* Continuous page number for the total pages, between 0-65535 */
  
  /* Process Locked */
  __HAL_LOCK(hnand); 
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  if(hnand->Init.NandBank == FSMC_NAND_BANK2)
  {
    deviceaddress = NAND_DEVICE1;
  }
  else
  {
    deviceaddress = NAND_DEVICE2;
  }

  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_BUSY;
  
  /* Save the content of pAddress as it will be modified */
  nandaddress.Block     = pAddress->Block;
  nandaddress.Page      = pAddress->Page;
  nandaddress.Zone      = pAddress->Zone;
  
  /* Page(s) read loop */  
  while((NumPageToRead != 0U) && (addressstatus == NAND_VALID_ADDRESS))
  {     
    /* Convert the nandaddress to the page number */
    pagenum = nandaddress.Page + (nandaddress.Block * hnand->Info.BlockSize) + (nandaddress.Zone * hnand->Info.ZoneSize * hnand->Info.BlockSize);

    /* update the buffer size */
    size = hnand->Info.PageSize + ((hnand->Info.PageSize) * numpagesread);
    
    /* Send read page command sequence */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_AREA_A;
   
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = offsetinpage;      /* Column Address A0-A7 */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = offsetinpage >> 8; /* Column Address A8-A11 */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = pagenum;           /* Row Address A12-A19 */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = pagenum >> 8;      /* Row Address A20-A27 */
  
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA))  = NAND_CMD_AREA_TRUE1;
    for(uint32_t i = 0; i < 20; i++)
    {
      /* A delay to do nothing, or else the data read from the NandFlash will be wrong. */
    }
    /* Get Data into Buffer */    
    for(; index < size; index++)
    {
      *(uint8_t *)pBuffer++ = *(uint8_t *)deviceaddress;
    }
    
    /* Increment read pages number */
    numpagesread++;
    
    /* Decrement pages to read */
    NumPageToRead--;
    
    /* Increment the NAND page address */
    addressstatus = HAL_NAND_Address_Inc(hnand, &nandaddress);
  }
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnand);

  return HAL_OK;

}

/**
  * @brief  Write Page(s) to NAND memory block 
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress : pointer to NAND address structure
  * @param  pBuffer : pointer to source buffer to write  
  * @param  NumPageToWrite  : number of pages to write to block 
  * @retval HAL status
  */
HAL_StatusTypeDef BSP_NAND_Write_Page(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumPageToWrite)
{
  __IO uint32_t index   = 0U;
  uint32_t tickstart = 0U;
  uint32_t deviceaddress = 0U , size = 0U, numpageswritten = 0U, addressstatus = NAND_VALID_ADDRESS;
  NAND_AddressTypeDef nandaddress;
  
  uint16_t offsetinpage = 0x0000; /* Offset in a page, between 0-2111(page size + spare size),
                                  this func writes data from the beginning of a page, so the offset is 0 */
  uint32_t pagenum;               /* Continuous page number for the total pages, between 0-65535 */
  
  /* Process Locked */
  __HAL_LOCK(hnand);  

  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  if(hnand->Init.NandBank == FSMC_NAND_BANK2)
  {
    deviceaddress = NAND_DEVICE1;
  }
  else
  {
    deviceaddress = NAND_DEVICE2;
  }
  
  /* Update the NAND controller state */ 
  hnand->State = HAL_NAND_STATE_BUSY;
  
  /* Save the content of pAddress as it will be modified */
  nandaddress.Block     = pAddress->Block;
  nandaddress.Page      = pAddress->Page;
  nandaddress.Zone      = pAddress->Zone;
  
  /* Page(s) write loop */
  while((NumPageToWrite != 0U) && (addressstatus == NAND_VALID_ADDRESS))
  {
    /* Convert the nandaddress to the page number */
    pagenum = nandaddress.Page + (nandaddress.Block * hnand->Info.BlockSize) + (nandaddress.Zone * hnand->Info.ZoneSize * hnand->Info.BlockSize);

    /* update the buffer size */
    size = hnand->Info.PageSize + ((hnand->Info.PageSize) * numpageswritten);
    
    /* Send write page command sequence */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE0;

    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = offsetinpage;      /* Column Address A0-A7 */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = offsetinpage >> 8; /* Column Address A8-A11 */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = pagenum;           /* Row Address A12-A19 */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = pagenum >> 8;      /* Row Address A20-A27 */

    /* Write data to memory */
    for(; index < size; index++)
    {
      *(__IO uint8_t *)deviceaddress = *(uint8_t *)pBuffer++;
    }
   
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;
    
    /* Get tick */
    tickstart = HAL_GetTick();
    
    /* Read status until NAND is ready */
    while(HAL_NAND_Read_Status(hnand) != NAND_READY)
    {
      if((HAL_GetTick() - tickstart ) > NAND_WRITE_TIMEOUT)
      {
        return HAL_TIMEOUT; 
      }
    }
 
    /* Increment written pages number */
    numpageswritten++;
    
    /* Decrement pages to write */
    NumPageToWrite--;
    
    /* Increment the NAND page address */
    addressstatus = HAL_NAND_Address_Inc(hnand, &nandaddress);
  }
  
  /* Update the NAND controller state */ 
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnand);
  
  return HAL_OK;
}

/**
  * @brief  Read Spare area(s) from NAND memory 
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress : pointer to NAND address structure
  * @param  pBuffer: pointer to source buffer to write  
  * @param  NumSpareAreaToRead: Number of spare area to read  
  * @retval HAL status
*/
HAL_StatusTypeDef BSP_NAND_Read_SpareArea(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumSpareAreaToRead)
{
  __IO uint32_t index   = 0U; 
  uint32_t deviceaddress = 0U, size = 0U, num_spare_area_read = 0U, addressstatus = NAND_VALID_ADDRESS;
  NAND_AddressTypeDef nandaddress;
  
  uint16_t offsetinpage = hnand->Info.PageSize; /* Offset in a page, between 0-2111(page size + spare size),
                                                   this func writes data from the beginning of a spare area, so the offset is the PageSize */
  uint32_t pagenum;                             /* Continuous page number for the total pages, between 0-65535 */

  /* Process Locked */
  __HAL_LOCK(hnand);  
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  if(hnand->Init.NandBank == FSMC_NAND_BANK2)
  {
    deviceaddress = NAND_DEVICE1;
  }
  else
  {
    deviceaddress = NAND_DEVICE2;
  }
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_BUSY;
  
  /* Save the content of pAddress as it will be modified */
  nandaddress.Block     = pAddress->Block;
  nandaddress.Page      = pAddress->Page;
  nandaddress.Zone      = pAddress->Zone;
  
  /* Spare area(s) read loop */ 
  while((NumSpareAreaToRead != 0U) && (addressstatus == NAND_VALID_ADDRESS))
  {     
    /* Convert the nandaddress to the page number */
    pagenum = nandaddress.Page + (nandaddress.Block * hnand->Info.BlockSize) + (nandaddress.Zone * hnand->Info.ZoneSize * hnand->Info.BlockSize);

    /* update the buffer size */
    size = (hnand->Info.SpareAreaSize) + ((hnand->Info.SpareAreaSize) * num_spare_area_read);

    /* Send read spare area command sequence */     
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_AREA_A;
    
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = offsetinpage;      /* Column Address A0-A7 */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = offsetinpage >> 8; /* Column Address A8-A11 */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = pagenum;           /* Row Address A12-A19 */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = pagenum >> 8;      /* Row Address A20-A27 */

    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_AREA_TRUE1;
      
    for(uint32_t i = 0; i < 20; i++)
    {
      /* A delay to do nothing, or else the data read from the NandFlash will be wrong. */
    }
    
    /* Get Data into Buffer */
    for(; index < size; index++)
    {
      *(uint8_t *)pBuffer++ = *(uint8_t *)deviceaddress;
    }
    
    /* Increment read spare areas number */
    num_spare_area_read++;
    
    /* Decrement spare areas to read */
    NumSpareAreaToRead--;
    
    /* Increment the NAND page address */
    addressstatus = HAL_NAND_Address_Inc(hnand, &nandaddress);
  }
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnand);

  return HAL_OK;  
}

/**
  * @brief  Write Spare area(s) to NAND memory 
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress : pointer to NAND address structure
  * @param  pBuffer : pointer to source buffer to write  
  * @param  NumSpareAreaTowrite  : number of spare areas to write to block
  * @retval HAL status
  */
HAL_StatusTypeDef BSP_NAND_Write_SpareArea(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumSpareAreaTowrite)
{
  __IO uint32_t index = 0U;
  uint32_t tickstart = 0U;
  uint32_t deviceaddress = 0U, size = 0U, num_spare_area_written = 0U, addressstatus = NAND_VALID_ADDRESS;
  NAND_AddressTypeDef nandaddress;
  
  uint16_t offsetinpage = hnand->Info.PageSize; /* Offset in a page, between 0-2111(page size + spare size),
                                                   this func writes data from the beginning of a spare area, so the offset is the PageSize */
  uint32_t pagenum;               /* Continuous page number for the total pages, between 0-65535 */

  /* Process Locked */
  __HAL_LOCK(hnand); 
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  if(hnand->Init.NandBank == FSMC_NAND_BANK2)
  {
    deviceaddress = NAND_DEVICE1;
  }
  else
  {
    deviceaddress = NAND_DEVICE2;
  }
  
  /* Update the FMC_NAND controller state */
  hnand->State = HAL_NAND_STATE_BUSY;  
  
  /* Save the content of pAddress as it will be modified */
  nandaddress.Block     = pAddress->Block;
  nandaddress.Page      = pAddress->Page;
  nandaddress.Zone      = pAddress->Zone;
  
  /* Spare area(s) write loop */
  while((NumSpareAreaTowrite != 0U) && (addressstatus == NAND_VALID_ADDRESS))
  {
    /* Convert the nandaddress to the page number */
    pagenum = nandaddress.Page + (nandaddress.Block * hnand->Info.BlockSize) + (nandaddress.Zone * hnand->Info.ZoneSize * hnand->Info.BlockSize);

    /* update the buffer size */
    size = (hnand->Info.SpareAreaSize) + ((hnand->Info.SpareAreaSize) * num_spare_area_written);

    /* Send write Spare area command sequence */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE0;

    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = offsetinpage;      /* Column Address A0-A7 */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = offsetinpage >> 8; /* Column Address A8-A11 */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = pagenum;           /* Row Address A12-A19 */
    *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = pagenum >> 8;      /* Row Address A20-A27 */

    /* Write data to memory */
    for(; index < size; index++)
    {
      *(__IO uint8_t *)deviceaddress = *(uint8_t *)pBuffer++;
    }
   
    *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;
    
    /* Get tick */
    tickstart = HAL_GetTick();
    
    /* Read status until NAND is ready */
    while(HAL_NAND_Read_Status(hnand) != NAND_READY)
    {
      if((HAL_GetTick() - tickstart ) > NAND_WRITE_TIMEOUT)
      {
        return HAL_TIMEOUT; 
      }
    }

    /* Increment written spare areas number */
    num_spare_area_written++;
    
    /* Decrement spare areas to write */
    NumSpareAreaTowrite--;
    
    /* Increment the NAND page address */
    addressstatus = HAL_NAND_Address_Inc(hnand, &nandaddress);
  }

  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hnand);
    
  return HAL_OK;
}

/**
  * @brief  NAND memory Block erase 
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress : pointer to NAND address structure
  * @retval HAL status
  */
HAL_StatusTypeDef BSP_NAND_Erase_Block(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress)
{
  uint32_t deviceaddress = 0U;
  uint32_t tickstart = 0U;
  uint32_t pagenum;               /* Continuous page number for the total pages, between 0-65535 */

  /* Process Locked */
  __HAL_LOCK(hnand);
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  if(hnand->Init.NandBank == FSMC_NAND_BANK2)
  {
    deviceaddress = NAND_DEVICE1;
  }
  else
  {
    deviceaddress = NAND_DEVICE2;
  }
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_BUSY;  
  
  /* Convert the nandaddress to the page number */
  pagenum = pAddress->Page + (pAddress->Block * hnand->Info.BlockSize) + (pAddress->Zone * hnand->Info.ZoneSize * hnand->Info.BlockSize);

  
  /* Send Erase block command sequence */
  *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_ERASE0;
  *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = pagenum;       /* Row Address A12-A19 */
  *(__IO uint8_t *)((uint32_t)(deviceaddress | ADDR_AREA)) = pagenum >> 8;  /* Row Address A20-A27 */
  *(__IO uint8_t *)((uint32_t)(deviceaddress | CMD_AREA)) = NAND_CMD_ERASE1; 
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Get tick */
  tickstart = HAL_GetTick();
  
  /* Read status until NAND is ready */
  while(HAL_NAND_Read_Status(hnand) != NAND_READY)
  {
    if((HAL_GetTick() - tickstart ) > NAND_WRITE_TIMEOUT)
    {
      /* Process unlocked */
      __HAL_UNLOCK(hnand);    
  
      return HAL_TIMEOUT; 
    } 
  }    
 
  /* Process unlocked */
  __HAL_UNLOCK(hnand);    
  
  return HAL_OK;  
}

/**
  * @brief  NAND memory read status 
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval NAND status
  */
uint32_t BSP_NAND_Read_Status(NAND_HandleTypeDef *hnand)
{
  return HAL_NAND_Read_Status(hnand); 
}

/**
  * @brief  Increment the NAND memory address
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param pAddress: pointer to NAND address structure
  * @retval The new status of the increment address operation. It can be:
  *           - NAND_VALID_ADDRESS: When the new address is valid address
  *           - NAND_INVALID_ADDRESS: When the new address is invalid address
  */
uint32_t BSP_NAND_Address_Inc(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress)
{
  return HAL_NAND_Address_Inc(hnand, pAddress);
}
    
/** @defgroup NAND_Exported_Functions_Group4 Peripheral State functions 
 *  @brief   Peripheral State functions 
 *
@verbatim   
  ==============================================================================
                         ##### NAND State functions #####
  ==============================================================================  
  [..]
    This subsection permits to get in run-time the status of the NAND controller 
    and the data flow.

@endverbatim
  * @{
  */
  
/**
  * @brief  return the NAND state
  * @param  hnand: pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @retval HAL state
  */
HAL_NAND_StateTypeDef BSP_NAND_GetState(NAND_HandleTypeDef *hnand)
{
  return HAL_NAND_GetState(hnand);
}

#endif /* HAL_NAND_MODULE_ENABLED  */

/**
  * @}
  */

/************************ (C) COPYRIGHT Waveshare *****END OF FILE****/
