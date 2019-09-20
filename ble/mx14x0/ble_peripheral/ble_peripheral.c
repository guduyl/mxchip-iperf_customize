/**
 ******************************************************************************
 * @file    ble_peripheral.c
 * @author  Perry Liu
 * @version V1.0.0
 * @date    17-Sep-2019
 * @brief   Ble peripheral demo
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

#include "mxos.h"
#include <gap.h>
#include <gap_le.h>
#include <gap_scan.h>
#include <gap_adv.h>
#include <gap_bond_le.h>
#include "peripheral_app.h"
#include "mbt.h"
#include "simple_ble_service.h"
#include "bas.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

/** Max link number of client */
#define APP_MAX_LINKS  1

/*============================================================================*
 *                              Constants
 *============================================================================*/
/** @brief  Default minimum advertising interval when device is discoverable (units of 625us, 160=100ms) */
#define DEFAULT_ADVERTISING_INTERVAL_MIN            320
/** @brief  Default maximum advertising interval */
#define DEFAULT_ADVERTISING_INTERVAL_MAX            400

#define DEFAULT_SCAN_INTERVAL     0x40
#define DEFAULT_SCAN_WINDOW       0x10

/*============================================================================*
 *                              Variables
 *============================================================================*/

/** @brief  GAP - scan response data (max size = 31 bytes) */
static const uint8_t scan_rsp_data[] =
{
    0x03,                             /* length */
    GAP_ADTYPE_APPEARANCE,            /* type="Appearance" */
    LO_WORD(GAP_GATT_APPEARANCE_UNKNOWN),
    HI_WORD(GAP_GATT_APPEARANCE_UNKNOWN),
};

/** @brief  GAP - Advertisement data (max size = 31 bytes, best kept short to conserve power) */
static const uint8_t adv_data[] =
{
    /* Flags */
    0x02,             /* length */
    GAP_ADTYPE_FLAGS, /* type="Flags" */
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    /* Service */
    0x03,             /* length */
    GAP_ADTYPE_16BIT_COMPLETE,
    LO_WORD(GATT_UUID_SIMPLE_PROFILE),
    HI_WORD(GATT_UUID_SIMPLE_PROFILE),
    /* Local name */
    0x0F,             /* length */
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'B', 'L', 'E', '_', 'P', 'E', 'R', 'I', 'P', 'H', 'E', 'R', 'A', 'L',
};

/*============================================================================*
 *                              Functions
 *============================================================================*/

/**
  * @brief  Initialize peripheral and gap bond manager related parameters
  * @return void
  */
void app_le_gap_init(void)
{
    /* Device name and device appearance */
    uint8_t  device_name[GAP_DEVICE_NAME_LEN] = "BLE_PERIPHERAL";
    uint16_t appearance = GAP_GATT_APPEARANCE_UNKNOWN;
    uint8_t  slave_init_mtu_req = false;


    /* Advertising parameters */
    uint8_t  adv_evt_type = GAP_ADTYPE_ADV_IND;
    uint8_t  adv_direct_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  adv_direct_addr[GAP_BD_ADDR_LEN] = {0};
    uint8_t  adv_chann_map = GAP_ADVCHAN_ALL;
    uint8_t  adv_filter_policy = GAP_ADV_FILTER_ANY;
    uint16_t adv_int_min = DEFAULT_ADVERTISING_INTERVAL_MIN;
    uint16_t adv_int_max = DEFAULT_ADVERTISING_INTERVAL_MAX;

    /* Scan parameters */
    uint8_t scan_mode = GAP_SCAN_MODE_ACTIVE;
    uint8_t scan_filter_policy = GAP_SCAN_FILTER_ANY;
    uint8_t scan_filter_duplicate = GAP_SCAN_FILTER_DUPLICATE_ENABLE;
    uint16_t scan_interval = DEFAULT_SCAN_INTERVAL;
    uint16_t scan_window = DEFAULT_SCAN_WINDOW;

    /* GAP Bond Manager parameters */
    uint8_t  auth_pair_mode = GAP_PAIRING_MODE_PAIRABLE;
    uint16_t auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG;
    uint8_t  auth_io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
#if F_BT_LE_SMP_OOB_SUPPORT
    uint8_t  auth_oob = false;
#endif
    uint8_t  auth_use_fix_passkey = false;
    uint32_t auth_fix_passkey = 0;
    uint8_t  auth_sec_req_enable = false;
    uint16_t auth_sec_req_flags = GAP_AUTHEN_BIT_BONDING_FLAG;

    /* Set device name and device appearance */
    le_set_gap_param(GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, device_name);
    le_set_gap_param(GAP_PARAM_APPEARANCE, sizeof(appearance), &appearance);
    le_set_gap_param(GAP_PARAM_SLAVE_INIT_GATT_MTU_REQ, sizeof(slave_init_mtu_req),
                     &slave_init_mtu_req);

    /* Set scan parameters */
    le_scan_set_param( GAP_PARAM_SCAN_MODE, sizeof(scan_mode), &scan_mode );
    le_scan_set_param( GAP_PARAM_SCAN_WINDOW, sizeof(scan_window), &scan_window );
    le_scan_set_param( GAP_PARAM_SCAN_INTERVAL, sizeof(scan_interval), &scan_interval );
    le_scan_set_param( GAP_PARAM_SCAN_FILTER_POLICY, sizeof(scan_filter_policy),
                       &scan_filter_policy );
    le_scan_set_param( GAP_PARAM_SCAN_FILTER_DUPLICATES, sizeof(scan_filter_duplicate),
                       &scan_filter_duplicate );

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

    /* Setup the GAP Bond Manager */
    gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(auth_pair_mode), &auth_pair_mode);
    gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(auth_flags), &auth_flags);
    gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(auth_io_cap), &auth_io_cap);
