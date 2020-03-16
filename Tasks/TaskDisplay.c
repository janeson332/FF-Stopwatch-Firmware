/**
  ******************************************************************************
  * @file    TaskDisplay.c
  * @author  Stefan
  * @version V1.0
  * @date    14.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/

#include <assert.h>

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
}tMessage;

static void Display_Init(void);
static void I2CWriteSingleByte(uint8_t addr, uint8_t data);

static tLCD_InitStruct mLcd;
static uint8_t mStorageBuffer[MESSAGE_BUFFER_SIZE];
static StaticMessageBuffer_t mMessageBufferStruct;
static MessageBufferHandle_t mMessageBuffer = 0;;

static tMessage mMessageRx;

void TaskDisplay(void){

	DEBUG_LOG("Start Display Task");

	// initialize message buffer
	mMessageBuffer = xMessageBufferCreateStatic(sizeof(mStorageBuffer),mStorageBuffer,&mMessageBufferStruct);
	assert(mMessageBuffer != 0);

	Display_Init();

	DEBUG_LOG("Display Initialized");
	while(1){

		xMessageBufferReceive(mMessageBuffer,(void*)(&mMessageRx),sizeof(tMessage),portMAX_DELAY);
		LCD_SetCursor(&mLcd,mMessageRx.col,mMessageRx.row);
		LCD_WriteString(&mLcd,mMessageRx.message);
	}
}

int8_t TaskDisplayWriteString(char const * const str,uint32_t col, uint32_t row){
	uint32_t length = strlen(str);

	if(length>LCD_COLS){
		DEBUG_LOG("Task Display: written to long string");
		return -1;
	}

	tMessage msg;
	memset(&msg,0,sizeof(tMessage));

	strncpy(msg.message,str,length);
	msg.col = col;
	msg.row = row;

	taskENTER_CRITICAL();
	xMessageBufferSend(mMessageBuffer,(void*)(&msg),sizeof(tMessage),portMAX_DELAY);
	taskEXIT_CRITICAL();

	return 0;
}

uint8_t TaskDisplayGetBacklightVal(){
	taskENTER_CRITICAL();
	uint8_t ret = LCD_GetBacklightState(&mLcd);
	taskEXIT_CRITICAL();
	return ret;
}

void TaskDisplaySetBacklightVal(uint8_t enable){
	taskENTER_CRITICAL();
	LCD_Backlight(&mLcd,enable);
	taskEXIT_CRITICAL();
}

static void Display_Init(void){
	I2C1_Init();

	mLcd.addr = LCD_ADDRESS;
	mLcd.cols = LCD_COLS;
	mLcd.rows = LCD_ROWS;
	mLcd.charsize = LCD_CHAR_SIZE;

	LCD_Init(&mLcd,I2CWriteSingleByte,vTaskDelay);
}

static void I2CWriteSingleByte(uint8_t addr, uint8_t data){
	I2C1_WriteRegister(addr,data,0,0);
}
