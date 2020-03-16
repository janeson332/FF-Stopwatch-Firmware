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

#define BLUETOOTH_TASK_STACK_SIZE                512

void BluetoothTask(void);

void BluetoothReceiveCB(uint8_t ch);


#endif /* BLUETOOTHTASK_H_INCLUDED */
