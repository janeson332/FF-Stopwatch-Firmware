/**
  ******************************************************************************
  * @file    TaskDisplay.h
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    14.02.2020
  * @brief   Handles Display output
  ******************************************************************************
*/

#ifndef TASKDISPLAY_H_INCLUDED
#define TASKDISPLAY_H_INCLUDED

#include "FreeRTOS.h"
#include "Stopwatch.h"

#define TASK_DISPLAY_STACK_SIZE 256

/**
 * @brief initialize resources
 */
void TaskDisplay_Init(void);

/**
 * @brief task..
 */
void TaskDisplay(void);

/**
 * @brief writes string on display
 */
int8_t TaskDisplay_WriteString(char const * const str,uint32_t col, uint32_t row);

/**
 * @brief returns backlight value from the display
 */
uint8_t TaskDisplay_GetBacklightVal();

/**
 * @brief set/unset backlight value
 */
void TaskDisplay_SetBacklightVal(uint8_t enable);

/**
 * @brief print time string on display
 */
void TaskDisplay_PrintTimeString(void);

/**
 * @brief format time to fit on display, and print it
 * @param time: time to print
 * @param preString: string printed before time
 */
void TaskDisplay_FormatAndPrintTime(StopTimeType_t time,char const * preString);


#endif /* TASKDISPLAY_H_INCLUDED */
