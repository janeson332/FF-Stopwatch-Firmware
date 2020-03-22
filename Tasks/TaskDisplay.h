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
#include "Stopwatch.h"

#define TASK_DISPLAY_STACK_SIZE 256

void TaskDisplay(void);
int8_t TaskDisplay_WriteString(char const * const str,uint32_t col, uint32_t row);
uint8_t TaskDisplay_GetBacklightVal();
void TaskDisplay_SetBacklightVal(uint8_t enable);

void TaskDisplay_PrintTimeString(void);
void TaskDisplay_FormatAndPrintTime(StopTimeType_t time,char const * preString);


#endif /* TASKDISPLAY_H_INCLUDED */
