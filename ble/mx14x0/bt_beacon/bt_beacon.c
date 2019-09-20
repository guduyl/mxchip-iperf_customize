/**
 ******************************************************************************
 * @file    bt_beacon.c
 * @author  Perry Liu
 * @version V1.0.0
 * @date    17-Sep-2019
 * @brief   bt beacon demo
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2019 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "mxos.h"
#include <gap.h>
#include <gap_adv.h>
#include <gap_msg.h>
#include "bt_beacon_app.h"
#include "mbt.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

/** @defgroup  PERIPH_DEMO_MAIN Peripheral Main
    * @brief Main file to initialize hardware and BT stack and start task scheduling
    * @{
    */

/*============================================================================*
 *                              Constants
 *============================================================================*/
/** @brief  Default minimum advertising interval when device is discoverable (units of 625us, 160=100ms) */
#define DEFAULT_ADVERTISING_INTERVAL_MIN            160
/** @brief  Default maximum advertising interval */
#define DEFAULT_ADVERTISING_INTERVAL_MAX            240

#define I_BEACON		1
#define ALT_BEACON		2
#define BEACON_TYPE		ALT_BEACON

#define APP_MAX_LINKS   1


/*============================================================================*
 *                              Variables
 *============================================================================*/

/** @brief  GAP - scan response data (max size = 31 bytes) */
#if BEACON_TYPE == I_BEACON
static const uint8_t scan_rsp_data[] =
{
    /* Manufacturer Specific */
    0x1A,             /* length */
    GAP_ADTYPE_MANUFACTURER_SPECIFIC,
	0x4C, 0x00,       /* Company: Apple */
    0x02,             /* Type: iBeacon */
    0x15,             /* iBeacon data length 0x15 (21) = UUID (16) + major (2) + minor (2) + RSSI (1) */
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, /* UUID (example) */
	0x00, 0x7B,       /* major (example: 123)*/
	0x01, 0xC8,       /* minor (example: 456)*/
	0xBF,             /* rssi: (example: -65 dBm) */
};

/** @brief  GAP - Advertisement data (max size = 31 bytes, best kept short to conserve power) */
static const uint8_t adv_data[] =
{
    /* Flags */
    0x02,             /* length */
    GAP_ADTYPE_FLAGS, /* type="Flags" */
    GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    /* Manufacturer Specific */
    0x1A,             /* length */
    GAP_ADTYPE_MANUFACTURER_SPECIFIC,
	0x4C, 0x00,       /* Company: Apple */
    0x02,             /* Type: iBeacon */
    0x15,             /* iBeacon data length 0x15 (21) = UUID (16) + major (2) + minor (2) + RSSI (1) */
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, /* 16 byte UUID (example) */
	0x00, 0x7B,       /* major (example: 123)*/
	0x01, 0xC8,       /* minor (example: 456)*/
	0xBF,             /* rssi: (example: -65 dBm) */
};

#elif BEACON_TYPE == ALT_BEACON
static const uint8_t scan_rsp_data[] =
{
    /* Manufacturer Specific */
    0x1B,             /* length */
    GAP_ADTYPE_MANUFACTURER_SPECIFIC,
	0x5D, 0x00,       /* Company: (example: Realtek) */
    0xBE, 0xAC,       /* Beacon Code: AltBeacon */
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, /* Beacon ID (example) */
	0x00, 0x7B,       /* Beacon ID major (example: 123)*/
	0x01, 0xC8,       /* Beacon ID minor (example: 456)*/
	0xBF,             /* rssi: (example: -65 dBm) */
	0xAA,             /* MFG RSVD (example: 0xAA)*/
};

/** @brief  GAP - Advertisement data (max size = 31 bytes, best kept short to conserve power) */
static const uint8_t adv_data[] =
{
    /* Flags */
    0x02,             /* length */
    GAP_ADTYPE_FLAGS, /* type="Flags" */
    GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    /* Manufacturer Specific */
    0x1B,             /* length */
    GAP_ADTYPE_MANUFACTURER_SPECIFIC,
	0x5D, 0x00,       /* Company: (example: Realtek) */
    0xBE, 0xAC,       /* Beacon Code: AltBeacon */
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, /* Beacon ID (example) */
	0x00, 0x7B,       /* Beacon ID major (example: 123)*/
	0x01, 0xC8,       /* Beacon ID minor (example: 456)*/
	0xBF,             /* rssi: (example: -65 dBm) */
	0xAA,             /* MFG RSVD (example: 0xAA)*/
};
#endif
/*============================================================================*
 *                              Functions
 *============================================================================*/
/**
  * @brief  Initialize peripheral and gap bond manager related parameters
  * @return void
  */
static void app_le_gap_init(void)
{
    /* Advertising parameters */
    uint8_t  adv_evt_type = GAP_ADTYPE_ADV_NONCONN_IND;
    uint8_t  adv_direct_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  adv_direct_addr[GAP_BD_ADDR_LEN] = {0};
    uint8_t  adv_chann_map = GAP_ADVCHAN_ALL;
    uint8_t  adv_filter_policy = GAP_ADV_FILTER_ANY;
    uint16_t adv_int_min = DEFAULT_ADVERTISING_INTERVAL_MIN;
    uint16_t adv_int_max = DEFAULT_ADVERTISING_INTERVAL_MAX;

    /* Set advertising parameters */
    le_adv_set_param(GAP_PARAM_ADV_EVENT_TYPE, sizeof(adv_evt_type), &adv_evt_type);
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR_TYPE, sizeof(adv_direct_type), &adv_direct_type);
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR, sizeof(adv_direct_addr), adv_direct_addr);
    le_adv_set_param(GAP_PARAM_ADV_CHANNEL_MAP, sizeof(adv_chann_map), &adv_chann_map);
    le_adv_set_param(GAP_PARAM_ADV_FILTER_POLICY, sizeof(adv_filter_policy), &adv_filter_policy);
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_int_min), &adv_int_min);
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_int_max), &adv_int_max);
    le_adv_set_param(GAP_PARAM_ADV_DATA, sizeof(adv_data), (void *)adv_data);
    le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, sizeof(scan_rsp_data), (void *)scan_rsp_data);
	
    /* register gap message callback */
    le_register_app_cb(bt_beacon_app_gap_callback);
}

void mbt_app_le_gap_init( void )
{
    app_le_gap_init();
}

/**
 * @brief    Entry of APP code
 * @return   int (To avoid compile warning)
 */
int main(void)
{
    /* Start mxos system functions according to mxos_config.h*/
    mxos_system_init();

    mbt_init(APP_MAX_LINKS);

	app_log("\n\r\n\r[BT Beacon Example] %s\n\r\n\r", (BEACON_TYPE == I_BEACON)? "Apple iBeacon": (BEACON_TYPE == ALT_BEACON)? "AltBeacon":"");
    return 0;
}

/** @} */ /* End of group PERIPH_DEMO_MAIN */


