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


static uint8_t lastButtonState[BUTTONS_NUMBER_MAX];

void TaskButton(void){

	Buttons_Init();
	memset(lastButtonState,0,sizeof(lastButtonState));

	Debug_Write("Buttons initialized\n");

	while(1){

		for(uint8_t i=0; i<BUTTONS_NUMBER_MAX;++i){

			uint8_t val = Buttons_GetState((tButtonType)(i));

			// pressed button
			if((lastButtonState[i]==0) && (val !=0)){
				//send button event..
				char tmp[64];
				snprintf(tmp,64,"Button %d pressed\n",i);
				Debug_Write(tmp);
				lastButtonState[i] = val;
			}
			// released
			else if (lastButtonState[i]!=0 && val == 0){
				lastButtonState[i] = 0;
				char tmp[64];
				snprintf(tmp,64,"Button %d released\n",i);
				Debug_Write(tmp);
			}

		}
	}
}
