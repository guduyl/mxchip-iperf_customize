#include "mxos.h"
#include "gap.h"
#include "gap_config.h"
#include "gap_le.h"
#include "gap_bond_le.h"
#include "gap_conn_le.h"
#include "gap_msg.h"
#include "gap_scan.h"
#include "gap_le_types.h"
#include "profile_client.h"
#include "ble_central_client_app.h"
#include "ble_central_app_flags.h"
#include "mbt.h"
#include "gcs_client.h"
#include "StringUtils.h"
#include "ble_central_link_mgr.h"
#include "simple_ble_client.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

/** @defgroup  CENTRAL_CLIENT_DEMO_MAIN Central Client Main
    * @brief Main file to initialize hardware and BT stack and start task scheduling
    * @{
    */

/*============================================================================*
 *                              Constants
 *============================================================================*/

/** @brief Default scan interval (units of 0.625ms, 0x10=2.5ms) */
#define DEFAULT_SCAN_INTERVAL     0x40
/** @brief Default scan window (units of 0.625ms, 0x10=2.5ms) */
#define DEFAULT_SCAN_WINDOW       0x10


/*============================================================================*
 *                              Functions
 *============================================================================*/

/**
  * @brief  Initialize central and gap bond manager related parameters
  * @return void
  */
void ble_central_app_le_gap_init(void)
{
    /* Device name and device appearance */
    uint8_t  device_name[GAP_DEVICE_NAME_LEN] = "BLE_CENTRAL_CLIENT";
    uint16_t appearance = GAP_GATT_APPEARANCE_UNKNOWN;

    /* Scan parameters */
    uint8_t  scan_mode = GAP_SCAN_MODE_ACTIVE;
    uint16_t scan_interval = DEFAULT_SCAN_INTERVAL;
    uint16_t scan_window = DEFAULT_SCAN_WINDOW;
    uint8_t  scan_filter_policy = GAP_SCAN_FILTER_ANY;
    uint8_t  scan_filter_duplicate = GAP_SCAN_FILTER_DUPLICATE_ENABLE;

    /* GAP Bond Manager parameters */
    uint8_t  auth_pair_mode = GAP_PAIRING_MODE_PAIRABLE;
    uint16_t auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG;
    uint8_t  auth_io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t  auth_oob = false;
    uint8_t  auth_use_fix_passkey = false;
    uint32_t auth_fix_passkey = 0;
    uint8_t  auth_sec_req_enable = false;
    uint16_t auth_sec_req_flags = GAP_AUTHEN_BIT_BONDING_FLAG;

    /* Set device name and device appearance */
    le_set_gap_param(GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, device_name);
    le_set_gap_param(GAP_PARAM_APPEARANCE, sizeof(appearance), &appearance);

    /* Set scan parameters */
    le_scan_set_param(GAP_PARAM_SCAN_MODE, sizeof(scan_mode), &scan_mode);
    le_scan_set_param(GAP_PARAM_SCAN_INTERVAL, sizeof(scan_interval), &scan_interval);
    le_scan_set_param(GAP_PARAM_SCAN_WINDOW, sizeof(scan_window), &scan_window);
    le_scan_set_param(GAP_PARAM_SCAN_FILTER_POLICY, sizeof(scan_filter_policy),
                      &scan_filter_policy);
    le_scan_set_param(GAP_PARAM_SCAN_FILTER_DUPLICATES, sizeof(scan_filter_duplicate),
                      &scan_filter_duplicate);

    /* Setup the GAP Bond Manager */
    gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(auth_pair_mode), &auth_pair_mode);
    gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(auth_flags), &auth_flags);
    gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(auth_io_cap), &auth_io_cap);
    gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(auth_oob), &auth_oob);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(auth_fix_passkey), &auth_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(auth_use_fix_passkey),
                      &auth_use_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
                      &auth_sec_req_flags);

    /* register gap message callback */
    le_register_app_cb(ble_central_app_gap_callback);
}

/**
 * @brief  Add GATT clients and register callbacks
 * @return void
 */
void ble_central_app_le_profile_init(void)
{
    client_init(2);

    ble_central_gcs_client_id = gcs_add_client(ble_central_gcs_client_callback, BLE_CENTRAL_APP_MAX_LINKS, BLE_CENTRAL_APP_MAX_DISCOV_TABLE_NUM);
    simple_ble_client_id = simp_ble_add_client(ble_central_simple_ble_client_callback, BLE_CENTRAL_APP_MAX_LINKS);
}

