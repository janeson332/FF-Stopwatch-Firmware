/**
  ******************************************************************************
  * @file    Buttons.h
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    14.02.2020
  * @brief   Button interface
  ******************************************************************************
*/

#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include "stm32f4xx.h"

#define BUTTONS_NUMBER_MAX		5

typedef enum {
	btn_next = 0,
	btn_select = 1,
	btn_res = 2, //reserve, no usage yet
	buzzer1 = 3,
	buzzer2 = 4
}ButtonType_t;

/**
 * @brief initializes buttons
 */
void Buttons_Init(void);

/**
 * @brief returns value of requested button
 * @return 1 on pressed else 0
 */
uint8_t Buttons_GetState(ButtonType_t button);


#endif /* BUTTONS_H_INCLUDED */
