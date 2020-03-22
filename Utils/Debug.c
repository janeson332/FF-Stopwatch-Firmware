/**
  ******************************************************************************
  * @file    Debug.c
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    14.02.2020
  * @brief   Module for USART Debug output
  ******************************************************************************
*/


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "Debug.h"
#include "stm32f40x_uart2.h"
#include "semphr.h"

#define DEBUG_LOG_BUFFER_SIZE  256

SemaphoreHandle_t mSemaphore = 0;
StaticSemaphore_t mMutexBuffer;
static char buffer[DEBUG_LOG_BUFFER_SIZE];

void Debug_Init(void){
	UART2_Init();

	mSemaphore = xSemaphoreCreateMutexStatic(&mMutexBuffer);
	assert(mSemaphore!=0);
}

void Debug_Write(char const * const str){
	xSemaphoreTake(mSemaphore,portMAX_DELAY);
	UART2_SendString(str);
	xSemaphoreGive(mSemaphore);
}


void Debug_Log(char const * const str, int32_t line, char const * const functionName){
	vPortEnterCritical();
	xSemaphoreTake(mSemaphore,portMAX_DELAY);
	snprintf(buffer,DEBUG_LOG_BUFFER_SIZE,"[\"%s()\": Line: %d] -> %s\n",functionName,line,str);
	UART2_SendString(buffer);
	xSemaphoreGive(mSemaphore);
	vPortExitCritical();
}
