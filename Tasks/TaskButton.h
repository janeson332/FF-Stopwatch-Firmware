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

#define TASK_BUTTON_STACK_SIZE  128

void TaskButton(void);

#endif /* TASKBUTTON_H_INCLUDED */
