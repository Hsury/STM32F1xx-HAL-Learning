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
osThreadId QConsHandle;
osThreadId QProdHandle;
osMessageQId osQueueHandle;

/* USER CODE BEGIN Variables */
uint32_t ProducerValue = 0, ConsumerValue = 0;
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void MessageQueueConsumer(void const * argument);
void MessageQueueProducer(void const * argument);

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
  /* definition and creation of QCons */
  osThreadDef(QCons, MessageQueueConsumer, osPriorityNormal, 0, 128);
  QConsHandle = osThreadCreate(osThread(QCons), NULL);

  /* definition and creation of QProd */
  osThreadDef(QProd, MessageQueueProducer, osPriorityIdle, 0, 128);
  QProdHandle = osThreadCreate(osThread(QProd), NULL);

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

/* MessageQueueConsumer function */
void MessageQueueConsumer(void const * argument)
{

  /* USER CODE BEGIN MessageQueueConsumer */
    osEvent event;
  /* Infinite loop */
  for(;;)
  {
    /* Get the message from the queue */
    event = osMessageGet(osQueueHandle, 100);
    
    if(event.status == osEventMessage)
    {
      if(event.value.v != ConsumerValue)
      {
        /* Catch-up. */
        ConsumerValue = event.value.v;
        
        /* Toggle LED3 to indicate error */
        BSP_LED_Toggle(LED3);
      }
      else
      {  
        /* Increment the value we expect to remove from the queue next time
        round. */
        ++ConsumerValue;
        
        /* Toggle LED2 to indicate a correct number received */
        BSP_LED_Toggle(LED2);
      }      
    }  
  }
  /* USER CODE END MessageQueueConsumer */
}

/* MessageQueueProducer function */
void MessageQueueProducer(void const * argument)
{
  /* USER CODE BEGIN MessageQueueProducer */
  /* Infinite loop */
  for(;;)
  {
    if(osMessagePut(osQueueHandle, ProducerValue, 100) != osOK)  
    {      
      /* Toggle LED3 to indicate error */
      BSP_LED_Toggle(LED3);
    }
    else
    {
      /* Increment the variable we are going to post next time round.  The
      consumer will expect the numbers to follow in numerical order. */
      ++ProducerValue;
      
      /* Toggle LED1 to indicate a correct number received */
      BSP_LED_Toggle(LED1);

      osDelay(250);
    }
  }
  /* USER CODE END MessageQueueProducer */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
