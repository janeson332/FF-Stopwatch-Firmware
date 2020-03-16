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
#include "Buttons.h"
#include "Utils/Debug.h"

#include "task.h"
#include "StopwatchTask.h"


static uint8_t lastButtonState[BUTTONS_NUMBER_MAX];

static TaskHandle_t mEventTaskHandle = 0;

void TaskButton(void){

	DEBUG_LOG("Start Button Task");

	Buttons_Init();
	memset(lastButtonState,0,sizeof(lastButtonState));

	DEBUG_LOG("Buttons Initialized");
	while(1){

		for(uint8_t i=0; i<BUTTONS_NUMBER_MAX;++i){

			uint8_t val = Buttons_GetState((tButtonType)(i));

			// pressed button
			if((lastButtonState[i]==0) && (val !=0)){
				//send button event..
				char tmp[64];
				snprintf(tmp,64,"Button %d pressed",i);
				DEBUG_LOG(tmp);
				lastButtonState[i] = val;
				if(mEventTaskHandle != 0){
					if(i == btn_next){
						xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUTTON_NEXT,eSetBits);
					} else if(i == btn_select){
						xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUTTON_SELECT,eSetBits);
					} else if(i == btn_res){
						xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUTTON_RESERVED,eSetBits);
					} else if(i == buzzer1){
						xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUZZER1_PRESSED,eSetBits);
					} else if(i == buzzer2){
						xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUZZER2_PRESSED,eSetBits);
					}
				}
			}
			// released
			else if (lastButtonState[i]!=0 && val == 0){
				lastButtonState[i] = 0;
				char tmp[64];
				snprintf(tmp,64,"Button %d released\n",i);
				DEBUG_LOG(tmp);
				if(mEventTaskHandle != 0){
					if(i == buzzer1){
						xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUZZER1_RELEASED,eSetBits);
					} else if(i == buzzer2){
						xTaskNotify(mEventTaskHandle,STOPWATCH_TASK_NOTIFY_BUZZER2_RELEASED,eSetBits);
					}
				}
			}

		}

		vTaskDelay(5);
	}
}

void TaskButton_SetRxEventTask(TaskHandle_t handle){
	mEventTaskHandle = handle;
}
