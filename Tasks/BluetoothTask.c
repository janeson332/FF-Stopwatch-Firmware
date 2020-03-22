/**
  ******************************************************************************
  * @file    BluetoothTask.c
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    21.03.2020
  * @brief   Task for handling bluetooth communication
  ******************************************************************************
*/

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "BluetoothTask.h"
#include "StopwatchTask.h"
#include "TaskDisplay.h"
#include "Stopwatch.h"
#include "RN52.h"
#include "Utils/Debug.h"
#include "Utils/BluetoothCommands.h"

#include "message_buffer.h"
#include "task.h"

#define MAX_MESSAGE_SIZE            128
#define CHECK_CONNECTED_INTERVALL   pdMS_TO_TICKS(1000)


StaticTask_t tcbBluetoothTask;
StackType_t stackBluetoothTask[BLUETOOTH_TASK_STACK_SIZE];

static char mStorageBuffer[MAX_MESSAGE_SIZE];
static StaticMessageBuffer_t mMessageBufferStruct;
static MessageBufferHandle_t mMessageBuffer = 0;
static char mMessageRx[MAX_MESSAGE_SIZE];

static uint8_t bluetoothRemoteOn = 0;

static void BluetoothReceiveCB(uint8_t ch);
static void ParseCommand(char const * str);
static uint8_t SetRemoteState(uint8_t val);


void BluetoothTask_Init(void){
	xTaskCreateStatic(BluetoothTask,"BluetoothTask",BLUETOOTH_TASK_STACK_SIZE,0,tskIDLE_PRIORITY,
			stackBluetoothTask,&tcbBluetoothTask);

	mMessageBuffer = xMessageBufferCreateStatic(sizeof(mStorageBuffer),mStorageBuffer,&mMessageBufferStruct);
	assert(mMessageBuffer != 0);

	RN52_SetReceiveCB(BluetoothReceiveCB);
}

uint8_t BluetoothTask_GetRemoteState(void){
	uint8_t val = 0;

	taskENTER_CRITICAL();
	val = bluetoothRemoteOn;
	taskEXIT_CRITICAL();
	return val;
}

void BluetoothTask_SendBuzzerMsg(ButtonType_t buzzer,uint32_t value){
	taskENTER_CRITICAL();
	if(buzzer == buzzer1){
		if(value != 0){
			RN52_SendString(BT_COMMAND_BUZZER1 BT_BUZZER_PRESSED "\n");
		}
		else{
			RN52_SendString(BT_COMMAND_BUZZER1 BT_BUZZER_RELEASED "\n");
		}
	}
	else if(buzzer == buzzer2){
		if(value != 0){
			RN52_SendString(BT_COMMAND_BUZZER2 BT_BUZZER_PRESSED "\n");
		}
		else{
			RN52_SendString(BT_COMMAND_BUZZER2 BT_BUZZER_RELEASED "\n");
		}
	}

	taskEXIT_CRITICAL();
}

void BluetoothTask(void){

	DEBUG_LOG("Start Bluetooth Task");

	while(1){

		uint32_t numBytes = xMessageBufferReceive(mMessageBuffer,(void*)(&mMessageRx),sizeof(mStorageBuffer),CHECK_CONNECTED_INTERVALL);

		if(numBytes != 0){
			// parse command
			DEBUG_LOG(mMessageRx);
			ParseCommand(mMessageRx);
		}
		else{
			if(RN52_Enabled()==ENABLE && (BluetoothTask_GetRemoteState()==1)){
				RN52RetType_t ret = RN52_SPPDeviceConnected();
				if(ret == RN52_OK){
					DEBUG_LOG("DeviceConnected");
				}
				else if(ret == RN52_NOK){
					DEBUG_LOG("No Device Connected, reset remote state");
					SetRemoteState(0);
				}
				else if(ret == RN52_Timeout){
					DEBUG_LOG("RN52_Timeout");
				}
				else{
					DEBUG_LOG("RN52 Error occured");
				}

			}
		}
	}
}


