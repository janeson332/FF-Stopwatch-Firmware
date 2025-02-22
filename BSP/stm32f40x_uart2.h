/**
  ******************************************************************************
  * @file    stm32f10x_uart2.h
  * @author  Jahn Stefan
  * @version V1.0
  * @date    03.12.2017
  * @brief   functions to operate with the usart2 as uart
  ******************************************************************************
  */

#ifndef STM32F4XX_UART2_H_INLCUDED
#define STM32F4XX_UART2_H_INLCUDED

#include <stdint.h>
#include "stm32f4xx.h"

void UART2_InterruptHandler(void);

void UART2_Init(void);

void UART2_SendChar(uint8_t ch);

void UART2_SendString(char const *str);

void UART2_SetReceiveParser(void (*ParserFunc)(uint8_t ch));




#endif
