/**
  ******************************************************************************
  * @file    Buttons.h
  * @author  Stefan
  * @version V1.0
  * @date    14.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/
#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include "stm32f4xx.h"

#define BUTTONS_NUMBER_MAX		5

typedef enum {
	btn_next = 0,
	btn_select = 1,
	btn_res = 2, //reserved
	buzzer1 = 3,
	buzzer2 = 4
}tButtonType;

void Buttons_Init(void);

uint8_t Buttons_GetState(tButtonType button);


#endif /* BUTTONS_H_INCLUDED */
