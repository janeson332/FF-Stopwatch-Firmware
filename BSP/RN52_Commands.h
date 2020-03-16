/**
  ******************************************************************************
  * @file    RN52_Commands.h
  * @author  Stefan
  * @version V1.0
  * @date    22.02.2020
  * @brief   [Placeholder]
  ******************************************************************************
*/
#ifndef RN52_COMMANDS_H_INCLUDED
#define RN52_COMMANDS_H_INCLUDED


#define RN52_SET_NORMALIZED_NAME       "S-,"
#define RN52_SET_AUTOMATIC_SD          "S^,"
#define RN52_SET_DISCOVERY_MASK        "SD,"
#define RN52_SET_FACTORY_DEFAULTS      "SF,1"
#define RN52_SET_CONNECTION_MASK       "SK,"
#define RN52_SET_DEVICE_NAME           "SN"

#define RN52_GET_BASIC_SETTINGS        "D\r\n"
#define RN52_GET_HELP                  "H\r\n"
#define RN52_GET_FIRMWARE_VERSION      "V\r\n"

#define RN52_INCREASE_VOL              "AV+\r\n"
#define RN52_DECREASE_VOL              "AV-\r\n"
#define RN52_QUERY_CONNECTION_STATUS   "Q\r\n"
#define RN52_REBOOT                    "R,1\r\n"


#define RN52_QUERY_BYTE0_SPP           ((uint8_t)(0x02))
#define RN52_QUERY_BYTE0_A2DP          ((uint8_t)(0x04))


#endif /* RN52_COMMANDS_H_INCLUDED */