static void ParseCommand(char const * str){

	// parse commands in usage order
	if(strncmp(BT_COMMAND_UPDATE_TIME,str,strlen(BT_COMMAND_UPDATE_TIME))==0){
		// fetch time from string
		uint32_t timeMs = strtol(&str[strlen(BT_COMMAND_UPDATE_TIME)],0,10);
		StopTimeType_t time;
		time.seconds = timeMs / 1000;
		time.milliseconds = timeMs % 1000;
		TaskDisplay_FormatAndPrintTime(time,"");
		DEBUG_LOG("UpdateTime");
	}

	else if(strncmp(BT_COMMAND_SET_TIME,str,strlen(BT_COMMAND_SET_TIME))==0){
		uint32_t timeMs = strtol(&str[strlen(BT_COMMAND_SET_TIME)],0,10);
		StopTimeType_t time;
		time.seconds = timeMs / 1000;
		time.milliseconds = timeMs % 1000;
		TaskDisplay_FormatAndPrintTime(time,"");
		DEBUG_LOG("SetTime");
		RN52_SendString(BT_COMMAND_ACK "\n");
	}

	else if(strncmp(BT_COMMAND_RESET_TIME,str,strlen(BT_COMMAND_RESET_TIME))==0){
		TaskDisplay_PrintTimeString();
		DEBUG_LOG("ResetTime");
		RN52_SendString(BT_COMMAND_ACK "\n");
	}

	else if(strncmp(BT_COMMAND_REMOTE,str,strlen(BT_COMMAND_REMOTE))==0){

		// check if on or off
		if(strncmp(BT_VALUE_ON,&str[strlen(BT_COMMAND_REMOTE)],strlen(BT_VALUE_ON))==0){
			DEBUG_LOG("RemoteEnable");

			if(SetRemoteState(1)){
				RN52_SendString(BT_COMMAND_NACK "\n");
			}
			else{
				RN52_SendString(BT_COMMAND_ACK "\n");
			}

		}
		else if(strncmp(BT_VALUE_OFF,&str[strlen(BT_COMMAND_REMOTE)],strlen(BT_VALUE_OFF))==0){
			DEBUG_LOG("RemoteDisable");
			if(SetRemoteState(0)){
				RN52_SendString(BT_COMMAND_NACK "\n");
			}
			else{
				RN52_SendString(BT_COMMAND_ACK "\n");
			}
		}
		else{
			DEBUG_LOG("Remote Invalid Data");
			RN52_SendString(BT_COMMAND_NACK "\n");
		}
	}
	else{
		RN52_SendString(BT_COMMAND_ERROR "\n");
		DEBUG_LOG("Invalid Command");
	}
}


/**
 * @brief isr
 * @TODO add timeout
 */
static void BluetoothReceiveCB(uint8_t ch){
	static uint8_t rxBuffer[MAX_MESSAGE_SIZE];
	static uint32_t rxBufferIdx = 0;

	if(rxBufferIdx < MAX_MESSAGE_SIZE){
		if(ch == '\n'){
			rxBuffer[rxBufferIdx]=0; // terminate string
			rxBufferIdx++;
			xMessageBufferSendFromISR(mMessageBuffer,(void*)(rxBuffer),rxBufferIdx,portMAX_DELAY);
			rxBufferIdx = 0;
		}
		else{
			rxBuffer[rxBufferIdx]=ch;
			rxBufferIdx++;
		}
	}
	else{
		// error string is too long
		rxBufferIdx = 0;
	}
}



static uint8_t SetRemoteState(uint8_t val){
	uint8_t ret = 0;
	taskENTER_CRITICAL();
	if(val == bluetoothRemoteOn){
		ret = 1;
	}
	else{
		bluetoothRemoteOn = val;
		ret = 0;
	}
	taskEXIT_CRITICAL();
	return ret;
}
