/**
  ******************************************************************************
  * @file    stm32f10x_uart1.h
  * @author  Jahn Stefan
  * @version V1.0
  * @date    03.12.2017
  * @brief   functions to operate with the usart1 as uart
  ******************************************************************************
  */

#ifndef STM32F4XX_UART1_H_INLCUDED
#define STM32F4XX_UART1_H_INLCUDED

#include <stdint.h>
#include "stm32f4xx.h"

void UART1_InterruptHandler(void);

void UART1_Init(void);

void UART1_SendChar(uint8_t ch);

void UART1_SendString(char const *str);

void UART1_SetReceiveParser(void (*ParserFunc)(uint8_t ch));


#endif
