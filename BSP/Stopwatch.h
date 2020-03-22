/**
  ******************************************************************************
  * @file    Stopwatch.h
  * @author  Jahn Stefan <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    10.08.2018
  * @brief   Module for stopping time with internal timer (tim2)
  ******************************************************************************
  */

#ifndef STOPWATCH_H_INCLUDED
#define STOPWATCH_H_INCLUDED

#include <stdint.h>
#include "stm32f4xx.h"

typedef struct{
	uint32_t seconds;
	uint32_t milliseconds;
}StopTimeType_t;

void Stopwatch_Init(void);

void Stopwatch_Reset(void);

void Stopwatch_StartContinue(void);

void Stopwatch_Stop(StopTimeType_t *pTime);

void Stopwatch_GetTime(StopTimeType_t *pTime);


#endif
