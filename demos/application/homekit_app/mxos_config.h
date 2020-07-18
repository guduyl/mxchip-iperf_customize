/**
******************************************************************************
* @file    mxos_config.h
* @author  Eshen Wang
* @version V1.0.0
* @date    07-July-2020
* @brief   This file provide application options diff to default.
******************************************************************************
*/

#ifndef __MXOS_CONFIG_H__
#define __MXOS_CONFIG_H__

// FW info
#define APP_INFO                        "Homekit Demo"
#define FIRMWARE_REVISION               "Homekit_Demo_1_0"
#define SERIAL_NUMBER                   "MXCH.HOME.0001"
#define MANUFACTURER                    "MXCHIP Inc."
#define PROTOCOL                        "com.mxchip.homekit"

// MXOS configurations
#define MXOS_CLI_ENABLE                 (1)
#define MXOS_WLAN_CONNECTION_ENABLE     (1)

#define MXOS_SYSTEM_DISCOVERY_ENABLE    (1)
#define CONFIG_MDNS_DEBUG               (MXOS_DEBUG_OFF)

#define MXOS_CONFIG_EASYLINK_BTN_ENABLE (1)
#define MXOS_CONFIG_SERVER_ENABLE       (0)

#define MXOS_CONFIG_IPV6                (1)

//#define MXOS_DEFAULT_APPLICATION_STACK_SIZE (2000)

// Wi-Fi config
#define WIFI_CONFIG_MODE    WIFI_CONFIG_MODE_AWS
#define EasyLink_Needs_Reboot  // reboot HAP

#endif
