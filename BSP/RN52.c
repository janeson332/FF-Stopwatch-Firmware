/**
  ******************************************************************************
  * @file    RN52.c
  * @author  Stefan
  * @version V1.0
  * @date    21.02.2020
  * @brief   Implementation of RN52 Bluetooth module
  ******************************************************************************
*/

#include <string.h>
#include <assert.h>

#include "RN52.h"
#include "RN52_Commands.h"
#include "stm32f40x_uart1.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "Utils/Debug.h"

//#define RN52_DEBUG_LOG_ENABLE
#ifndef RN52_DEBUG_LOG_ENABLE
	#define DEBUG_LOG(x)
#endif


#define DIGITAL_SWITCH_PIN			GPIO_Pin_8
#define DIGITAL_SWITCH_PORT         GPIOA
#define DIGITAL_SWITCH_RCC_ENR      RCC_AHB1ENR_GPIOAEN

#define RN52_POWER_ENABLE_PIN		GPIO_Pin_9
#define RN52_POWER_ENABLE_PORT		GPIOC
#define RN52_POWER_ENABLE_RCC_ENR	RCC_AHB1ENR_GPIOCEN

#define RN52_COMMAND_MODE_PIN		GPIO_Pin_7
#define RN52_COMMAND_MODE_PORT		GPIOC
#define RN52_COMMAND_MODE_RCC_ENR	RCC_AHB1ENR_GPIOCEN

#define RN52_INT_PIN				GPIO_Pin_8
#define RN52_INT_PIN_PORT			GPIOC
#define RN52_INT_PIN_RCC_ENR 		RCC_AHB1ENR_GPIOCEN

#define RN52_TIMEOUT_TICK           ((uint32_t)(100))

static RN52RetType_t Enter_CommandMode(void);
static RN52RetType_t Exit_CommandMode(void);

static uint32_t mCommandModeEntered = 0;
static uint32_t mResponseReceived = 0;
static char mResponse[256];
static void (*mSavedReceiveCallback)(uint8_t ch) = 0;
static uint32_t (*GetTicks)(void) = 0;

static uint8_t CharacterHexToNibble(char ch);
static uint8_t CharacterHexToByte(char high,char low);
static void CommandParser(uint8_t ch);
static uint8_t IsResponseValid(void);

void RN52_Init(uint32_t (*GetTick)(void)){
	assert(GetTick != 0);
	GetTicks = GetTick;

	UART1_Init();

	//enable clocks
	RCC_AHB1PeriphClockCmd(DIGITAL_SWITCH_RCC_ENR,ENABLE);
	RCC_AHB1PeriphClockCmd(RN52_POWER_ENABLE_RCC_ENR,ENABLE);


	//set to default off
	GPIO_ResetBits(DIGITAL_SWITCH_PORT,DIGITAL_SWITCH_PIN);
	GPIO_ResetBits(RN52_POWER_ENABLE_PORT,RN52_POWER_ENABLE_PIN);
	GPIO_SetBits(RN52_COMMAND_MODE_PORT,RN52_COMMAND_MODE_PIN);

	// initialize gpios
	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Mode = GPIO_Mode_OUT;
	gpioInit.GPIO_OType= GPIO_OType_PP;
	gpioInit.GPIO_Pin  = DIGITAL_SWITCH_PIN;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpioInit.GPIO_Speed= GPIO_Speed_25MHz;
	GPIO_Init(DIGITAL_SWITCH_PORT,&gpioInit);

	gpioInit.GPIO_Pin = RN52_POWER_ENABLE_PIN;
	GPIO_Init(RN52_POWER_ENABLE_PORT,&gpioInit);

	gpioInit.GPIO_Pin = RN52_COMMAND_MODE_PIN;
	GPIO_Init(RN52_COMMAND_MODE_PORT,&gpioInit);

	gpioInit.GPIO_Pin = RN52_INT_PIN;
	gpioInit.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(RN52_INT_PIN_PORT,&gpioInit);

}

void RN52_SendString(char const *str){
	UART1_SendString(str);
}

void RN52_SendChar(uint8_t ch){
	UART1_SendChar(ch);
}

void RN52_SetReceiveCB(void (*receiveCB)(uint8_t ch)){
	UART1_SetReceiveParser(receiveCB);
	mSavedReceiveCallback = receiveCB;
}

void RN52_PowerEnable(FunctionalState state){
	if(state == ENABLE){
		GPIO_SetBits(DIGITAL_SWITCH_PORT,DIGITAL_SWITCH_PIN);
		GPIO_SetBits(RN52_POWER_ENABLE_PORT,RN52_POWER_ENABLE_PIN);
	} else{
		GPIO_ResetBits(DIGITAL_SWITCH_PORT,DIGITAL_SWITCH_PIN);
		GPIO_ResetBits(RN52_POWER_ENABLE_PORT,RN52_POWER_ENABLE_PIN);
	}
}

