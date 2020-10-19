/**
******************************************************************************
* @file    mxos_config.h
* @author  Snow Yang
* @version V1.0.0
* @date    08-Aug-2018
* @brief   This file provide application options diff to default.
******************************************************************************
*/

#ifndef __MXOS_CONFIG_H__
#define __MXOS_CONFIG_H__

#define APP_INFO            "MXOS Demo(FLASH)"
#define FIRMWARE_REVISION   "V1.0.0"
#define MANUFACTURER        "MXCHIP Inc."
#define SERIAL_NUMBER       "20201019"
#define PROTOCOL            "com.mxchip.demo"

/************************************************************************
 * Application thread stack size */
#define MXOS_DEFAULT_APPLICATION_STACK_SIZE         (1500)

#define MXOS_CLI_ENABLE                 (1)
#define MXOS_SYSTEM_MONITOR_ENABLE      (1) // set 0 to disable WDG for debug

#define MXOS_WLAN_CONNECTION_ENABLE     (0)
#define MXOS_CONFIG_EASYLINK_BTN_ENABLE (0)
#define EasyLink_Needs_Reboot

#define MXOS_CONFIG_SERVER_ENABLE       (0)
#define MXOS_SYSTEM_DISCOVERY_ENABLE    (0)

#endif
