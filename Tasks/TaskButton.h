/**
  ******************************************************************************
  * @file    TaskButton.h
  * @author  Stefan
  * @version V1.0
  * @date    14.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/
#ifndef TASKBUTTON_H_INCLUDED
#define TASKBUTTON_H_INCLUDED

#include "FreeRTOS.h"
#include "task.h"

#define TASK_BUTTON_STACK_SIZE  512

void TaskButton(void);
void TaskButton_SetRxEventTask(TaskHandle_t handle);

#endif /* TASKBUTTON_H_INCLUDED */
