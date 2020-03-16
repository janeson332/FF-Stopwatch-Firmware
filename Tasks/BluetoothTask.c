/**
  ******************************************************************************
  * @file    BluetoothTask.c
  * @author  Stefan
  * @version V1.0
  * @date    26.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/

#include "RN52.h"
#include "queue.h"



void BluetoothTask(void){


	while(1){

		if(RN52_Enabled()==ENABLE){


		}

		vTaskDelay(100);
	}
}

void BluetoothReceiveCB(uint8_t ch){

}
