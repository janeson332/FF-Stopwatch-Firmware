/**
  ******************************************************************************
  * @file    RN52.h
  * @author  Stefan
  * @version V1.0
  * @date    21.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/
#ifndef RN52_H_INCLUDED
#define RN52_H_INCLUDED

#include "stm32f4xx.h"

typedef enum{
	RN52_OK = 0,
	RN52_NOK,
	RN52_Error,
	RN52_Timeout
}RN52RetType_t;

void RN52_Init(uint32_t (*getTick)());

void RN52_SendString(char const *str);

void RN52_SendChar(uint8_t ch);

void RN52_SetReceiveCB(void (*receiveCB)(uint8_t ch));

void RN52_PowerEnable(FunctionalState state);

FunctionalState RN52_Enabled(void);

RN52RetType_t RN52_DeviceConnected(void);


#endif /* RN52_H_INCLUDED */
