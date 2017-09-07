/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  *
  * Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "gpio.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId firstLEDTaskHandle;
osThreadId secondLEDTaskHandle;

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartFirstLEDTask(void const * argument);
void StartSecondLEDTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of firstLEDTask */
  osThreadDef(firstLEDTask, StartFirstLEDTask, osPriorityNormal, 0, 128);
  firstLEDTaskHandle = osThreadCreate(osThread(firstLEDTask), NULL);

  /* definition and creation of secondLEDTask */
  osThreadDef(secondLEDTask, StartSecondLEDTask, osPriorityIdle, 0, 128);
  secondLEDTaskHandle = osThreadCreate(osThread(secondLEDTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartFirstLEDTask function */
void StartFirstLEDTask(void const * argument)
{

  /* USER CODE BEGIN StartFirstLEDTask */
	uint32_t count = 0;
	(void) argument;
  /* Infinite loop */
  for(;;)
  {
    count = osKernelSysTick() + 5000;

    /* Toggle LED1 every 200 ms for 5 s */
    while (count >= osKernelSysTick())
    {
      BSP_LED_Toggle(LED1);

      osDelay(200);
    }

    /* Turn off LED1 */
    BSP_LED_Off(LED1);

    /* Suspend Thread 1 */
    osThreadSuspend(NULL);

    count = osKernelSysTick() + 5000;

    /* Toggle LED1 every 400 ms for 5 s */
    while (count >= osKernelSysTick())
    {
      BSP_LED_Toggle(LED1);

      osDelay(400);
    }

    /* Resume Thread 2*/
    osThreadResume(secondLEDTaskHandle);
  }

  /* USER CODE END StartFirstLEDTask */
}

/* StartSecondLEDTask function */
void StartSecondLEDTask(void const * argument)
{
  /* USER CODE BEGIN StartSecondLEDTask */
  uint32_t count;
  (void) argument;
  /* Infinite loop */
  for(;;)
  {
    count = osKernelSysTick() + 10000;

    /* Toggle LED2 every 500 ms for 10 s */
    while (count >= osKernelSysTick())
    {
      BSP_LED_Toggle(LED2);

      osDelay(500);
    }

    /* Turn off LED2 */
    BSP_LED_Off(LED2);

    /* Resume Thread 1 */
    osThreadResume(firstLEDTaskHandle);

    /* Suspend Thread 2 */
    osThreadSuspend(NULL);
  }
  /* USER CODE END StartSecondLEDTask */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
