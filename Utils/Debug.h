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

    void Debug_Init(void);

    void Debug_Write(char const * const str);


#endif /* DEBUG_H_INCLUDED */