#if F_BT_LE_SMP_OOB_SUPPORT
    gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(auth_oob), &auth_oob);
#endif
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(auth_fix_passkey), &auth_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(auth_use_fix_passkey),
                      &auth_use_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
                      &auth_sec_req_flags);

    /* register gap message callback */
    le_register_app_cb(app_gap_callback);
}

/**
 * @brief  Add GATT services and register callbacks
 * @return void
 */
void app_le_profile_init(void)
{
    server_init(1);

    simp_srv_id = simp_ble_service_add_service((void *)app_profile_callback);
    server_register_app_cb(app_profile_callback);
}

void mbt_app_le_profile_init(void)
{
    app_le_profile_init();
}

void mbt_app_le_gap_init(void)
{
    app_le_gap_init();
}

static void cmd_scan(int argc, char *argv[])
{
    if (argc <= 1)
        return;

    if (atoi(argv[1]) == 1)
    {
        uint8_t scan_filter_policy = GAP_SCAN_FILTER_ANY;
        uint8_t scan_filter_duplicate = GAP_SCAN_FILTER_DUPLICATE_DISABLE;

        le_scan_set_param(GAP_PARAM_SCAN_FILTER_POLICY, sizeof(scan_filter_policy),
                          &scan_filter_policy);
        le_scan_set_param(GAP_PARAM_SCAN_FILTER_DUPLICATES, sizeof(scan_filter_duplicate),
                          &scan_filter_duplicate);
        mbt_scan_start();
    }
    else
    {
        mbt_scan_stop();
    }
}

static void cmd_send_notify(int argc, char *argv[])
{
    if (argc <= 1)
        return;

    simp_ble_service_send_v3_notify(link_state.conn_id, simp_srv_id, argv[1], strlen(argv[1]));
}

static void cmd_send_indicate(int argc, char *argv[])
{
    if (argc <= 1)
        return;

    simp_ble_service_send_v4_indicate(link_state.conn_id, simp_srv_id, argv[1], strlen(argv[1]));
}

static mcli_cmd_t user_cmds[] =
{
    {
        "btscan",
        "0: stop, 1: start",
        cmd_scan
    },
    {
        "notify",
        "send notification to client",
        cmd_send_notify
    },
    {
        "indicate",
        "send indication to client",
        cmd_send_indicate
    }
};

int main(void)
{
    /* Start mxos system functions according to mxos_config.h*/
    mxos_system_init();

    mcli_cmds_add(user_cmds, sizeof(user_cmds) / sizeof(mcli_cmd_t));

    mbt_init(APP_MAX_LINKS);

    return 0;
}
