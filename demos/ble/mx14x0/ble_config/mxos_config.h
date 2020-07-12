/**
******************************************************************************
* @file    mxos_config.h
* @author  William Xu
* @version V1.0.0
* @date    08-Aug-2018
* @brief   This file provide application options diff to default.
******************************************************************************
*/

#ifndef __MXOS_CONFIG_H__
#define __MXOS_CONFIG_H__

#define APP_INFO            "uint test"

#define FIRMWARE_REVISION   "uint_test-0.0.1"
#define MANUFACTURER        "MXCHIP Inc."
#define SERIAL_NUMBER       "xxx.xxx.xxx"
#define PROTOCOL            "com.mxchip.basic"

/************************************************************************
 * MXOS_WLAN_CONNECTION_ENABLE: Start wlan connection when MXOS system starts. */
#define MXOS_WLAN_CONNECTION_ENABLE                 0

/************************************************************************
 * WIFI_CONFIG_MODE: wlan configuration mode. */
//#define WIFI_CONFIG_MODE                            WIFI_CONFIG_MODE_AWS

/************************************************************************
 * EasyLink_TimeOut: Easylink configuration timeout. */
#define EasyLink_TimeOut                            120000

/************************************************************************
 * Application thread stack size */
#define MXOS_DEFAULT_APPLICATION_STACK_SIZE         (4096)

/************************************************************************
 * Add service _easylink._tcp._local. for discovery */
#define MXOS_SYSTEM_DISCOVERY_ENABLE                0

/************************************************************************
 * Set local server port to system discovery mdns service  */
#define MXOS_SYSTEM_DISCOVERY_PORT                  8080

/************************************************************************
 * MXOS system debug info  */
#define CONFIG_SYSTEM_DEBUG                         MXOS_DEBUG_ON

/************************************************************************
 * Command line interface  */
#define MXOS_CLI_ENABLE                             1

#endif
