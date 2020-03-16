/**
  ******************************************************************************
  * @file    Debug.h
  * @author  Stefan
  * @version V1.0
  * @date    14.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include "FreeRTOS.h"

#define DEBUG_LOG(str) Debug_Log(str,__LINE__,__PRETTY_FUNCTION__)


void Debug_Init(void);

void Debug_Write(char const * const str);

void Debug_Log(char const * const str, int32_t line, char const * const functionName);


#endif /* DEBUG_H_INCLUDED */
