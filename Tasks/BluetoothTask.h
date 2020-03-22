/**
  ******************************************************************************
  * @file    BluetoothTask.h
  * @author  Stefan
  * @version V1.0
  * @date    24.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/
#ifndef BLUETOOTHTASK_H_INCLUDED
#define BLUETOOTHTASK_H_INCLUDED


#include "FreeRTOS.h"
#include "Buttons.h"

#define BLUETOOTH_TASK_STACK_SIZE                512

void BluetoothTask(void);
uint8_t BluetoothTask_GetRemoteState(void);

void BluetoothTask_SendBuzzerMsg(ButtonType_t buzzer,uint32_t value);


#endif /* BLUETOOTHTASK_H_INCLUDED */
