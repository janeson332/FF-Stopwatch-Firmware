/**
  ******************************************************************************
  * @file    BluetoothTask.h
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    21.03.2020
  * @brief   Task for handling bluetooth communication
  ******************************************************************************
*/

#ifndef BLUETOOTHTASK_H_INCLUDED
#define BLUETOOTHTASK_H_INCLUDED

#include "FreeRTOS.h"
#include "Buttons.h"

#define BLUETOOTH_TASK_STACK_SIZE                512

/**
 * @brief initializes resources for the bluetooth task
 */
void BluetoothTask_Init(void);

/**
 * @brief task..
 */
void BluetoothTask(void);

/**
 * @brief returns true if remote device has been connected and enabled remote mode
 */
uint8_t BluetoothTask_GetRemoteState(void);

/**
 * @brief sends command/msg of the given buzzer
 */
void BluetoothTask_SendBuzzerMsg(ButtonType_t buzzer,uint32_t value);


#endif /* BLUETOOTHTASK_H_INCLUDED */
