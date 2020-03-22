/**
  ******************************************************************************
  * @file    RN52.h
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    21.02.2020
  * @brief   RN52 Bluetooth modul interface
  ******************************************************************************
*/
#ifndef RN52_H_INCLUDED
#define RN52_H_INCLUDED

#include "stm32f4xx.h"

typedef enum{
	RN52_OK = 0, /**< all went well*/
	RN52_NOK,    /**< same as ok, eg. return on device not connected*/
	RN52_Error,  /**< some error occured */
	RN52_Timeout /**< timeout occured */
}RN52RetType_t;

/**
 * @brief Initializes RN52 bluetooth device
 * @param getTick: Function pointer which returns actual tick value (ms)
 */
void RN52_Init(uint32_t (*getTick)());

/**
 * @brief sends given string on rn52 uart (spp)
 */
void RN52_SendString(char const *str);

/**
 * @brief send given char on rn52 uart
 */
void RN52_SendChar(uint8_t ch);

/**
 * @brief sets a callback on receiving a character (in ISR called)
 */
void RN52_SetReceiveCB(void (*receiveCB)(uint8_t ch));

/**
 * @brief enables disables RN52 bluetooth module
 */
void RN52_PowerEnable(FunctionalState state);

/**
 * @brief returns state of rn52
 */
FunctionalState RN52_Enabled(void);

/**
 * @brief returns RN52_OK when spp device is connected, else other return type
 */
RN52RetType_t RN52_SPPDeviceConnected(void);


#endif /* RN52_H_INCLUDED */
