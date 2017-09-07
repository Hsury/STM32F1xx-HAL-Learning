/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
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
#include "main.h"
#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
HAL_StatusTypeDef Status;

#define ADDR_AT24C04_WRITE_FIRST_16_PAGES 0xA0
#define ADDR_AT24C04_WRITE_LAST_16_PAGES 0xA2
#define ADDR_AT24C04_READ 0xA1
#define AT24C04_PAGE_SIZE 16
#define AT24C04_TIMEOUT 0xFF
#define BUFFER_SIZE 512

uint8_t WriteBuffer[BUFFER_SIZE] = "COPYRIGHT(c) Waveshare\r\n"
                                   "Replace this string buffer with your own one (512 bytes or less). "
                                   "The string will be saved into the EEPROM.\r\n\r\n"
                                   "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" "
                                   "AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE "
                                   "IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE "
                                   "DISCLAIMED.";

uint8_t ReadBuffer[BUFFER_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
HAL_StatusTypeDef AT24C04_Write(I2C_HandleTypeDef *hi2c, uint8_t *pData);
HAL_StatusTypeDef AT24C04_Read(I2C_HandleTypeDef *hi2c, uint8_t *pData);
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
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */
  printf("\r\n*************** I2C Example ***************\r\n");

  /* Write data to EEPROM */
  if((Status = AT24C04_Write(&hi2c1, WriteBuffer)) != HAL_OK)
  {
    printf("\r\n EEPROM 24C04 write false. Error code: %d\r\n", Status);
  }

  /* Read data from EEPROM */
  if((Status = AT24C04_Read(&hi2c1, ReadBuffer)) == HAL_OK)
  {
    printf("\r\nReadBuffer = \r\n");
    printf("%s", ReadBuffer);
  }
  else
  {
    printf("\r\n EEPROM 24C04 read false. Error code: %d\r\n", Status);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
/**
  * @brief  Write an amount of data in blocking mode to the AT24C04 EEPROM.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C.
  * @param  pData Pointer to data buffer
  * @retval HAL status
  */
HAL_StatusTypeDef AT24C04_Write(I2C_HandleTypeDef *hi2c, uint8_t *pData)
{
  uint16_t MemAddress;
  uint16_t Page = 0;
  /** The AT24C04 internally organized with 32 pages of 16 bytes each, the 4K
    * requires an 9-bit data word address for random word addressing. 
    * However, AT24C04 can only receive 8-bit data per period, so the 9-bit 
    * data word address is actually compromised with the page address bit in 
    * device address and the 8-bit memory address.
    * The device address 0xA0 means the first 16 pages while 0xA2 means the 
    * last 32 pages. 
    * The jumpers will also define the address, so the address 0xA0 or 0xA2 
    * are not always right.
    */
  while(Page < 16) 
  {
    MemAddress = Page << 4;
  /** A page write is initiated the same as a byte write, but the 
    * microcontroller does not send a stop condition after the first data word
    * is clocked in. 
    * In AT24C04, 1 page = 16 bytes.
    * When the word address, internally generated, reaches the page boundary, 
    * the following byte is placed at the beginning of the same page.
    */
    Status = HAL_I2C_Mem_Write(&hi2c1, ADDR_AT24C04_WRITE_FIRST_16_PAGES, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, AT24C04_PAGE_SIZE, AT24C04_TIMEOUT);
    if(Status == HAL_OK)
    {
      Page++;
      pData += AT24C04_PAGE_SIZE;
      HAL_Delay(5);
    }
    else
    {
      return Status;
    }
  }
  while(Page >= 16 && Page < 32) 
  {
    MemAddress = (Page - 16) << 4;
    Status = HAL_I2C_Mem_Write(&hi2c1, ADDR_AT24C04_WRITE_LAST_16_PAGES, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, AT24C04_PAGE_SIZE, AT24C04_TIMEOUT);
    if(Status == HAL_OK)
    {
      Page++;
      pData += AT24C04_PAGE_SIZE;
      HAL_Delay(5);
    }
    else
    {
      return Status;
    }
  }
  return Status;
}
/**
  * @brief  Read all of the data from the AT24C04 EEPROM.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C.
  * @param  pData Pointer to data buffer
  * @retval HAL status
  */
HAL_StatusTypeDef AT24C04_Read(I2C_HandleTypeDef *hi2c, uint8_t *pData)
{
  uint16_t MemAddress = 0x00;
  Status = HAL_I2C_Mem_Read(&hi2c1, ADDR_AT24C04_READ, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, BUFFER_SIZE, AT24C04_TIMEOUT);
  return Status;
}

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
