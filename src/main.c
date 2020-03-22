/**
  ******************************************************************************
  * @file    main.c
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    14.02.2020
  * @brief   main function
  ******************************************************************************
*/


// FreeRTOS Kernel includes..
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "Tasks/TaskButton.h"
#include "Tasks/TaskDisplay.h"
#include "Tasks/StopwatchTask.h"
#include "Tasks/BluetoothTask.h"

#include "i2c1.h"
#include "LCD_i2c.h"

#include "stm32f4xx.h"
#include "Utils/Debug.h"

#include "RN52.h"
#include "stm32f40x_uart2.h"


void InitializeHardware(void){
	Debug_Init();
	Buttons_Init();
	//Display and i2c are initialized in the display task, since the use OS Functions
	Stopwatch_Init();
	RN52_Init(xTaskGetTickCount);
	RN52_PowerEnable(ENABLE);
}

int main(void){

	InitializeHardware();
	DEBUG_LOG("Hardware initialized");

	DEBUG_LOG("Init Tasks and Ressources");
	TaskDisplay_Init();
	StopwatchTask_Init();
	TaskButton_Init();
	BluetoothTask_Init();

	TaskButton_SetRxEventTask(StopwatchTask_GetTaskHandle());

	DEBUG_LOG("Initialization finished.. start Scheduler");
    vTaskStartScheduler();

    while(1);
}


