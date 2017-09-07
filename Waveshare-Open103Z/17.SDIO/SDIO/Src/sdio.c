/**
  ******************************************************************************
  * File Name          : SDIO.c
  * Description        : This file provides code for the configuration
  *                      of the SDIO instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
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
#include "sdio.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypedef SDCardInfo;
DMA_HandleTypeDef hdma_sdio;

/* SDIO init function */

void MX_SDIO_SD_Init(void)
{

  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 2;
  if (HAL_SD_Init(&hsd, &SDCardInfo) != SD_OK)
  {
    Error_Handler();
  }

  HAL_SD_WideBusOperation_Config(&hsd, SDIO_BUS_WIDE_4B);

}

void HAL_SD_MspInit(SD_HandleTypeDef* sdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(sdHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspInit 0 */

  /* USER CODE END SDIO_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SDIO_CLK_ENABLE();
  
    /**SDIO GPIO Configuration    
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_sdio.Instance = DMA2_Channel4;
    hdma_sdio.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdio.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio.Init.Mode = DMA_NORMAL;
    hdma_sdio.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_sdio) != HAL_OK)
    {
      Error_Handler();
    }

    /* Several peripheral DMA handle pointers point to the same DMA handle.
     Be aware that there is only one channel to perform all the requested DMAs. */
    /* Be sure to change transfer direction before calling
     HAL_SD_ReadBlocks_DMA or HAL_SD_WriteBlocks_DMA. */
    __HAL_LINKDMA(sdHandle,hdmarx,hdma_sdio);
    __HAL_LINKDMA(sdHandle,hdmatx,hdma_sdio);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(SDIO_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspInit 1 */

  /* USER CODE END SDIO_MspInit 1 */
  }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* sdHandle)
{

  if(sdHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspDeInit 0 */

  /* USER CODE END SDIO_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDIO_CLK_DISABLE();
  
    /**SDIO GPIO Configuration    
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD 
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(sdHandle->hdmarx);
    HAL_DMA_DeInit(sdHandle->hdmatx);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(SDIO_IRQn);

  }
  /* USER CODE BEGIN SDIO_MspDeInit 1 */

  /* USER CODE END SDIO_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */
/**
  * @brief SD_DMAConfigRx
  * @par Function Description
  *   This function configure the DMA to receive data from the SD card
  * @retval
  *  SD_ERROR or SD_OK
  */
HAL_SD_ErrorTypedef SD_DMAConfigRx(SD_HandleTypeDef *hsd)
{
  static DMA_HandleTypeDef hdma_rx;
  HAL_StatusTypeDef status = HAL_ERROR;
  
  /* Invalidate the dma tx handle*/
  hsd->hdmatx = NULL;
  hsd->hdmarx = NULL;
  
  if(hsd->hdmarx == NULL)
  {
    /* Configure DMA Rx parameters */
    hdma_rx.Instance                 = DMA2_Channel4;
    hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hdma_rx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsd, hdmarx, hdma_rx);

    /* Stop any ongoing transfer and reset the state*/
    HAL_DMA_Abort(&hdma_rx);
    
    /* Deinitialize the Channel for new transfer */
    HAL_DMA_DeInit(&hdma_rx);

    /* Configure the DMA Channel */
    status = HAL_DMA_Init(&hdma_rx);
    
    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(DMA2_Channel4_5_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);
  }
  else
  {
    status = HAL_OK;
  }
  
  return (status != HAL_OK? SD_ERROR : SD_OK);
}

/**
  * @brief SD_DMAConfigTx
  * @par Function Description
  *   This function configure the DMA to transmit data to the SD card
  * @retval
  *  SD_ERROR or SD_OK
  */
HAL_SD_ErrorTypedef SD_DMAConfigTx(SD_HandleTypeDef *hsd)
{
  static DMA_HandleTypeDef hdma_tx;
  HAL_StatusTypeDef status;
  
  /* Invalidate the dma tx handle*/
  hsd->hdmatx = NULL;
  hsd->hdmarx = NULL;
  
  if(hsd->hdmatx == NULL)
  {
    /* Configure DMA Tx parameters */
    hdma_tx.Instance                 = DMA2_Channel4;
    hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hdma_tx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    
    /* Associate the DMA handle */
    __HAL_LINKDMA(hsd, hdmatx, hdma_tx);
    
    /* Stop any ongoing transfer and reset the state*/
    HAL_DMA_Abort(&hdma_tx);
    
    /* Deinitialize the Channel for new transfer */
    HAL_DMA_DeInit(&hdma_tx);
    
    /* Configure the DMA Channel */
    status = HAL_DMA_Init(&hdma_tx); 
    
    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(DMA2_Channel4_5_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);  
  }
  else
  {
    status = HAL_OK;
  }
  
  return (status != HAL_OK? SD_ERROR : SD_OK);
}
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
