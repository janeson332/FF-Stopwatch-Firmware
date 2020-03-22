/**
  ******************************************************************************
  * @file    StopwatchTask.c
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    18.02.2020
  * @brief   Main Task for stopwatch functionality
  ******************************************************************************
*/


#include "StopwatchTask.h"
#include "TaskDisplay.h"
#include "BluetoothTask.h"
#include "Stopwatch.h"
#include "Utils/Debug.h"
#include "Utils/StopwatchModes.h"


typedef enum {
	MainHandler,	/*< main menu handler */
	ModeHandler     /*< sub menu handler */
}MainHandlerMode_t;

static StaticTask_t tcbStopwatchTask;
static StackType_t stackStopwatchTask[STOPWATCH_TASK_STACK_SIZE];
static TaskHandle_t mTaskHandle = 0;

static MainHandlerMode_t mHandlerState = MainHandler;
static ModeHandleTableType_t const * mModeHndlTable = 0;
static uint32_t          mCurrentModeCounter = 0;

void StopwatchTask_Init(void){
	mTaskHandle =
			xTaskCreateStatic(StopwatchTask,"StopwatchTask",STOPWATCH_TASK_STACK_SIZE,0,3,
					stackStopwatchTask,&tcbStopwatchTask);

	mModeHndlTable = StopwatchModes_GetModeTable();
}

TaskHandle_t StopwatchTask_GetTaskHandle(void){
	return mTaskHandle;
}

void StopwatchTask(void){

	DEBUG_LOG("Start Stopwatch Task");

	uint32_t taskNotificationValue = 0;

	TaskDisplay_WriteString("Welcome         ",0,0);
	TaskDisplay_WriteString("                ",0,1);

	vTaskDelay(1000); // Welcome string time


	TaskDisplay_WriteString(mModeHndlTable[mCurrentModeCounter].displayText,0,1);
	TaskDisplay_WriteString("Menu            ",0,0);

	while(1){

		// handle remote functionality
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
