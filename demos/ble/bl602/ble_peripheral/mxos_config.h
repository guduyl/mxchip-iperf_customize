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

#define APP_INFO            "helloworld"

#define FIRMWARE_REVISION   "0.0.1"
#define MANUFACTURER        "MXCHIP Inc."
#define PROTOCOL            "com.mxchip.basic"
#define SERIAL_NUMBER       "0000.0000.0001"


/************************************************************************
 * MXOS_WLAN_CONNECTION_ENABLE: Start wlan connection when MXOS system starts. */
#define MXOS_WLAN_CONNECTION_ENABLE                 1

/************************************************************************
 * WIFI_CONFIG_MODE: wlan configuration mode. */
#define WIFI_CONFIG_MODE                            WIFI_CONFIG_MODE_NONE

/************************************************************************
 * EasyLink_TimeOut: Easylink configuration timeout. */
#define EasyLink_TimeOut                            600000

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
 * Start standard QC test function other than application  */
#define MXOS_QUALITY_CONTROL_ENABLE                 1

/**
 *  MXOS_CONFIG_EASYLINK_BTN_ENABLE: Enable EasyLink Button,
 *  - Press to start easylink
 *  - Long pressed  @ref MXOS_CONFIG_EASYLINK_BTN_LONG_PRESS_TIMEOUT milliseconds
 *    to clear all settings
 *  Default: Enable
 */
#define MXOS_CONFIG_EASYLINK_BTN_ENABLE             0

/************************************************************************
 * Command line interface  */
#define MXOS_CLI_ENABLE                             1

#define MXOS_SYSTEM_MONITOR_ENABLE                  0

#endif
