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

#include "task.h"

static tModeHandleTable* mModeHndlTable = 0;
static uint32_t          mCurrentModeCounter = 0;

void StopwatchTask(void){

	DEBUG_LOG("Start Stopwatch Task");
	Stopwatch_Init();
	mModeHndlTable = StopwatchModes_GetModeTable();

	uint32_t taskNotificationValue = 0;

	vTaskDelay(1000); // wait until display task has been finished
	TaskDisplayWriteString("Welcome         ",0,0);
	TaskDisplayWriteString("                ",0,1);
	vTaskDelay(1000); // wait until display task has been finished



	DEBUG_LOG("Stopwatch Task initialized");
	while(1){


		if((STOPWATCH_TASK_NOTIFY_BUTTON_NEXT & taskNotificationValue) != 0){
			DEBUG_LOG("Button Next");
			mCurrentModeCounter++;
			if(mModeHndlTable[mCurrentModeCounter].ModeFunction==0){
				mCurrentModeCounter = 0;
			}
		}

		if((STOPWATCH_TASK_NOTIFY_BUTTON_SELECT & taskNotificationValue) != 0 ){
			mModeHndlTable[mCurrentModeCounter].ModeFunction();
			DEBUG_LOG("Button Select");
		}

		if((STOPWATCH_TASK_NOTIFY_BUTTON_RESERVED & taskNotificationValue) != 0 ){
			DEBUG_LOG("Button Reserved");
		}

		if((STOPWATCH_TASK_NOTIFY_BUZZER1_PRESSED & taskNotificationValue) != 0 ){
			DEBUG_LOG("Buzzer1 Pressed");
		}

		if((STOPWATCH_TASK_NOTIFY_BUZZER1_RELEASED & taskNotificationValue) != 0 ){
			DEBUG_LOG("Buzzer1 Released");
		}

		if((STOPWATCH_TASK_NOTIFY_BUZZER2_PRESSED & taskNotificationValue) != 0 ){
			DEBUG_LOG("Buzzer2 Pressed");
		}

		if((STOPWATCH_TASK_NOTIFY_BUZZER2_RELEASED & taskNotificationValue) != 0 ){
			DEBUG_LOG("Buzzer2 Released");
		}

		TaskDisplayWriteString(mModeHndlTable[mCurrentModeCounter].displayText,0,1);


		TaskDisplayWriteString("Menu            ",0,0);
		DEBUG_LOG("Wait for Event");
		xTaskNotifyWait(0,STOPWATCH_TASK_NOTIFY_WAIT,&taskNotificationValue,portMAX_DELAY);
	}
}
