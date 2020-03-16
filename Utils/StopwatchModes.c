/**
  ******************************************************************************
  * @file    StopwatchModes.c
  * @author  Stefan
  * @version V1.0
  * @date    19.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/



#include "StopwatchModes.h"
#include "Tasks/TaskDisplay.h"
#include "Tasks/StopwatchTask.h"
#include "task.h"
#include "timers.h"
#include "Stopwatch.h"
#include "Utils/Debug.h"
#include "RN52.h"

#define STOPWATCH_BUZZER_EVENT     (STOPWATCH_TASK_NOTIFY_BUZZER1_PRESSED | STOPWATCH_TASK_NOTIFY_BUZZER2_PRESSED)
#define STOPWATCH_BUZZER_RELEASE   (STOPWATCH_TASK_NOTIFY_BUZZER1_RELEASED | STOPWATCH_TASK_NOTIFY_BUZZER2_RELEASED)

static const tModeHandleTable modeHandleTable[] = {
		{StopwatchMode_SingleStop, "Loeschangriff   "},
		{StopwatchMode_DualStop, "Staffellauf     "},
		{StopwatchMode_Settings, "Einstellungen   "},
		{0,0}, //signalizes end of user selectable modes
		{StopwatchMode_Remote,   "Remote Mode     "}
};

static tTimeStruct dualStopTimes[2];
static uint32_t dualStopTimeIdx = 0;


void FormatAndPrintTime(tTimeStruct time,char const * preString);
void PrintDualStopResults( TimerHandle_t xTimer );
void BuzzerReset(void);
uint32_t WaitForEvent(uint32_t event, TickType_t xTicksToWait, BaseType_t resetFlag);

tModeHandleTable const * StopwatchModes_GetModeTable(){
	return modeHandleTable;
}

int StopwatchMode_SingleStop(void){
	TaskDisplayWriteString(modeHandleTable[mode_SingleStop].displayText,0,0);
	TaskDisplayWriteString("Zeit:           ",0,1);

	uint32_t taskNotificationValue=0;
	tTimeStruct time;

	while(1){
		TaskDisplayWriteString("      Ready",5,1);
		Stopwatch_Reset();

		taskNotificationValue = WaitForEvent(STOPWATCH_TASK_NOTIFY_BUTTON_SELECT | STOPWATCH_BUZZER_EVENT,portMAX_DELAY,pdTRUE);
		if(taskNotificationValue & STOPWATCH_TASK_NOTIFY_BUTTON_SELECT){
			DEBUG_LOG("Return to mode handler");
			return 0;
		}
		else if(taskNotificationValue & STOPWATCH_BUZZER_EVENT){
			taskNotificationValue = 0;
			DEBUG_LOG("Start stopping time");
			Stopwatch_StartContinue();
			do{
				taskNotificationValue = WaitForEvent(STOPWATCH_BUZZER_EVENT,31,pdTRUE);
				Stopwatch_GetTime(&time);
				FormatAndPrintTime(time,"");
			}while(taskNotificationValue == 0);

			Stopwatch_Stop(&time);
			FormatAndPrintTime(time,"");
			DEBUG_LOG("Time Stopped");

			BuzzerReset();
		}
	}

	return 1;
}


void PrintDualStopResults( TimerHandle_t xTimer ){
	DEBUG_LOG("Entered");
	if(dualStopTimeIdx == 0){
		FormatAndPrintTime(dualStopTimes[dualStopTimeIdx],"P1");
		dualStopTimeIdx = 1;
	} else{
		FormatAndPrintTime(dualStopTimes[dualStopTimeIdx],"P2");
		dualStopTimeIdx = 0;
	}
}


int StopwatchMode_DualStop(void){

	TaskDisplayWriteString(modeHandleTable[mode_DualStop].displayText,0,0);
	TaskDisplayWriteString("Zeit:           ",0,1);

	uint32_t taskNotificationValue=0;
	tTimeStruct actualTime;
	StaticTimer_t tim;
	TimerHandle_t timer;

	while(1){
		TaskDisplayWriteString("      Ready",5,1);
		Stopwatch_Reset();
		dualStopTimeIdx = 1;
		memset(&dualStopTimes,0,sizeof(dualStopTimes));

		taskNotificationValue = WaitForEvent(STOPWATCH_TASK_NOTIFY_BUTTON_SELECT | STOPWATCH_BUZZER_EVENT, portMAX_DELAY,pdTRUE);
		if(taskNotificationValue & STOPWATCH_TASK_NOTIFY_BUTTON_SELECT){
			DEBUG_LOG("Return to mode handler");
			return 0;
		}
		else if(taskNotificationValue & STOPWATCH_BUZZER_EVENT){
			taskNotificationValue = 0;
			DEBUG_LOG("Start stopping time");
			Stopwatch_StartContinue();

			do{
				taskNotificationValue = WaitForEvent(STOPWATCH_BUZZER_EVENT,31,pdTRUE);
				Stopwatch_GetTime(&actualTime);
				FormatAndPrintTime(actualTime,"");
				if((dualStopTimes[0].milliseconds == 0) && (taskNotificationValue & STOPWATCH_TASK_NOTIFY_BUZZER1_PRESSED)){
					DEBUG_LOG("Time0 captured");
					memcpy(&(dualStopTimes[0]),&actualTime,sizeof(tTimeStruct));
				}
				if((dualStopTimes[1].milliseconds == 0) && (taskNotificationValue & STOPWATCH_TASK_NOTIFY_BUZZER2_PRESSED)){
					DEBUG_LOG("Time1 captured");
					memcpy(&(dualStopTimes[1]),&actualTime,sizeof(tTimeStruct));
				}
			}while((dualStopTimes[0].milliseconds == 0) || (dualStopTimes[1].milliseconds == 0));

			DEBUG_LOG("Time Stopped");
			Stopwatch_Stop(&actualTime);
			FormatAndPrintTime(dualStopTimes[0],"");

			timer = xTimerCreateStatic("ToggleTim",pdMS_TO_TICKS(1000),pdTRUE,0,PrintDualStopResults,&tim);
			xTimerStart(timer,0);

			//Buzzer Reset..
			BuzzerReset();

			xTimerStop(timer,5000);
			xTimerReset(timer,0);
			xTimerDelete(timer,0);

		}
	}

	return 1;
}

int StopwatchMode_Remote(void){
	return 1;
}

int StopwatchMode_Settings(void){
	TaskDisplayWriteString(modeHandleTable[mode_Settings].displayText,0,0);
	typedef enum {
		SetBacklight=0,
		EnableBluetooth=1,
		Back=2
	}tSettings;

	tSettings settings = SetBacklight;
	char buffer[16+1];

	uint32_t taskNotificationValue=0;

	while(1){

		switch(settings){
		case SetBacklight:{

			TaskDisplayWriteString("Display:        ",0,1);
			if(TaskDisplayGetBacklightVal()){
				TaskDisplayWriteString(" ON",13,1);
			}
			else{
				TaskDisplayWriteString("OFF",13,1);
			}

		}break;
		case EnableBluetooth:{
			TaskDisplayWriteString("Bluetooth:      ",0,1);
			if(RN52_Enabled()==ENABLE){
				TaskDisplayWriteString(" ON",13,1);
			}
			else{
				TaskDisplayWriteString("OFF",13,1);
			}
		}break;
		case Back:{
			TaskDisplayWriteString("Back...         ",0,1);
		}break;
		}

		taskNotificationValue = WaitForEvent(STOPWATCH_TASK_NOTIFY_BUTTON_SELECT | STOPWATCH_TASK_NOTIFY_BUTTON_NEXT, portMAX_DELAY,pdTRUE);
		if(taskNotificationValue & STOPWATCH_TASK_NOTIFY_BUTTON_SELECT){
			switch(settings){
			case SetBacklight:{
				TaskDisplaySetBacklightVal(TaskDisplayGetBacklightVal()==0?1:0);
			}break;
			case EnableBluetooth:{
				RN52_PowerEnable((RN52_Enabled()==ENABLE)?DISABLE:ENABLE);
			}break;
			case Back:{
				return 1;
			}break;
			}
		}
		else if(taskNotificationValue & STOPWATCH_TASK_NOTIFY_BUTTON_NEXT){
			if(settings<2){
				settings++;
			}else{
				settings = 0;
			}
		}
	}
}


void FormatAndPrintTime(tTimeStruct time,char const * preString){
	char buffer[16+1];
	char displayBuffer[16+1];
	memset(displayBuffer,0,sizeof(displayBuffer));

	int32_t preStringLen = strlen(preString);
	int32_t timeLen = snprintf(buffer,16+1,"%d.%02ds",time.seconds,(time.milliseconds/10)%100);
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

	TaskDisplayWriteString(displayBuffer,5,1);
}

void BuzzerReset(void){
	BaseType_t ret = pdTRUE;

	do{
		DEBUG_LOG("Retry");
		WaitForEvent(STOPWATCH_BUZZER_EVENT,portMAX_DELAY,pdTRUE);
		ret = WaitForEvent(STOPWATCH_BUZZER_RELEASE,2000,pdTRUE);
	}while(ret != 0);

	DEBUG_LOG("Reset successful");
}

/**
 * @brief return pdFalse on Timeout, else pdTrue
 */
uint32_t WaitForEvent(uint32_t event, TickType_t xTicksToWait, BaseType_t resetFlag){
	BaseType_t ret = pdFALSE;
	uint32_t notificationValue;


	do{
		if(resetFlag == pdFALSE){
			ret = xTaskNotifyWait(0,event,&notificationValue,xTicksToWait);
		} else{
			ret = xTaskNotifyWait(event,event,&notificationValue,xTicksToWait);
		}

	}while((ret == pdTRUE) && ((notificationValue & event) == 0));

	return (ret==pdFALSE) ? 0 : notificationValue;
}
