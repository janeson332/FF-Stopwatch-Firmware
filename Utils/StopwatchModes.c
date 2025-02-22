/**
  ******************************************************************************
  * @file    StopwatchModes.c
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    21.03.2020
  * @brief   Implementation of the possible modes of the application
  ******************************************************************************
*/

#include <assert.h>
#include <string.h>

#include "StopwatchModes.h"
#include "Stopwatch.h"
#include "RN52.h"
#include "Utils/Debug.h"
#include "Tasks/TaskDisplay.h"
#include "Tasks/StopwatchTask.h"
#include "Tasks/BluetoothTask.h"

#include "task.h"
#include "timers.h"

#define STOPWATCH_BUZZER_EVENT     (STOPWATCH_TASK_NOTIFY_BUZZER1_PRESSED | STOPWATCH_TASK_NOTIFY_BUZZER2_PRESSED)
#define STOPWATCH_BUZZER_RELEASE   (STOPWATCH_TASK_NOTIFY_BUZZER1_RELEASED | STOPWATCH_TASK_NOTIFY_BUZZER2_RELEASED)

#define UPDATE_TIME_INTERVALL      31
#define BUZZER_RESET_INTERVALL     2000
#define TIME_TOGGLE_INTERVALL      1000

static StopwatchModeRetType_t StopwatchMode_SingleStop(void);
static StopwatchModeRetType_t StopwatchMode_DualStop(void);
static StopwatchModeRetType_t StopwatchMode_Remote(void);
static StopwatchModeRetType_t StopwatchMode_Settings(void);

static const ModeHandleTableType_t modeHandleTable[] = {
		{StopwatchMode_SingleStop, "Loeschangriff   "},
		{StopwatchMode_DualStop, "Staffellauf     "},
		{StopwatchMode_Settings, "Einstellungen   "},
		{0,0}, //signalizes end of user selectable modes
		{StopwatchMode_Remote,   "Remote Mode     "}
};

typedef enum {
	Idle,WaitForStart,StopTime,ShowResult
}StopwatchState_t;

static StopwatchState_t stopwatchState = Idle;

static StopTimeType_t dualStopTimes[2];
static uint32_t dualStopTimeIdx = 0;


static void PrintDualStopResults(StopTimeType_t const * pTimes, uint32_t const n);
static uint32_t BuzzerReset(void);
static uint32_t WaitForEvent(uint32_t event, TickType_t xTicksToWait, BaseType_t resetFlag);
static void UpdateTime(StopTimeType_t const * const pTime);


void StopwatchMode_Reset(void){
	stopwatchState = Idle;
	Stopwatch_Reset();
}

ModeHandleTableType_t const * StopwatchModes_GetModeTable(){
	return modeHandleTable;
}

