/**
  ******************************************************************************
  * @file    TaskButton.c
  * @author  Stefan
  * @version V1.0
  * @date    14.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/

#include "TaskButton.h"

#include "Utils/Debug.h"

#include "task.h"
#include "StopwatchTask.h"


static uint8_t lastButtonTaskEventState[BUTTONS_NUMBER_MAX];

static TaskHandle_t mEventTaskHandle = 0;

static void HandleTaskEvent(ButtonType_t button);

void TaskButton(void){

	DEBUG_LOG("Start Button Task");

	Buttons_Init();
	memset(lastButtonTaskEventState,0,sizeof(lastButtonTaskEventState));

	DEBUG_LOG("Buttons Initialized");
	while(1){

		for(uint8_t i=0; i<BUTTONS_NUMBER_MAX;++i){
			HandleTaskEvent((ButtonType_t)(i));


		}

		vTaskDelay(5);
	}
}

void TaskButton_SetRxEventTask(TaskHandle_t handle){
	mEventTaskHandle = handle;
}


static void HandleTaskEvent(ButtonType_t button){

	uint8_t val = Buttons_GetState(button);
	// pressed button
	if((lastButtonTaskEventState[button]==0) && (val !=0)){
		//send button event..
		lastButtonTaskEventState[button] = val;

		// send task notification
		if(mEventTaskHandle != 0){
			if(button == btn_next){
				xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUTTON_NEXT,eSetBits);
			} else if(button == btn_select){
				xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUTTON_SELECT,eSetBits);
			} else if(button == btn_res){
				xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUTTON_RESERVED,eSetBits);
			} else if(button == buzzer1){
				xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUZZER1_PRESSED,eSetBits);
			} else if(button == buzzer2){
				xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUZZER2_PRESSED,eSetBits);
			}
		}
	}

	// button release
	else if (lastButtonTaskEventState[button]!=0 && val == 0){
		lastButtonTaskEventState[button] = 0;
		if(mEventTaskHandle != 0){
			if(button == buzzer1){
				xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUZZER1_RELEASED,eSetBits);
			} else if(button == buzzer2){
				xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUZZER2_RELEASED,eSetBits);
			}
		}
	}
}
