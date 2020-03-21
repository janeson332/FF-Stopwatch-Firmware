/**
  ******************************************************************************
  * @file    StopwatchTask.c
  * @author  Stefan
  * @version V1.0
  * @date    18.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/


#include "StopwatchTask.h"
#include "Stopwatch.h"
#include "TaskDisplay.h"
#include "Utils/Debug.h"
#include "Utils/StopwatchModes.h"
#include "BluetoothTask.h"

#include "task.h"

typedef enum {
	MainHandler,
	ModeHandler
}MainHandlerMode_t;

static MainHandlerMode_t mHandlerState = MainHandler;
static ModeHandleTableType_t* mModeHndlTable = 0;
static uint32_t          mCurrentModeCounter = 0;

void StopwatchTask(void){

	DEBUG_LOG("Start Stopwatch Task");
	Stopwatch_Init();
	mModeHndlTable = StopwatchModes_GetModeTable();

	uint32_t taskNotificationValue = 0;

	vTaskDelay(1000); // wait until display task has been finished
	TaskDisplay_WriteString("Welcome         ",0,0);
	TaskDisplay_WriteString("                ",0,1);
	vTaskDelay(1000); // wait until display task has been finished



	TaskDisplay_WriteString(mModeHndlTable[mCurrentModeCounter].displayText,0,1);
	TaskDisplay_WriteString("Menu            ",0,0);

	DEBUG_LOG("Stopwatch Task initialized");
	while(1){


		if(((mHandlerState == MainHandler) || mCurrentModeCounter <= mode_None ) && BluetoothTask_GetRemoteState()){
			mCurrentModeCounter = mode_Remote;
			mHandlerState = ModeHandler;
			StopwatchMode_Reset();
		}

		if(mHandlerState == MainHandler){
			xTaskNotifyWait(STOPWATCH_TASK_NOTIFY_WAIT,STOPWATCH_TASK_NOTIFY_WAIT,&taskNotificationValue,200);

			if((STOPWATCH_TASK_NOTIFY_BUTTON_NEXT & taskNotificationValue) != 0){
				DEBUG_LOG("Button Next");
				mCurrentModeCounter++;
				if(mModeHndlTable[mCurrentModeCounter].ModeFunction==0){
					mCurrentModeCounter = 0;
				}
				TaskDisplay_WriteString(mModeHndlTable[mCurrentModeCounter].displayText,0,1);
				TaskDisplay_WriteString("Menu            ",0,0);
			}

			if((STOPWATCH_TASK_NOTIFY_BUTTON_SELECT & taskNotificationValue) != 0 ){
				mHandlerState = ModeHandler;
				DEBUG_LOG("Button Select");
			}


		}else if(mHandlerState == ModeHandler){

			 StopwatchModeRetType_t ret = mModeHndlTable[mCurrentModeCounter].ModeFunction();

			if(ret == StopwatchMode_ReturnToMain){
				mHandlerState = MainHandler;
				if(mCurrentModeCounter>=mode_None){
					mCurrentModeCounter = mode_SingleStop;
				}
				TaskDisplay_WriteString(mModeHndlTable[mCurrentModeCounter].displayText,0,1);
				TaskDisplay_WriteString("Menu            ",0,0);
			} else if(ret == StopwatchMode_Error){
				mHandlerState = MainHandler;
				if(mCurrentModeCounter>=mode_None){
					mCurrentModeCounter = mode_SingleStop;
				}
				DEBUG_LOG("ModeHandler returned Error");
			}

			vTaskDelay(10);
		}

	}
}