static StopwatchModeRetType_t StopwatchMode_SingleStop(void){
	uint32_t taskNotificationValue=0;

	static StopTimeType_t time;

	switch(stopwatchState){
	case Idle:{
		TaskDisplay_WriteString(modeHandleTable[mode_SingleStop].displayText,0,0);
		TaskDisplay_PrintTimeString();

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
			TaskDisplay_FormatAndPrintTime(time,"");
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


static StopwatchModeRetType_t StopwatchMode_DualStop(void){
	uint32_t taskNotificationValue=0;
	static StopTimeType_t time;
	StopwatchModeRetType_t ret = StopwatchMode_OK;

	switch(stopwatchState){
	case Idle:{
		TaskDisplay_WriteString(modeHandleTable[mode_DualStop].displayText,0,0);
		TaskDisplay_PrintTimeString();

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
			TaskDisplay_FormatAndPrintTime(dualStopTimes[0],"");
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



static StopwatchModeRetType_t StopwatchMode_Remote(void){
	StopwatchModeRetType_t ret = StopwatchMode_OK;

	switch(stopwatchState){
	case Idle:{
		TaskDisplay_WriteString(modeHandleTable[mode_Remote].displayText,0,0);
		TaskDisplay_PrintTimeString();

		stopwatchState = ShowResult;
	} break;
	default:
		if(BluetoothTask_GetRemoteState()==0){
			stopwatchState = Idle;
			ret = StopwatchMode_ReturnToMain;
			// reset flags..
			WaitForEvent(STOPWATCH_TASK_NOTIFY_BUTTON_SELECT | STOPWATCH_TASK_NOTIFY_BUTTON_NEXT,0,pdTRUE);
		}
		else{
			uint32_t notification = WaitForEvent(STOPWATCH_BUZZER_EVENT |
					STOPWATCH_BUZZER_RELEASE ,0,pdTRUE);

			if(notification & STOPWATCH_TASK_NOTIFY_BUZZER1_PRESSED){
				BluetoothTask_SendBuzzerMsg(buzzer1,1);
				DEBUG_LOG("Buzzer1=Pressed");
			}
			else if(notification & STOPWATCH_TASK_NOTIFY_BUZZER1_RELEASED){
				BluetoothTask_SendBuzzerMsg(buzzer1,0);
				DEBUG_LOG("Buzzer1=Released");
			}

			if(notification & STOPWATCH_TASK_NOTIFY_BUZZER2_PRESSED){
				BluetoothTask_SendBuzzerMsg(buzzer2,1);
				DEBUG_LOG("Buzzer2=Pressed");
			}
			else if(notification & STOPWATCH_TASK_NOTIFY_BUZZER2_RELEASED){
				BluetoothTask_SendBuzzerMsg(buzzer2,0);
				DEBUG_LOG("Buzzer2=Released");
			}

		}
	}
	return ret;
}


static StopwatchModeRetType_t StopwatchMode_Settings(void){
	TaskDisplay_WriteString(modeHandleTable[mode_Settings].displayText,0,0);
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

		TaskDisplay_WriteString("Display:        ",0,1);
		if(select){
			TaskDisplay_SetBacklightVal(TaskDisplay_GetBacklightVal()==0?1:0);
		}
		TaskDisplay_WriteString((TaskDisplay_GetBacklightVal()?" ON":"OFF"),13,1);

	}break;
	case BluetoothSetting:{
		TaskDisplay_WriteString("Bluetooth:      ",0,1);
		if(select){
			RN52_PowerEnable((RN52_Enabled()==ENABLE)?DISABLE:ENABLE);
		}
		TaskDisplay_WriteString((RN52_Enabled()==ENABLE ?" ON":"OFF"),13,1);
	}break;
	case Back:{
		TaskDisplay_WriteString("Back...         ",0,1);
		if(select){
			settings = DisplaySetting;
			ret = StopwatchMode_ReturnToMain;
		}
	}break;
	}
	vTaskDelay(100);
	return ret;
}


static uint32_t BuzzerReset(void){
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
static uint32_t WaitForEvent(uint32_t event, TickType_t xTicksToWait, BaseType_t resetFlag){
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

static void UpdateTime(StopTimeType_t const * const pTime){
	assert(pTime != 0);

	static TickType_t lastTick = 0;
	if(xTaskGetTickCount() - lastTick >= UPDATE_TIME_INTERVALL){
		lastTick = xTaskGetTickCount();
		TaskDisplay_FormatAndPrintTime(*pTime,"");
	}
}

static void PrintDualStopResults(StopTimeType_t const * pTimes, uint32_t const n){
	assert(n==2);
	static TickType_t lastTick = 0;

	if(xTaskGetTickCount() - lastTick >= TIME_TOGGLE_INTERVALL){
		lastTick = xTaskGetTickCount();
		if(dualStopTimeIdx == 0){
			TaskDisplay_FormatAndPrintTime(pTimes[dualStopTimeIdx],"P1");
			dualStopTimeIdx = 1;
		} else{
			TaskDisplay_FormatAndPrintTime(pTimes[dualStopTimeIdx],"P2");
			dualStopTimeIdx = 0;
		}
	}
}
