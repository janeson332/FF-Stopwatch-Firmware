/**
  ******************************************************************************
  * @file    main.c
  * @author  Janeson332
  * @version V1.0
  * @date    24.01.2020
  * @brief   Sample main function for FreeRTOS on stm32f4 microcontroller
  ******************************************************************************
*/


// FreeRTOS Kernel includes..
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"


#include "stm32f4xx.h"

// Test Task Settings (Task Control Block and Stack)
#define TEST_TASK_STACK_SIZE 100
StaticTask_t tcbTestTask;
StackType_t stackTestTask[TEST_TASK_STACK_SIZE];
			
// simple test task
void myTestTask(void * parameters){
    int x = 0;
    while(1){
        x++;
        vTaskDelay(1000);
    }
}


int main(void){

    xTaskCreateStatic(myTestTask,"TestTaskName",TEST_TASK_STACK_SIZE,0,tskIDLE_PRIORITY,stackTestTask,&tcbTestTask);

    vTaskStartScheduler();

    while(1);
}