FunctionalState RN52_Enabled(void){
	return (GPIO_ReadInputDataBit(DIGITAL_SWITCH_PORT,DIGITAL_SWITCH_PIN)==Bit_SET?ENABLE:DISABLE);
}

RN52RetType_t RN52_DeviceConnected(void){
	uint32_t tick = GetTicks();

	RN52RetType_t ret = Enter_CommandMode();
	if(ret !=RN52_OK){
		Exit_CommandMode();
		return ret;
	}
	// send command
	RN52_SendString(RN52_QUERY_CONNECTION_STATUS);

	while(!mResponseReceived){
		if(GetTicks() - tick >= RN52_TIMEOUT_TICK){
			Exit_CommandMode();
			return RN52_Timeout;
		}
	}

	Exit_CommandMode();
	mResponseReceived = 0;
	DEBUG_LOG(mResponse);
	if(IsResponseValid()){
		uint8_t byte0 = CharacterHexToByte(mResponse[0],mResponse[1]);
		if((byte0 & (RN52_QUERY_BYTE0_SPP | RN52_QUERY_BYTE0_A2DP))
				 == (RN52_QUERY_BYTE0_SPP | RN52_QUERY_BYTE0_A2DP)){
			return RN52_OK;
		} else{
			return RN52_NOK;
		}
	}
	else{
		return RN52_Error;
	}
}

static RN52RetType_t Enter_CommandMode(void){
	uint32_t tick = GetTicks();

	GPIO_ResetBits(RN52_COMMAND_MODE_PORT,RN52_COMMAND_MODE_PIN);
	UART1_SetReceiveParser(CommandParser);
	while(!mCommandModeEntered){
		if(GetTicks() - tick >= RN52_TIMEOUT_TICK){
			RN52_SetReceiveCB(mSavedReceiveCallback);
			DEBUG_LOG("TIMEOUT");
			return RN52_Timeout;
		}
	}
	DEBUG_LOG("OK");
	return RN52_OK;
}

static RN52RetType_t Exit_CommandMode(void){
	uint32_t tick = GetTicks();

	GPIO_SetBits(RN52_COMMAND_MODE_PORT,RN52_COMMAND_MODE_PIN);
	while(mCommandModeEntered){
		if(GetTicks() - tick >= RN52_TIMEOUT_TICK){
			RN52_SetReceiveCB(mSavedReceiveCallback);
			DEBUG_LOG("TIMEOUT");
			return RN52_Timeout;
		}
	}
	UART1_SetReceiveParser(mSavedReceiveCallback);
	DEBUG_LOG("OK");
	return RN52_OK;
}

static void CommandParser(uint8_t ch){
	static char receiveBuffer[256];
	static uint32_t receiveBufferPos = 0;

	if(ch != '\n'){
		if(ch != '\r'){
			receiveBuffer[receiveBufferPos] = ch;
			receiveBufferPos++;
		}
	}else{
		receiveBuffer[receiveBufferPos] = 0;

		if(strcmp("CMD",receiveBuffer)==0){
			mCommandModeEntered = 1;
		}
		else if(strcmp("END",receiveBuffer)==0){
			mCommandModeEntered = 0;
			mResponseReceived = 0;
		}
		else{
			strcpy(mResponse,receiveBuffer);
			mResponseReceived = 1;
		}
		receiveBufferPos = 0;
	}
}
static uint8_t CharacterHexToByte(char high,char low){
	return (CharacterHexToNibble(high) << 4) | CharacterHexToNibble(low);
}

static uint8_t CharacterHexToNibble(char ch){
	if(ch == '0'){
		return 0;
	} else if(ch == '1'){
		return 1;
	} else if(ch == '2'){
		return 2;
	} else if(ch == '3'){
		return 3;
	} else if(ch == '4'){
		return 4;
	} else if(ch == '5'){
		return 5;
	} else if(ch == '6'){
		return 6;
	} else if(ch == '7'){
		return 7;
	} else if(ch == '8'){
		return 8;
	} else if(ch == '9'){
		return 9;
	} else if(ch == 'A'){
		return 10;
	} else if(ch == 'B'){
		return 11;
	} else if(ch == 'C'){
		return 12;
	} else if(ch == 'D'){
		return 13;
	} else if(ch == 'E'){
		return 14;
	} else if(ch == 'F'){
		return 15;
	} else{
		return 0;
	}
	return 0;
}

static uint8_t IsResponseValid(void){
	if(strcmp(mResponse,"?")==0){
		return 0;
	} else if(strcmp(mResponse,"ERR")==0){
		return 0;
	} else{
		return 1;
	}
}
