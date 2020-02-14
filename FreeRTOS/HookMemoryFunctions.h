/**
  ******************************************************************************
  * @file    HookMemoryFunctions.h
  * @author  Janeson332
  * @version V1.0
  * @date    24.01.2020
  *
  *  definition of hook functions and memory allocation functions
  *  needed by the FreeRTOS Kernel (depends on settings in FreeRTOSConfig.h)
  ******************************************************************************
*/


#ifndef HOOKMEMORYFUNCTIONS_H_INCLUDED
#define HOOKMEMORYFUNCTIONS_H_INCLUDED

#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief allocates task memory (stack and tcb) for the Idle Task
 * Needed because configSUPPORT_STATIC_ALLOCATION is set to 1
 */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize );

/**
 * @brief allocates task memory for the timer task
 * Needed because configUSE_TIMERS is set to 1 (software timers enabled)
 */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize );

/**
 * @brief hook when a stack overflow occurs, needed 'cause of configCHECK_FOR_STACK_OVERFLOW set to 2
 */
void vApplicationStackOverflowHook( TaskHandle_t *pxTask, signed char*pcTaskName );


#endif /* HOOKMEMORYFUNCTIONS_H_INCLUDED */
