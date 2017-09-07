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
#include "stm32f1xx_hal.h"
#include "gpio.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId rxTaskHandle;
osThreadId txTaskHandle;
osMessageQId osQueueHandle;

/* USER CODE BEGIN Variables */
/* The rate at which the Tx task sends to the queue. */
#define TX_DELAY                 (500)

/* The value that is sent from the Tx task to the Rx task on the queue. */
#define QUEUED_VALUE             (100)

/* The length of time the LED will remain on for.  It is on just long enough
   to be able to see with the human eye so as not to distort the power readings too
   much. */
#define LED_TOGGLE_DELAY         (20)
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartQueueRxTask(void const * argument);
void StartQueueTxTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Pre/Post sleep processing prototypes */
void PreSleepProcessing(uint32_t *ulExpectedIdleTime);
void PostSleepProcessing(uint32_t *ulExpectedIdleTime);

/* Hook prototypes */

/* USER CODE BEGIN PREPOSTSLEEP */
void PreSleepProcessing(uint32_t *ulExpectedIdleTime)
{
/* place for user code */ 
}

void PostSleepProcessing(uint32_t *ulExpectedIdleTime)
{
/* place for user code */
}
/* USER CODE END PREPOSTSLEEP */

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
  /* definition and creation of rxTask */
  osThreadDef(rxTask, StartQueueRxTask, osPriorityLow, 0, 128);
  rxTaskHandle = osThreadCreate(osThread(rxTask), NULL);

  /* definition and creation of txTask */
  osThreadDef(txTask, StartQueueTxTask, osPriorityIdle, 0, 128);
  txTaskHandle = osThreadCreate(osThread(txTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of osQueue */
  osMessageQDef(osQueue, 1, uint16_t);
  osQueueHandle = osMessageCreate(osMessageQ(osQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartQueueRxTask function */
void StartQueueRxTask(void const * argument)
{

  /* USER CODE BEGIN StartQueueRxTask */
  osEvent event;
  /* Infinite loop */
  for(;;)
  {
    /* Wait until something arrives in the queue. */
    event = osMessageGet(osQueueHandle, osWaitForever);
    
    /*  To get here something must have arrived, but is it the expected
        value?  If it is, turn the LED on for a short while. */
    if(event.status == osEventMessage)
    {
      if(event.value.v == QUEUED_VALUE)
      {  
        BSP_LED_On(LED1);
        osDelay(LED_TOGGLE_DELAY);
        BSP_LED_Off(LED1);
      }			
    }		
  }
  /* USER CODE END StartQueueRxTask */
}

/* StartQueueTxTask function */
void StartQueueTxTask(void const * argument)
{
  /* USER CODE BEGIN StartQueueTxTask */
  /* Infinite loop */
  for(;;)
  {
    /* Place this thread into the blocked state until it is time to run again.
       The kernel will place the MCU into the Retention low power sleep state
       when the idle thread next runs. */
    osDelay(TX_DELAY);

    /* Send to the queue - causing the queue receive thread to flash its LED.
       It should not be necessary to block on the queue send because the Rx
       thread will already have removed the last queued item. */    
    osMessagePut(osQueueHandle, (uint32_t)QUEUED_VALUE, 0);
    BSP_LED_Toggle(LED2);
  }
  /* USER CODE END StartQueueTxTask */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
