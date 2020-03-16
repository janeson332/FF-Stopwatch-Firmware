/**
  ******************************************************************************
  * @file    Stopwatch.h
  * @author  Jahn Stefan
  * @version V1.0
  * @date    10.08.2018
  * @brief   Module to handle a stopwatch (stops time and so on..)
  ******************************************************************************
  */

#include "stm32f4xx.h"
#include <stdint.h>


#ifndef STOPWATCH_H_INCLUDED
#define STOPWATCH_H_INCLUDED

typedef struct{
	uint32_t seconds;
	uint32_t milliseconds;
}tTimeStruct;

void Stopwatch_Init(void);

void Stopwatch_Reset(void);

void Stopwatch_StartContinue(void);

void Stopwatch_Stop(tTimeStruct *pTime);

void Stopwatch_GetTime(tTimeStruct *pTime);


#endif
