/**
  ******************************************************************************
  * @file    StopwatchModes.c
  * @author  Stefan
  * @version V1.0
  * @date    19.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/


#include <assert.h>
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

#define UPDATE_TIME_INTERVALL      31
#define BUZZER_RESET_INTERVALL     2000
#define TIME_TOGGLE_INTERVALL      1000

static const tModeHandleTable modeHandleTable[] = {
		{StopwatchMode_SingleStop, "Loeschangriff   "},
		{StopwatchMode_DualStop, "Staffellauf     "},
		{StopwatchMode_Settings, "Einstellungen   "},
		{0,0}, //signalizes end of user selectable modes
		{StopwatchMode_SingleStop,   "Remote Mode     "}
};

typedef enum {
	Idle,WaitForStart,StopTime,ShowResult
}StopwatchState_t;

static StopwatchState_t stopwatchState = Idle;

static tTimeStruct dualStopTimes[2];
static uint32_t dualStopTimeIdx = 0;


void FormatAndPrintTime(tTimeStruct time,char const * preString);
void PrintDualStopResults(tTimeStruct const * pTimes, uint32_t const n);
uint32_t BuzzerReset(void);
uint32_t WaitForEvent(uint32_t event, TickType_t xTicksToWait, BaseType_t resetFlag);
void UpdateTime(tTimeStruct const * const pTime);

tModeHandleTable const * StopwatchModes_GetModeTable(){
	return modeHandleTable;
}

StopwatchModeRetType_t StopwatchMode_SingleStop(void){
	uint32_t taskNotificationValue=0;

	static tTimeStruct time;

	switch(stopwatchState){
	case Idle:{
		TaskDisplayWriteString(modeHandleTable[mode_SingleStop].displayText,0,0);
		TaskDisplayWriteString("Zeit:           ",0,1);
		Stopwatch_Reset();
		stopwatchState = WaitForStart;
	} break;

	case WaitForStart:{

		taskNotificationValue = WaitForEvent(STOPWATCH_TASK_NOTIFY_BUTTON_SELECT | STOPWATCH_BUZZER_EVENT,0,pdTRUE);
		if(taskNotificationValue & STOPWATCH_TASK_NOTIFY_BUTTON_SELECT){
			DEBUG_LOG("Return to main handler");
			stopwatchState = Idle;
			return StopwatchMode_ReturnToMain;
		}
		else if(taskNotificationValue & STOPWATCH_BUZZER_EVENT){
			DEBUG_LOG("Start stopping time");
			Stopwatch_StartContinue();
			stopwatchState = StopTime;
		}

	}break;

	case StopTime:{
		taskNotificationValue = WaitForEvent(STOPWATCH_BUZZER_EVENT,0,pdTRUE);

		if(taskNotificationValue != 0){
			Stopwatch_Stop(&time);
			FormatAndPrintTime(time,"");
			DEBUG_LOG("Time Stopped");
			stopwatchState = ShowResult;
		}else{
			Stopwatch_GetTime(&time);
			UpdateTime(&time);
		}

	}break;
	case ShowResult:{
		if(BuzzerReset()){
			stopwatchState = Idle;
		}

	}break;

	default:
		DEBUG_LOG("Error: We never should reach here");
		stopwatchState = Idle;
		return StopwatchMode_Error;
	}

	return StopwatchMode_OK;
}


StopwatchModeRetType_t StopwatchMode_DualStop(void){
	uint32_t taskNotificationValue=0;
	static tTimeStruct time;
	StopwatchModeRetType_t ret = StopwatchMode_OK;

	switch(stopwatchState){
	case Idle:{
		TaskDisplayWriteString(modeHandleTable[mode_DualStop].displayText,0,0);
		TaskDisplayWriteString("Zeit:           ",0,1);
		Stopwatch_Reset();
		dualStopTimeIdx = 0;
		memset(&dualStopTimes,0,sizeof(dualStopTimes));
		stopwatchState = WaitForStart;
	} break;

	case WaitForStart:{

		taskNotificationValue = WaitForEvent(STOPWATCH_TASK_NOTIFY_BUTTON_SELECT | STOPWATCH_BUZZER_EVENT,0,pdTRUE);
		if(taskNotificationValue & STOPWATCH_TASK_NOTIFY_BUTTON_SELECT){
			DEBUG_LOG("Return to main handler");
			stopwatchState = Idle;
			ret = StopwatchMode_ReturnToMain;
		}
		else if(taskNotificationValue & STOPWATCH_BUZZER_EVENT){
			DEBUG_LOG("Start stopping time");
			Stopwatch_StartContinue();
			stopwatchState = StopTime;
		}

	}break;

	case StopTime:{
		taskNotificationValue = WaitForEvent(STOPWATCH_BUZZER_EVENT,0,pdTRUE);

		Stopwatch_GetTime(&time);
		UpdateTime(&time);

		if((dualStopTimes[0].milliseconds == 0) && (taskNotificationValue & STOPWATCH_TASK_NOTIFY_BUZZER1_PRESSED)){
			DEBUG_LOG("Time0 captured");
			dualStopTimes[0] = time;
		}
		if((dualStopTimes[1].milliseconds == 0) && (taskNotificationValue & STOPWATCH_TASK_NOTIFY_BUZZER2_PRESSED)){
			DEBUG_LOG("Time1 captured");
			dualStopTimes[1] = time;
		}

		if((dualStopTimes[0].milliseconds != 0) && (dualStopTimes[1].milliseconds != 0)){
			Stopwatch_Stop(0);
			DEBUG_LOG("Time Stopped");
			stopwatchState = ShowResult;
			FormatAndPrintTime(dualStopTimes[0],"");
		}

	}break;
	case ShowResult:{
		PrintDualStopResults(dualStopTimes,2);

		if(BuzzerReset()){
			stopwatchState = Idle;
		}

	}break;

	default:
		DEBUG_LOG("Error: We never should reach here");
		stopwatchState = Idle;
		ret = StopwatchMode_Error;
	}

	return ret;
}



StopwatchModeRetType_t StopwatchMode_Remote(void){
	return StopwatchMode_ReturnToMain;
}


StopwatchModeRetType_t StopwatchMode_Settings(void){
	TaskDisplayWriteString(modeHandleTable[mode_Settings].displayText,0,0);
	typedef enum {
		DisplaySetting=0,
		BluetoothSetting=1,
		Back=2
	}tSettings;

	static tSettings settings = DisplaySetting;
	StopwatchModeRetType_t ret = StopwatchMode_OK;

	uint32_t notificationVal= WaitForEvent(STOPWATCH_TASK_NOTIFY_BUTTON_SELECT | STOPWATCH_TASK_NOTIFY_BUTTON_NEXT,0,pdTRUE);
	uint32_t select = notificationVal & STOPWATCH_TASK_NOTIFY_BUTTON_SELECT;
	uint32_t next = notificationVal & STOPWATCH_TASK_NOTIFY_BUTTON_NEXT;

	if(next){
		if(settings == Back){
			settings = DisplaySetting;
		}
		else{
			settings++;
		}
	}

	switch(settings){
	case DisplaySetting:{

		TaskDisplayWriteString("Display:        ",0,1);
		if(select){
			TaskDisplaySetBacklightVal(TaskDisplayGetBacklightVal()==0?1:0);
		}
		TaskDisplayWriteString((TaskDisplayGetBacklightVal()?" ON":"OFF"),13,1);

	}break;
	case BluetoothSetting:{
		TaskDisplayWriteString("Bluetooth:      ",0,1);
		if(select){
			RN52_PowerEnable((RN52_Enabled()==ENABLE)?DISABLE:ENABLE);
		}
		TaskDisplayWriteString((RN52_Enabled()==ENABLE ?" ON":"OFF"),13,1);
	}break;
	case Back:{
		TaskDisplayWriteString("Back...         ",0,1);
		if(select){
			settings = DisplaySetting;
			ret = StopwatchMode_ReturnToMain;
		}
	}break;
	}
	vTaskDelay(100);
	return ret;
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

uint32_t BuzzerReset(void){
	uint32_t ret = 0;
	uint32_t notificationValue = 0;
	typedef enum{Pressed,Released}tState;
	static tState state = Released;
	static TickType_t lastTick = 0;

	if(state == Released){
		notificationValue = WaitForEvent(STOPWATCH_BUZZER_EVENT,0,pdTRUE);
		if(notificationValue != 0){
			lastTick = xTaskGetTickCount();
			state = Pressed;
		}
	}
	else{
		if(xTaskGetTickCount() - lastTick >= BUZZER_RESET_INTERVALL){
			state = Released;
			DEBUG_LOG("Reset successful");
			ret = 1;
		}else if(WaitForEvent(STOPWATCH_BUZZER_RELEASE,0,pdTRUE)){
			state = Released;
			DEBUG_LOG("Retry");
		}
	}

	return ret;
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

void UpdateTime(tTimeStruct const * const pTime){
	assert(pTime != 0);

	static TickType_t lastTick = 0;
	if(xTaskGetTickCount() - lastTick >= UPDATE_TIME_INTERVALL){
		lastTick = xTaskGetTickCount();
		FormatAndPrintTime(*pTime,"");
	}
}

void PrintDualStopResults(tTimeStruct const * pTimes, uint32_t const n){
	assert(n==2);
	static TickType_t lastTick = 0;

	if(xTaskGetTickCount() - lastTick >= TIME_TOGGLE_INTERVALL){
		lastTick = xTaskGetTickCount();
		if(dualStopTimeIdx == 0){
			FormatAndPrintTime(pTimes[dualStopTimeIdx],"P1");
			dualStopTimeIdx = 1;
		} else{
			FormatAndPrintTime(pTimes[dualStopTimeIdx],"P2");
			dualStopTimeIdx = 0;
		}
	}
}
