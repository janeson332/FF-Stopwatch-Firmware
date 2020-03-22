/**
  ******************************************************************************
  * @file    TaskDisplay.c
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    14.02.2020
  * @brief   Handles Display output
  ******************************************************************************
*/

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "TaskDisplay.h"
#include "Utils/Debug.h"
#include "i2c1.h"
#include "LCD_i2c.h"

#include "task.h"
#include "message_buffer.h"

#define LCD_ADDRESS					(0x70>>1)
#define LCD_COLS					16
#define LCD_ROWS       				2
#define LCD_CHAR_SIZE   			LCD_5x8DOTS

#define MESSAGE_BUFFER_SIZE			128

typedef struct{
	uint8_t message[LCD_COLS+1];
	uint32_t col;
	uint32_t row;
}MessageType_t;

static void Display_Init(void);
static void I2CWriteSingleByte(uint8_t addr, uint8_t data);

static StaticTask_t tcbDisplayTask;
static StackType_t stackDisplayTask[TASK_DISPLAY_STACK_SIZE];

static tLCD_InitStruct mLcd;
static uint8_t mStorageBuffer[MESSAGE_BUFFER_SIZE];
static StaticMessageBuffer_t mMessageBufferStruct;
static MessageBufferHandle_t mMessageBuffer = 0;

static MessageType_t mMessageRx;

void TaskDisplay_Init(void){

	xTaskCreateStatic(TaskDisplay,"DisplayTask",TASK_DISPLAY_STACK_SIZE,0,tskIDLE_PRIORITY,
			stackDisplayTask,&tcbDisplayTask);

	// initialize message buffer
	mMessageBuffer = xMessageBufferCreateStatic(sizeof(mStorageBuffer),mStorageBuffer,&mMessageBufferStruct);
	assert(mMessageBuffer != 0);
	Display_Init();

}

void TaskDisplay(void){

	LCD_Init(&mLcd,I2CWriteSingleByte,vTaskDelay);
	DEBUG_LOG("Start Display Task");

	while(1){

		xMessageBufferReceive(mMessageBuffer,(void*)(&mMessageRx),sizeof(MessageType_t),portMAX_DELAY);
		LCD_SetCursor(&mLcd,mMessageRx.col,mMessageRx.row);
		LCD_WriteString(&mLcd,mMessageRx.message);
	}
}

int8_t TaskDisplay_WriteString(char const * const str,uint32_t col, uint32_t row){
	uint32_t length = strlen(str);

	if(length>LCD_COLS){
		DEBUG_LOG("Task Display: written to long string");
		return -1;
	}

	MessageType_t msg;
	memset(&msg,0,sizeof(MessageType_t));

	strncpy(msg.message,str,length);
	msg.col = col;
	msg.row = row;

	taskENTER_CRITICAL();
	xMessageBufferSend(mMessageBuffer,(void*)(&msg),sizeof(MessageType_t),portMAX_DELAY);
	taskEXIT_CRITICAL();

	return 0;
}

void TaskDisplay_PrintTimeString(void){
	TaskDisplay_WriteString("Zeit:      Ready",0,1);
}

void TaskDisplay_FormatAndPrintTime(StopTimeType_t time,char const * preString){

	char buffer[LCD_COLS+1];
	char displayBuffer[LCD_COLS+1];
	memset(displayBuffer,0,sizeof(displayBuffer));

	int32_t preStringLen = strlen(preString);
	int32_t timeLen = snprintf(buffer,LCD_COLS+1,"%d.%02ds",time.seconds,(time.milliseconds/10)%100);
	timeLen = strlen(buffer);

	if(preStringLen+timeLen > 11){
		DEBUG_LOG("Lengths do not fit into display");
		snprintf(displayBuffer,16,"        INF");
	}
	else{
		strncpy(displayBuffer,preString,preStringLen);
		memset(&displayBuffer[preStringLen],' ',11-preStringLen-timeLen);
		strcat(displayBuffer,buffer);
	}

	TaskDisplay_WriteString(displayBuffer,5,1);
}

uint8_t TaskDisplay_GetBacklightVal(){
	taskENTER_CRITICAL();
	uint8_t ret = LCD_GetBacklightState(&mLcd);
	taskEXIT_CRITICAL();
	return ret;
}

void TaskDisplay_SetBacklightVal(uint8_t enable){
	taskENTER_CRITICAL();
	LCD_Backlight(&mLcd,enable);
	taskEXIT_CRITICAL();
}

static void Display_Init(void){
	I2C1_Init(); // i2c lcd display
	mLcd.addr = LCD_ADDRESS;
	mLcd.cols = LCD_COLS;
	mLcd.rows = LCD_ROWS;
	mLcd.charsize = LCD_CHAR_SIZE;

}

static void I2CWriteSingleByte(uint8_t addr, uint8_t data){
	I2C1_WriteRegister(addr,data,0,0);
}
