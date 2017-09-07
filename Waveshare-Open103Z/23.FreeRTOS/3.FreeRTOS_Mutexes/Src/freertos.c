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
osThreadId MutHighHandle;
osThreadId MutMediumHandle;
osThreadId MutLowHandle;
osMutexId osMutexHandle;

/* USER CODE BEGIN Variables */
#define mutexSHORT_DELAY     ((uint32_t) 20)
#define mutexNO_DELAY        ((uint32_t) 0)
#define mutexTWO_TICK_DELAY  ((uint32_t) 2)

/* Variables used to detect and latch errors. */
static __IO uint32_t HighPriorityThreadCycles = 0, MediumPriorityThreadCycles = 0, LowPriorityThreadCycles = 0;
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void MutexHighPriorityThread(void const * argument);
void MutexMeduimPriorityThread(void const * argument);
void MutexLowPriorityThread(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* Create the mutex(es) */
  /* definition and creation of osMutex */
  osMutexDef(osMutex);
  osMutexHandle = osMutexCreate(osMutex(osMutex));

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
  /* definition and creation of MutHigh */
  osThreadDef(MutHigh, MutexHighPriorityThread, osPriorityNormal, 0, 128);
  MutHighHandle = osThreadCreate(osThread(MutHigh), NULL);

  /* definition and creation of MutMedium */
  osThreadDef(MutMedium, MutexMeduimPriorityThread, osPriorityLow, 0, 128);
  MutMediumHandle = osThreadCreate(osThread(MutMedium), NULL);

  /* definition and creation of MutLow */
  osThreadDef(MutLow, MutexLowPriorityThread, osPriorityIdle, 0, 128);
  MutLowHandle = osThreadCreate(osThread(MutLow), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* MutexHighPriorityThread function */
void MutexHighPriorityThread(void const * argument)
{

  /* USER CODE BEGIN MutexHighPriorityThread */
  /* Infinite loop */
  for(;;)
  {
    /* The first time through the mutex will be immediately available, on
    subsequent times through the mutex will be held by the low priority thread
    at this point and this Take will cause the low priority thread to inherit
    the priority of this tadhr.  In this case the block time must be
    long enough to ensure the low priority thread will execute again before the
    block time expires.  If the block time does expire then the error
    flag will be set here. */
    if(osMutexWait(osMutexHandle, mutexTWO_TICK_DELAY) != osOK)
    {
      /* Toggle LED 3 to indicate error */
      BSP_LED_Toggle(LED3);
    }
    
    /* Ensure the other thread attempting to access the mutex
    are able to execute to ensure they either block (where a block 
    time is specified) or return an error (where no block time is 
    specified) as the mutex is held by this task. */
    osDelay(mutexSHORT_DELAY);
    
    /* We should now be able to release the mutex .  
    When the mutex is available again the medium priority thread
    should be unblocked but not run because it has a lower priority
    than this thread.  The low priority thread should also not run 
    at this point as it too has a lower priority than this thread. */
    if(osMutexRelease(osMutexHandle) != osOK)
    {
      /* Toggle LED 3 to indicate error */
      BSP_LED_Toggle(LED3);
    }
    
    /* Keep count of the number of cycles this thread has performed. */
    HighPriorityThreadCycles++;
    BSP_LED_Toggle(LED1);
    
    /* Suspend ourselves to the medium priority thread can execute. */
    osThreadSuspend(NULL);
  }
  /* USER CODE END MutexHighPriorityThread */
}

/* MutexMeduimPriorityThread function */
void MutexMeduimPriorityThread(void const * argument)
{
  /* USER CODE BEGIN MutexMeduimPriorityThread */
  /* Infinite loop */
  for(;;)
  {
    /* This thread will run while the high-priority thread is blocked, and the
    high-priority thread will block only once it has the mutex - therefore
    this call should block until the high-priority thread has given up the 
    mutex, and not actually execute past this call until the high-priority 
    thread is suspended. */
    if(osMutexWait(osMutexHandle, osWaitForever) == osOK)
    {
      if(osThreadGetState(MutHighHandle) != osThreadSuspended)
      {
        /* Did not expect to execute until the high priority thread was
        suspended.
        Toggle LED 3 to indicate error */
        BSP_LED_Toggle(LED3);
      }
      else
      {
        /* Give the mutex back before suspending ourselves to allow
        the low priority thread to obtain the mutex. */
        if(osMutexRelease(osMutexHandle) != osOK)
        {
          /* Toggle LED 3 to indicate error */
          BSP_LED_Toggle(LED3);
        } 
        osThreadSuspend(NULL);
      }
    }
    else
    {
      /* We should not leave the osMutexWait() function
      until the mutex was obtained. 
      Toggle LED 3 to indicate error */
      BSP_LED_Toggle(LED3);
    }
    
    /* The High and Medium priority threads should be in lock step. */
    if(HighPriorityThreadCycles != (MediumPriorityThreadCycles + 1))
    {
      /* Toggle LED 3 to indicate error */
      BSP_LED_Toggle(LED3);
    }
    
    /* Keep count of the number of cycles this task has performed so a 
    stall can be detected. */
    MediumPriorityThreadCycles++;
    BSP_LED_Toggle(LED2);
  }
  /* USER CODE END MutexMeduimPriorityThread */
}

/* MutexLowPriorityThread function */
void MutexLowPriorityThread(void const * argument)
{
  /* USER CODE BEGIN MutexLowPriorityThread */
  /* Infinite loop */
  for(;;)
  {
    /* Keep attempting to obtain the mutex.  We should only obtain it when
    the medium-priority thread has suspended itself, which in turn should only
    happen when the high-priority thread is also suspended. */
    if(osMutexWait(osMutexHandle, mutexNO_DELAY) == osOK)
    {
      /* Is the high and medium-priority threads suspended? */
      if((osThreadGetState(MutHighHandle) != osThreadSuspended) || (osThreadGetState(MutMediumHandle) != osThreadSuspended))
      {
        /* Toggle LED 3 to indicate error */
        BSP_LED_Toggle(LED3);
      }
      else
      {
        /* Keep count of the number of cycles this task has performed 
        so a stall can be detected. */
        LowPriorityThreadCycles++;
        BSP_LED_Toggle(LED4);
        
        /* We can resume the other tasks here even though they have a
        higher priority than the this thread. When they execute they
        will attempt to obtain the mutex but fail because the low-priority 
        thread is still the mutex holder.  this thread will then inherit 
        the higher priority.  The medium-priority thread will block indefinitely 
        when it attempts to obtain the mutex, the high-priority thread will only 
        block for a fixed period and an error will be latched if the 
        high-priority thread has not returned the mutex by the time this 
        fixed period has expired. */
        osThreadResume(MutMediumHandle);
        osThreadResume(MutHighHandle);
        
        /* The other two tasks should now have executed and no longer
        be suspended. */
        if((osThreadGetState(MutHighHandle) == osThreadSuspended) || (osThreadGetState(MutMediumHandle) == osThreadSuspended))
        {
          /* Toggle LED 3 to indicate error */
          BSP_LED_Toggle(LED3);
        }
        
        /* Release the mutex, disinheriting the higher priority again. */
        if(osMutexRelease(osMutexHandle) != osOK)
        {
          /* Toggle LED 3 to indicate error */
          BSP_LED_Toggle(LED3);
        }
      }
    }
    
#if configUSE_PREEMPTION == 0
    {
      taskYIELD();
    }
#endif
  }
  /* USER CODE END MutexLowPriorityThread */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
