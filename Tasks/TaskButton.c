/**
  ******************************************************************************
  * @file    TaskButton.c
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    14.02.2020
  * @brief   Task for handling button input
  ******************************************************************************
*/

#include <string.h>

#include "TaskButton.h"
#include "StopwatchTask.h"
#include "Buttons.h"
#include "Utils/Debug.h"

static StaticTask_t tcbButtonTask;
static StackType_t  stackButtonTask[TASK_BUTTON_STACK_SIZE];

static uint8_t lastButtonTaskEventState[BUTTONS_NUMBER_MAX];
static TaskHandle_t mEventTaskHandle = 0;

static void HandleTaskEvent(ButtonType_t button);

void TaskButton_Init(void){
	memset(lastButtonTaskEventState,0,sizeof(lastButtonTaskEventState));
	xTaskCreateStatic(TaskButton,"ButtonTask",TASK_BUTTON_STACK_SIZE,0,tskIDLE_PRIORITY,
			stackButtonTask,&tcbButtonTask);

}

void TaskButton(void){

	DEBUG_LOG("Start TaskButton");

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
				DEBUG_LOG("Buzzer1 Pressed");
			} else if(button == buzzer2){
				xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUZZER2_PRESSED,eSetBits);
				DEBUG_LOG("Buzzer2 Pressed");
			}
		}
	}

	// button release
	else if ((lastButtonTaskEventState[button] != 0) && (val == 0)){
		lastButtonTaskEventState[button] = 0;
		if(mEventTaskHandle != 0){
			if(button == buzzer1){
				xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUZZER1_RELEASED,eSetBits);
				DEBUG_LOG("Buzzer1 Released");
			} else if(button == buzzer2){
				xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUZZER2_RELEASED,eSetBits);
				DEBUG_LOG("Buzzer2 Released");
			}
		}
	}
}
