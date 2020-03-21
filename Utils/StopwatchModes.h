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

typedef enum{
	mode_SingleStop = 0,
	mode_DualStop = 1,
	mode_Settings=2,
	mode_None = 3,
	mode_Remote = 4
}StopwatchModeType_t;

typedef enum{
	StopwatchMode_OK,
	StopwatchMode_ReturnToMain,
	StopwatchMode_Error
}StopwatchModeRetType_t;

typedef struct{
	StopwatchModeRetType_t (*ModeFunction)();
	char const * const displayText;
}ModeHandleTableType_t;

void StopwatchMode_Reset(void);

ModeHandleTableType_t const * StopwatchModes_GetModeTable();

StopwatchModeRetType_t StopwatchMode_SingleStop(void);

StopwatchModeRetType_t StopwatchMode_DualStop(void);

StopwatchModeRetType_t StopwatchMode_Remote(void);

StopwatchModeRetType_t StopwatchMode_Settings(void);

#endif /* STOPWATCHMODES_H_INCLUDED */
