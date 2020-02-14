/**
  ******************************************************************************
  * @file    TaskDisplay.h
  * @author  Stefan
  * @version V1.0
  * @date    14.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/
#ifndef TASKDISPLAY_H_INCLUDED
#define TASKDISPLAY_H_INCLUDED

#include "FreeRTOS.h"


#define TASK_DISPLAY_STACK_SIZE 256

void TaskDisplay(void);
int8_t TaskDisplayWriteString(char const * const str,uint32_t col, uint32_t row);

#endif /* TASKDISPLAY_H_INCLUDED */
