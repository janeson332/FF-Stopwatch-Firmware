/**
  ******************************************************************************
  * @file    Buttons.c
  * @author  Stefan
  * @version V1.0
  * @date    14.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/

#include <assert.h>

#include "Buttons.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

typedef struct{
	GPIO_TypeDef* port;
	uint32_t pin;
}tGPIOInit;

static tGPIOInit gpioInitArr[BUTTONS_NUMBER_MAX] = {
	{GPIOC,GPIO_Pin_11}, // btn_next
	{GPIOC,GPIO_Pin_10}, // btn_select
	{GPIOA,GPIO_Pin_15}, // btn_res
	{GPIOC,GPIO_Pin_12}, // buzzer1
	{GPIOD,GPIO_Pin_2}   // buzzer2
};

void Buttons_Init(void){

	GPIO_InitTypeDef gpioInit;
	GPIO_StructInit(&gpioInit);

	// no usage yet for butten_res so clock won't get enabled
	//RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN ,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN,ENABLE);

	gpioInit.GPIO_Mode = GPIO_Mode_IN;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;

	for(uint8_t i = 0; i<BUTTONS_NUMBER_MAX; ++i){
		gpioInit.GPIO_Pin = gpioInitArr[i].pin;
		GPIO_Init(gpioInitArr[i].port, &gpioInit);
	}
}

uint8_t Buttons_GetState(tButtonType button){
	assert(button < BUTTONS_NUMBER_MAX);
	return GPIO_ReadInputDataBit(gpioInitArr[button].port,gpioInitArr[button].pin);
}
