/**
******************************************************************************
* @file    mxos_config.h
* @author  Eshen Wang
* @version V1.0.0
* @date    05-Aug-2020
* @brief   This file provide application options diff to default.
******************************************************************************
*/

#ifndef __MXOS_CONFIG_H__
#define __MXOS_CONFIG_H__

// FW info
#define APP_INFO                        "Qcloud Demo"
#define FIRMWARE_REVISION               "V0_1"
#define SERIAL_NUMBER                   "MXCH.QIOT.0001"
#define MANUFACTURER                    "MXCHIP Inc."
#define PROTOCOL                        "com.mxchip.qcloud"

// MXOS configurations
#define MXOS_CLI_ENABLE                 (1)

#define MXOS_SYSTEM_DISCOVERY_ENABLE    (0)
#define MXOS_CONFIG_SERVER_ENABLE       (0)

#define MXOS_CONFIG_IPV6                (1)

//#define MXOS_DEFAULT_APPLICATION_STACK_SIZE (2000)

// Wi-Fi config /* 1: tencent softap, 0: MXCHIP AWSS (need app) */
#define CONFIG_WIFI_SOFTAP_ENABLED      (1)

#if CONFIG_WIFI_SOFTAP_ENABLED // tencent softAP
#define MXOS_WLAN_CONNECTION_ENABLE     (0)
#else // MXCHIP AWSS
#define MXOS_WLAN_CONNECTION_ENABLE     (1)
#define WIFI_CONFIG_MODE    WIFI_CONFIG_MODE_AWS
#endif

// wifi config mode trigger by easylink button
#define MXOS_CONFIG_EASYLINK_BTN_ENABLE (1)
#define EasyLink_Needs_Reboot

#endif
