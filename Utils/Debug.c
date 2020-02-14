/**
  ******************************************************************************
  * @file    Debug.c
  * @author  Stefan
  * @version V1.0
  * @date    14.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/


#include <assert.h>
#include "Debug.h"
#include "stm32f40x_uart2.h"
#include "semphr.h"

SemaphoreHandle_t mSemaphore = 0;
StaticSemaphore_t mMutexBuffer;

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
