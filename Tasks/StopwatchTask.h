/**
  ******************************************************************************
  * @file    StopwatchTask.h
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    18.02.2020
  * @brief   Main Task for stopwatch functionality
  ******************************************************************************
*/

#ifndef STOPWATCHTASK_H_INCLUDED
#define STOPWATCHTASK_H_INCLUDED


#include "FreeRTOS.h"
#include "task.h"

#define STOPWATCH_TASK_STACK_SIZE                512

// task notification values
#define STOPWATCH_TASK_NOTIFY_BUTTON_NEXT        ((uint32_t)(0x01))
#define STOPWATCH_TASK_NOTIFY_BUTTON_SELECT      ((uint32_t)(0x02))
#define STOPWATCH_TASK_NOTIFY_BUTTON_RESERVED    ((uint32_t)(0x04))
#define STOPWATCH_TASK_NOTIFY_BUZZER1_PRESSED    ((uint32_t)(0x08))
#define STOPWATCH_TASK_NOTIFY_BUZZER2_PRESSED    ((uint32_t)(0x10))
#define STOPWATCH_TASK_NOTIFY_BUZZER1_RELEASED   ((uint32_t)(0x20))
#define STOPWATCH_TASK_NOTIFY_BUZZER2_RELEASED   ((uint32_t)(0x40))


#define STOPWATCH_TASK_NOTIFY_WAIT              (STOPWATCH_TASK_NOTIFY_BUTTON_NEXT | STOPWATCH_TASK_NOTIFY_BUTTON_SELECT | \
	                                             STOPWATCH_TASK_NOTIFY_BUTTON_RESERVED |\
												 STOPWATCH_TASK_NOTIFY_BUZZER1_PRESSED | STOPWATCH_TASK_NOTIFY_BUZZER1_RELEASED | \
												 STOPWATCH_TASK_NOTIFY_BUZZER2_PRESSED | STOPWATCH_TASK_NOTIFY_BUZZER2_RELEASED )
/**
 * @brief initializes resources,..
 */
void StopwatchTask_Init(void);

/**
 * @brief returns task handle
 */
TaskHandle_t StopwatchTask_GetTaskHandle(void);

/**
 * @brief task..
 */
void StopwatchTask(void);

#endif /* STOPWATCHTASK_H_INCLUDED */
