/**
  ******************************************************************************
  * @file    BluetoothCommands.h
  * @author  Stefan
  * @version V1.0
  * @date    16.03.2020
  * @brief   Commands for the communication with the app
  ******************************************************************************
*/


#ifndef BLUETOOTHCOMMANDS_H_INCLUDED
#define BLUETOOTHCOMMANDS_H_INCLUDED

#define BT_COMMAND_ACK                "OK"     /*< acknowledge value */
#define BT_COMMAND_NACK               "NOK"    /*< not acknowledge value */
#define BT_COMMAND_ERROR              "ERR"    /*< error value on unrecognized command */

#define BT_VALUE_ON                   "ON"     /*< remote command value ON */
#define BT_VALUE_OFF                  "OFF"    /*< remote command value OFF */

#define BT_COMMAND_REMOTE             "Remote="
#define BT_COMMAND_BUZZER1            "Buzzer1="
#define BT_COMMAND_BUZZER2            "Buzzer2="
#define BT_COMMAND_SET_TIME           "SetTime="
#define BT_COMMAND_UPDATE_TIME        "UpdateTime="
#define BT_COMMAND_RESET_TIME         "ResetTime"
#define BT_COMMAND_RESET_REMOTE_MODE  "ResetRemoteMode"

#define BT_BUZZER_PRESSED             "Pressed"
#define BT_BUZZER_RELEASED            "Released"



#endif /* BLUETOOTHCOMMANDS_H_INCLUDED */
