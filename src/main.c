/**
  ******************************************************************************
  * @file    main.c
  * @author  Janeson332
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

// Test Task Settings (Task Control Block and Stack)
#define TEST_TASK_STACK_SIZE 512
StaticTask_t tcbTestTask;
StackType_t stackTestTask[TEST_TASK_STACK_SIZE];

StaticTask_t tcbTestTask1;
StackType_t stackTestTask1[TEST_TASK_STACK_SIZE];

StaticTask_t tcbButtonTask;
StackType_t  stackButtonTask[TASK_BUTTON_STACK_SIZE];
			
StaticTask_t tcbDisplayTask;
StackType_t stackDisplayTask[TASK_DISPLAY_STACK_SIZE];

StaticTask_t tcbStopwatchTask;
StackType_t stackStopwatchTask[STOPWATCH_TASK_STACK_SIZE];

StaticTask_t tcbBluetoothTask;
StackType_t stackBluetoothTask[BLUETOOTH_TASK_STACK_SIZE];
// simple test task

//static void I2CWriteSingleByte(uint8_t addr, uint8_t data){
//	I2C1_WriteRegister(addr,data,0,0);
//}

/*
tLCD_InitStruct lcd;
void myTestTask(void * parameters){
	memset(&lcd,0,sizeof(tLCD_InitStruct));
    I2C1_Init();
    lcd.addr = (0x70>>1);
    lcd.cols = 16;
    lcd.rows = 2;
    lcd.charsize = LCD_5x8DOTS;

    int x = 0;
    LCD_Init(&lcd,I2CWriteSingleByte,vTaskDelay);


    while(1){

    	char tmp[20];
    	snprintf(tmp,20,"%d",x);
    	LCD_SetCursor(&lcd,0,0);
    	LCD_WriteString(&lcd,tmp);
    	Debug_Write(tmp); Debug_Write("\n");
    	vTaskDelay(500);
    	++x;
    }
}
*/

// simple test task
void myTestTask2(void * parameters){

	RN52_Init(xTaskGetTickCount);
//	RN52_PowerEnable(ENABLE);
//	vTaskDelay(2000);
//	RN52_PowerEnable(DISABLE);

//	vTaskDelay(2000);
	RN52_PowerEnable(ENABLE);
	UART2_SetReceiveParser(RN52_SendChar);
	RN52_SetReceiveCB(UART2_SendChar);

	pdMS_TO_TICKS(100);
    while(1){
    	static uint8_t test = 0;
    	uint32_t notificationValue=0;
    	xTaskNotifyWait(0,STOPWATCH_TASK_NOTIFY_BUTTON_NEXT | STOPWATCH_TASK_NOTIFY_BUTTON_SELECT,&notificationValue,portMAX_DELAY);

    	if(notificationValue & STOPWATCH_TASK_NOTIFY_BUTTON_SELECT){
    		DEBUG_LOG("CHECK_CONNECTION");
			RN52RetType_t ret = RN52_DeviceConnected();

			if(ret==RN52_OK){
				DEBUG_LOG("DeviceConnected");
			}else if(ret==RN52_NOK){
				DEBUG_LOG("Device Disconnected");
			}else{
				DEBUG_LOG("Timeout or Error");
			}
    	} else{
    		DEBUG_LOG("ENABLE/DISABLE");
    		if(test==0){
				RN52_PowerEnable(ENABLE);
				test = 1;
			}else{
				RN52_PowerEnable(DISABLE);
				test = 0;
			}
    	}

    }
}

int main(void){
	Debug_Init();
	RN52_Init(xTaskGetTickCount);
	RN52_PowerEnable(ENABLE);
	DEBUG_LOG("Start Program");


	xTaskCreateStatic(TaskDisplay,"DisplayTask",TASK_DISPLAY_STACK_SIZE,0,tskIDLE_PRIORITY,stackDisplayTask,&tcbDisplayTask);
	TaskHandle_t stopwatchHandle =
			xTaskCreateStatic(StopwatchTask,"StopwatchTask",STOPWATCH_TASK_STACK_SIZE,0,3,stackStopwatchTask,&tcbStopwatchTask);

	TaskButton_SetRxEventTask(stopwatchHandle);

	xTaskCreateStatic(TaskButton,"ButtonTask",TASK_BUTTON_STACK_SIZE,0,tskIDLE_PRIORITY,stackButtonTask,&tcbButtonTask);

	xTaskCreateStatic(BluetoothTask,"BluetoothTask",BLUETOOTH_TASK_STACK_SIZE,0,tskIDLE_PRIORITY,stackBluetoothTask,&tcbBluetoothTask);



	//TaskHandle_t taskHdl = xTaskCreateStatic(myTestTask2,"RN52",TEST_TASK_STACK_SIZE,0,tskIDLE_PRIORITY,stackTestTask,&tcbTestTask);
	//TaskButton_SetRxEventTask(taskHdl);
	//xTaskCreateStatic(TaskButton,"ButtonTask",TASK_BUTTON_STACK_SIZE,0,tskIDLE_PRIORITY,stackButtonTask,&tcbButtonTask);
    vTaskStartScheduler();

    while(1);
}


