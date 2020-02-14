/**
  ******************************************************************************
  * @file    HookMemoryFunctions.c
  * @author  Janeson332
  * @version V1.0
  * @date    24.01.2020
  *
  *  implementation of hook functions and memory allocation functions
  *  needed by the FreeRTOS Kernel (depends on settings in FreeRTOSConfig.h)
  ******************************************************************************
*/

#include "HookMemoryFunctions.h"


void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize ){

static StaticTask_t idleTaskTCB;
static StackType_t idleTaskStack[ configMINIMAL_STACK_SIZE ];

    *ppxIdleTaskTCBBuffer = &idleTaskTCB;

    *ppxIdleTaskStackBuffer = idleTaskStack;

    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize ){

static StaticTask_t timerTaskTCB;
static StackType_t timerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task’s state will be stored. */
    *ppxTimerTaskTCBBuffer = &timerTaskTCB;

    /* Pass out the array that will be used as the Timer task’s stack. */
    *ppxTimerTaskStackBuffer = timerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vApplicationStackOverflowHook( TaskHandle_t *pxTask, signed char*pcTaskName ){

    while(1);
}
