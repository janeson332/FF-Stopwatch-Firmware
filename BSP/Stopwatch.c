/**
  ******************************************************************************
  * @file    Stopwatch.h
  * @author  Jahn Stefan <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    10.08.2018
  * @brief   Module for stopping time with internal timer (tim2)
  ******************************************************************************
  */

#include <string.h>
#include "Stopwatch.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#define STOPWATCH_TIM			TIM2
#define MAX_CNT_VALUE			999999

static uint32_t seconds = 0;

static void InitStopwatchTimer(void);
static void StopStopwatchTimer(void);
static void StartStopwatchTimer(void);
static uint32_t GetMillisFromTimer(void);

void Stopwatch_Init(void){
	InitStopwatchTimer();
}

void Stopwatch_Reset(void){
	Stopwatch_Stop(0);
	STOPWATCH_TIM->CNT = 0;
	seconds = 0;
}

void Stopwatch_StartContinue(void){
	StartStopwatchTimer();
}

void Stopwatch_Stop(StopTimeType_t *pTime){
	StopStopwatchTimer();
	if(pTime != 0){
		pTime->seconds = seconds;
		pTime->milliseconds = GetMillisFromTimer();
	}
}

void Stopwatch_GetTime(StopTimeType_t *pTime){
	if(pTime != 0){
		pTime->seconds = seconds;
		pTime->milliseconds = GetMillisFromTimer();
	}
}


/*** private functions ***/

static void StopStopwatchTimer(void){
	TIM_Cmd(STOPWATCH_TIM,DISABLE);
}
static void StartStopwatchTimer(void){
	TIM_Cmd(STOPWATCH_TIM,ENABLE);
}

static uint32_t GetMillisFromTimer(void){
	return ((1000.0*STOPWATCH_TIM->CNT)/60000.0);
}

static void InitStopwatchTimer(void){
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM2EN,ENABLE);


	TIM_TimeBaseInitTypeDef timInitStruct;
	memset(&timInitStruct,0,sizeof(TIM_TimeBaseInitTypeDef));

	timInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timInitStruct.TIM_Period = 60000;
	timInitStruct.TIM_Prescaler = 1400-1;
	TIM_TimeBaseInit(STOPWATCH_TIM,&timInitStruct);
	TIM_ITConfig(STOPWATCH_TIM,TIM_IT_Update,ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(STOPWATCH_TIM,TIM_IT_Update)){
		TIM_ClearITPendingBit(STOPWATCH_TIM,TIM_IT_Update);
		++seconds;
		seconds %= MAX_CNT_VALUE;
	}
}