void ble_central_connect(uint8_t *DestAddr, uint8_t type)
{
    uint8_t addr_len;
    uint8_t DestAddrType = type;
    T_GAP_LE_CONN_REQ_PARAM conn_req_param;

    mbt_scan_stop();

    conn_req_param.scan_interval = DEFAULT_SCAN_INTERVAL;
    conn_req_param.scan_window = DEFAULT_SCAN_WINDOW;
    conn_req_param.conn_interval_min = 80;
    conn_req_param.conn_interval_max = 80;
    conn_req_param.conn_latency = 0;
    conn_req_param.supv_tout = 1000;
    conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
    conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
    le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);

    app_log("cmd_con, DestAddr: %02X:%02X:%02X:%02X:%02X:%02X",
            DestAddr[5], DestAddr[4], DestAddr[3], DestAddr[2], DestAddr[1],DestAddr[0]);

    mbt_le_conn(DestAddr, (T_GAP_REMOTE_ADDR_TYPE)DestAddrType, 1000);
}

void mbt_app_le_profile_init(void)
{
    ble_central_app_le_profile_init();
}

void mbt_app_le_gap_init(void)
{
    ble_central_app_le_gap_init();
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

static uint8_t ctoi(char c)
{
	if((c >= 'A') && (c <= 'F')) {
		return (c - 'A' + 0x0A);
	}

	if((c >= 'a') && (c <= 'f')) {
		return (c - 'a' + 0x0A);
	}

	if((c >= '0') && (c <= '9')) {
		return (c - '0' + 0x00);
	}

	return 0xFF;
}

static uint8_t hex_str_to_bd_addr(uint32_t str_len, char *str, uint8_t *num_arr)
{
	num_arr += str_len/2 -1;
	uint32_t n = 0;
	uint8_t num = 0;

	if (str_len < 2) {
		return FALSE;
	}
	while (n < str_len) {
		if ((num = ctoi(str[n++])) == 0xFF) {
			return FALSE;
		}
		*num_arr = num << 4;
		if ((num = ctoi(str[n++])) == 0xFF) {
			return FALSE;
		}
		*num_arr |= num;
		num_arr--;
	}
	return TRUE;
}

static void cmd_condev(int argc, char *argv[])
{
    uint8_t addr[6];
    if (argc <= 1)
        return;

    hex_str_to_bd_addr(strlen(argv[1]), argv[1], addr);

    if(argc == 2)
    {
        ble_central_connect(addr, 0);
    }else
    {
        uint8_t type = atoi(argv[2]);
        if(type <= 1 )
            ble_central_connect(addr, type);
        else
            ble_central_connect(addr, 0);
    }
}

static void cmd_ind(int argc, char *argv[])
{
    int conn_id = 0, value = 0;
    bool b_value = false;

    if (argc == 3)
    {   
        conn_id = atoi(argv[1]);
        value = atoi(argv[2]);

        b_value = value == 0 ? false:true;
        if(conn_id >= BLE_CENTRAL_APP_MAX_LINKS)
            return;

        simp_ble_client_set_v4_ind(conn_id, b_value);
    }
}

static void cmd_notify(int argc, char *argv[])
{
    int conn_id = 0, value = 0;
    bool b_value = false;

    if (argc == 3)
    {    
        conn_id = atoi(argv[1]);
        value = atoi(argv[2]);

        b_value = value == 0 ? false:true;
        if(conn_id >= BLE_CENTRAL_APP_MAX_LINKS)
            return;

        simp_ble_client_set_v3_notify(conn_id, b_value);
    }
}

static void cmd_write(int argc, char *argv[])
{
    int conn_id = 0, value = 0;

    if (argc <= 2)
        return;

    conn_id = atoi(argv[1]);

    simp_ble_client_write_v2_char(conn_id, strlen(argv[2]), (uint8_t *)argv[2], GATT_WRITE_TYPE_CMD);
}

static void cmd_read(int argc, char *argv[])
{
}

static mcli_cmd_t user_cmds[] =
{
    {
        "btscan",
        "0: stop, 1: start",
        cmd_scan
    },
    {
        "condev",
        "connect to specific bt addr",
        cmd_condev
    },
    {
        "notify",
        "enable/disable notify. notify connid value",
        cmd_notify
    },
    {
        "ind",
        "enable/disable indication. ind connid value",
        cmd_ind
    },
    {
        "write",
        "write to peripheral, write connid data",
        cmd_write
    },
    {
        "read",
        "read data",
        cmd_read
    }
};

int main( void )
{
    merr_t err = kNoErr;

    /* Start mxos system functions according to mxos_config.h*/
    mxos_system_init();

    mcli_cmds_add(user_cmds, sizeof(user_cmds) / sizeof(mcli_cmd_t));

    err = mbt_init( BLE_CENTRAL_APP_MAX_LINKS );

    return err;
}


