/**
  ******************************************************************************
  * @file    TaskButton.h
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    14.02.2020
  * @brief   Task for handling button input
  ******************************************************************************
*/

#ifndef TASKBUTTON_H_INCLUDED
#define TASKBUTTON_H_INCLUDED



#include "FreeRTOS.h"
#include "task.h"

#define TASK_BUTTON_STACK_SIZE  512

/**
 * @brief initializes resources
 */
void TaskButton_Init(void);

/**
 * @brief task...
 */
void TaskButton(void);

/**
 * @brief sets task to send task notifications
 */
void TaskButton_SetRxEventTask(TaskHandle_t handle);


#endif /* TASKBUTTON_H_INCLUDED */
