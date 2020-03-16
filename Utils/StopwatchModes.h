/**
  ******************************************************************************
  * @file    StopwatchModes.h
  * @author  Stefan
  * @version V1.0
  * @date    19.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/
#ifndef STOPWATCHMODES_H_INCLUDED
#define STOPWATCHMODES_H_INCLUDED

#include "FreeRTOS.h"

typedef struct{
	int (*ModeFunction)();
	char const * const displayText;
}tModeHandleTable;

typedef enum{
	mode_SingleStop = 0,
	mode_DualStop = 1,
	mode_Settings=2,
	mode_None = 3,
	mode_Remote = 4
}tStopwatchModes;

tModeHandleTable const * StopwatchModes_GetModeTable();

int StopwatchMode_SingleStop(void);

int StopwatchMode_DualStop(void);

int StopwatchMode_Remote(void);

int StopwatchMode_Settings(void);

#endif /* STOPWATCHMODES_H_INCLUDED */
