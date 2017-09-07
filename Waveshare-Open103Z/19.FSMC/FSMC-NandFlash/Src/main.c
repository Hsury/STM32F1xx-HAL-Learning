/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
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
#include "main.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include "stm32_bsp_nand.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
NAND_IDTypeDef NAND_ID;
NAND_AddressTypeDef NAND_Address;

/* for K9F1G08 */
#define NAND_PAGE_SIZE             ((uint16_t)0x0800) /* 2 * 1024 bytes per page w/o Spare Area */
#define NAND_BLOCK_SIZE            ((uint16_t)0x0040) /* 64 pages per block */
#define NAND_ZONE_SIZE             ((uint16_t)0x0400) /* 1024 Block per zone */
#define NAND_SPARE_AREA_SIZE       ((uint16_t)0x0040) /* last 64 bytes as spare area */
#define NAND_MAX_ZONE              ((uint16_t)0x0001) /* 1 zones of 1024 block */

#define NB_PAGE                 ((uint32_t)3)
#define BUFFER_SIZE             (NAND_PAGE_SIZE * NB_PAGE)
#define WRITE_READ_ADDR         ((uint32_t)0x0000)

static uint8_t TxBuffer[BUFFER_SIZE];
static uint8_t RxBuffer[BUFFER_SIZE];
uint32_t status;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_FSMC_Init();

  /* USER CODE BEGIN 2 */
  printf("***** NandFlash Example ******\r\n");
  printf("The CLE and ALE pins on the development board are A17 and A16 but the default code generated by STM32CubeMX uses A16 for CLE and A17 for ALE."
         "So if the source code is generated by STM32CubeMX again, you should manually edit the stm32f4xx_hal_nand.h file and modify:"
         "#define CMD_AREA                   ((uint32_t)(1U<<16U))"
         "#define ADDR_AREA                  ((uint32_t)(1U<<17U))"
         "to:"
         "#define CMD_AREA                   ((uint32_t)(1U<<17U))"
         "#define ADDR_AREA                  ((uint32_t)(1U<<16U))"
         "\r\n"
         );
  /* Read the NAND memory ID */
  BSP_NAND_Read_ID(&hnand1, &NAND_ID);

  printf("Nand Flash ID: 0x%02X%02X%02X%02X\r\n", NAND_ID.Maker_Id, NAND_ID.Device_Id,
          NAND_ID.Third_Id, NAND_ID.Fourth_Id );
  switch (NAND_ID.Maker_Id << 24 | NAND_ID.Device_Id << 16 | NAND_ID.Third_Id << 8 | NAND_ID.Fourth_Id) {
    case 0xECF18015:
      printf("Type: K9F1G08U0A\r\n");
      break;
    case 0xECF10095:
      printf("Type: K9F1G08U0B/C/D/E\r\n");
      break;
    case 0xADF1801D:
      printf("Type: HY27UF081G2A\r\n");
      break;
    default:
      printf("Type: Unknow\r\n");
      break;
  }
  
  /* Set the destination address for R/W */
  NAND_Address.Zone = 0;
  NAND_Address.Block = 0;
  NAND_Address.Page = 3;
  
  /* Erase the NAND first Block (Block 0) */
  /* Writing a page (including the spare area following the page) twice or more without erasing will
     lead to error bytes in the page (including the spare area following the page). */
  /* So you have to erase the block containing 64 pages even if just need to write some bytes */
  BSP_NAND_Erase_Block(&hnand1, &NAND_Address);

  /* Fill the buffer to send */
  for (uint32_t i = 0, tmp; i < BUFFER_SIZE; i += 2) {
    tmp = i / 2;
    TxBuffer[i] = (tmp & 0xFF00) >> 8;
    TxBuffer[i + 1] = (tmp & 0x00FF);
  }
  
  /* Write data to FMC NAND memory */
  BSP_NAND_Write_Page(&hnand1, &NAND_Address, TxBuffer, NB_PAGE);

  /* Read data from FMC NAND memory */
  BSP_NAND_Read_Page(&hnand1, &NAND_Address, RxBuffer, NB_PAGE);

  printf("Buffers read: ");
  for(uint32_t i = 0; i < BUFFER_SIZE; i++) {
    if(i % 16 == 0) {
      printf("\r\n");
    }
    if(i % 2 == 0) {
      printf("%02X", RxBuffer[i]);
    } else {
      printf("%02X ", RxBuffer[i]);
    }
  }

  if(memcmp(TxBuffer, RxBuffer,sizeof(TxBuffer)) == 0 ) {
    printf("\r\n\r\n NandFlash Read Write Test OK\r\n");
  } else {
    printf("\r\n\r\n NandFlash Read Write False\r\n");
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
