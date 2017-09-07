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
osThreadId semaphoreTask1Handle;
osThreadId semaphoreTask2Handle;
osSemaphoreId SEMHandle;

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartSemaphoreTask1(void const * argument);
void StartSemaphoreTask2(void const * argument);

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

  /* Create the semaphores(s) */
  /* definition and creation of SEM */
  osSemaphoreDef(SEM);
  SEMHandle = osSemaphoreCreate(osSemaphore(SEM), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of semaphoreTask1 */
  osThreadDef(semaphoreTask1, StartSemaphoreTask1, osPriorityLow, 0, 128);
  semaphoreTask1Handle = osThreadCreate(osThread(semaphoreTask1), NULL);

  /* definition and creation of semaphoreTask2 */
  osThreadDef(semaphoreTask2, StartSemaphoreTask2, osPriorityIdle, 0, 128);
  semaphoreTask2Handle = osThreadCreate(osThread(semaphoreTask2), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartSemaphoreTask1 function */
void StartSemaphoreTask1(void const * argument)
{

  /* USER CODE BEGIN StartSemaphoreTask1 */
  uint32_t count = 0;
  /* Infinite loop */
  for(;;)
  {
    if (SEMHandle != NULL)
    {
      /* Try to obtain the semaphore. */
      if(osSemaphoreWait(SEMHandle , 100) == osOK)
      {
        count = osKernelSysTick() + 2000;
        
        /* Toggle LED1 every 200 ms for 2 seconds*/
        while (count >= osKernelSysTick())
        {
          /* Toggle LED1 */
          BSP_LED_Toggle(LED1);
          
          /* Delay 200ms */
          osDelay(200);
        }
        
        /* Turn off LED1 */
        BSP_LED_Off(LED1);
        
        /* Release the semaphore */
        osSemaphoreRelease(SEMHandle);
        
        /* Suspend ourselves to execute task 2 (lower priority)  */
        osThreadSuspend(NULL); 
      }
    }
  }
  /* USER CODE END StartSemaphoreTask1 */
}

/* StartSemaphoreTask2 function */
void StartSemaphoreTask2(void const * argument)
{
  /* USER CODE BEGIN StartSemaphoreTask2 */
  uint32_t count = 0;
  /* Infinite loop */
  for(;;)
  {
    if (SEMHandle != NULL)
    {
      /* Try to obtain the semaphore. */
      if(osSemaphoreWait(SEMHandle, 0) == osOK)
      {
        /* Resume Thread 1 (higher priority)*/
        osThreadResume(semaphoreTask1Handle);
        
        count = osKernelSysTick() + 2000;
        
        /* Toggle LED2 every 200 ms for 2 seconds*/
        while (count >= osKernelSysTick())
        {
          BSP_LED_Toggle(LED2);
          
          osDelay(200);
        }
        
        /* Turn off LED2 */
        BSP_LED_Off(LED2);
        
        /* Release the semaphore to unblock Task 1 (higher priority)  */
        osSemaphoreRelease(SEMHandle);
      }
    }
  }
  /* USER CODE END StartSemaphoreTask2 */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
