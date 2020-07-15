#include "mxos.h"
#include <gap.h>
#include <gap_le.h>
#include <gap_scan.h>
#include <gap_adv.h>
#include <gap_bond_le.h>
#include "ble_service.h"
#include "mbt.h"
#include "ble.h"
#include "ble_profile.h"
#include "StringUtils.h"

#define app_log(M, ...)         MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

/** Max link number of client */
#define APP_MAX_LINKS  1

/*============================================================================*
 *                              Constants
 *============================================================================*/
/** @brief  Default minimum advertising interval when device is discoverable (units of 625us, 160=100ms) */
#define DEFAULT_ADVERTISING_INTERVAL_MIN            160
/** @brief  Default maximum advertising interval */
#define DEFAULT_ADVERTISING_INTERVAL_MAX            200

#define DEFAULT_SCAN_INTERVAL     0x40
#define DEFAULT_SCAN_WINDOW       0x10

/*============================================================================*
 *                              Variables
 *============================================================================*/

static mos_queue_id_t ble_msg = NULL;

///** @brief  GAP - scan response data (max size = 31 bytes) */
//static const uint8_t scan_rsp_data[] =
//{
//    0x13,                                       /* length */
//    GAP_ADTYPE_LOCAL_NAME_COMPLETE,            /* type="Complete local name" */
//    'S', 'U', 'N', 'I', 'N', 'G', '_',
//    'D', 'E', 'V', 'I', 'C', 'E', '_', '0', '0', '0', '0'
//};

#define ADV_MAC_INDEX               (10)
#define ADV_USER_DATA_INDEX         (16)
#define ADV_USER_DATA_MAX_LEN       (15)

/** @brief  GAP - Advertisement data (max size = 31 bytes, best kept short to conserve power) */
static uint8_t adv_data[] =
{
    /* Flags */
    0x02, /* length */
    GAP_ADTYPE_FLAGS, /* type="Flags" */
    GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

    /* Local name */
    0x03, /* length */
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'M', 'X',

    /* Manufacture data */
    0x14, /* length */
    GAP_ADTYPE_MANUFACTURER_SPECIFIC,
    0x01,                                           //version
    0x66, 0x55, 0x44, 0x33, 0x22, 0x11,             //default ble mac, the mac address start index: ADV_MAC_INDEX
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  //Reserve
};

/*============================================================================*
 *                              Functions
 *============================================================================*/

WEAK void ble_get_pid( char *pid_string, uint32_t buff_len )
{
    app_log("ble_get_pid not define, error!");

    return;
}


static void ble_set_adv_ble_mac( void )
{
    uint8_t ble_mac[32] = { 0 };

    gap_get_param( GAP_PARAM_BD_ADDR, ble_mac );

    app_log("suning ble mac:%02X%02X%02X%02X%02X%02X", ble_mac[0], ble_mac[1], ble_mac[2], ble_mac[3], ble_mac[4], ble_mac[5]);

    adv_data[ADV_MAC_INDEX + 0] = ble_mac[5];
    adv_data[ADV_MAC_INDEX + 1] = ble_mac[4];
    adv_data[ADV_MAC_INDEX + 2] = ble_mac[3];
    adv_data[ADV_MAC_INDEX + 3] = ble_mac[2];
    adv_data[ADV_MAC_INDEX + 4] = ble_mac[1];
    adv_data[ADV_MAC_INDEX + 5] = ble_mac[0];

    return;
}

WEAK void ble_adc_get_user_data( void *data_p, uint8_t *len_p )
{
    app_log("not set user data in ble adv");
    return;
}

void ble_adv_param_set( void )
{
    uint8_t user_data[ADV_USER_DATA_MAX_LEN] = { 0 };
    uint8_t len = 0;

    ble_set_adv_ble_mac( );

    ble_adc_get_user_data( user_data, &len );
    require( len <= ADV_USER_DATA_MAX_LEN, exit );

    if ( len > 0 )
    {
        memcpy( &adv_data[ADV_USER_DATA_INDEX], user_data, len );
    }

    le_adv_set_param( GAP_PARAM_ADV_DATA, sizeof(adv_data), (void *) adv_data );

    exit:
    return;
}

/**
  * @brief  Initialize peripheral and gap bond manager related parameters
  * @return void
  */
void app_le_gap_init(void)
{
    /* Device name and device appearance */
    uint8_t  device_name[GAP_DEVICE_NAME_LEN] = "SN";
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
    le_set_gap_param(GAP_PARAM_SLAVE_INIT_GATT_MTU_REQ, sizeof(slave_init_mtu_req), &slave_init_mtu_req);

    /* Set scan parameters */
    le_scan_set_param( GAP_PARAM_SCAN_MODE, sizeof(scan_mode), &scan_mode );
    le_scan_set_param( GAP_PARAM_SCAN_WINDOW, sizeof(scan_window), &scan_window );
    le_scan_set_param( GAP_PARAM_SCAN_INTERVAL, sizeof(scan_interval), &scan_interval );
    le_scan_set_param( GAP_PARAM_SCAN_FILTER_POLICY, sizeof(scan_filter_policy), &scan_filter_policy );
    le_scan_set_param( GAP_PARAM_SCAN_FILTER_DUPLICATES, sizeof(scan_filter_duplicate), &scan_filter_duplicate );

    /* Set advertising parameters */
    le_adv_set_param(GAP_PARAM_ADV_EVENT_TYPE, sizeof(adv_evt_type), &adv_evt_type);
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR_TYPE, sizeof(adv_direct_type), &adv_direct_type);
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR, sizeof(adv_direct_addr), adv_direct_addr);
    le_adv_set_param(GAP_PARAM_ADV_CHANNEL_MAP, sizeof(adv_chann_map), &adv_chann_map);
    le_adv_set_param(GAP_PARAM_ADV_FILTER_POLICY, sizeof(adv_filter_policy), &adv_filter_policy);
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_int_min), &adv_int_min);
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_int_max), &adv_int_max);

//    le_adv_set_param( GAP_PARAM_ADV_DATA, sizeof(adv_data), (void *) adv_data );
//    le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, sizeof(scan_rsp_data), (void *)scan_rsp_data);

    /* Setup the GAP Bond Manager */
    gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(auth_pair_mode), &auth_pair_mode);
    gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(auth_flags), &auth_flags);
    gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(auth_io_cap), &auth_io_cap);
#if F_BT_LE_SMP_OOB_SUPPORT
    gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(auth_oob), &auth_oob);
#endif
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(auth_fix_passkey), &auth_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(auth_use_fix_passkey), &auth_use_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags), &auth_sec_req_flags);

    /* register gap message callback */
    le_register_app_cb(app_gap_callback);
}

/**
 * @brief  Add GATT services and register callbacks
 * @return void
 */
void app_le_profile_init(void)
{
    T_SERVER_ID service_id = 0;

    server_init(1);

    service_id = ble_service_add_service((void *)app_profile_callback);

    ble_set_service_id( service_id );

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

void ble_adv_stop( void )
{
    mbt_le_adv_stop( );
    return;
}

int ble_service_init( void )
{
    int err = kGeneralErr;

    //create msg queue
    ble_msg = mos_queue_new( sizeof(BLE_MSG_S), BLE_MSG_QUEUE_SIZE );
    require( ble_msg != NULL, exit );

    mbt_init(APP_MAX_LINKS);

    err = kNoErr;

    exit:
    return err;
}

int ble_push_msg( BLE_MSG_S *msg_p )
{
    int err = kGeneralErr;

    require( ble_msg != NULL && msg_p != NULL, exit );
    require_string( mos_queue_get_free( ble_msg ) > 0, exit, "sys event queue is full!");

    err = mos_queue_push( ble_msg, msg_p, 0 );
    require_noerr( err, exit );

    exit:
    return err;
}

int ble_pop_msg( BLE_MSG_S *msg_p, uint32_t timeout_ms )
{
    int err = kGeneralErr;

    require( ble_msg != NULL && msg_p != NULL, exit );

    err = mos_queue_pop( ble_msg, msg_p, timeout_ms );

    exit:
    return err;
}

//push data into queue
USED void ble_recv_data( void *data, uint32_t len )
{
    int err = kGeneralErr;
    BLE_MSG_S msg;

    msg.event = BLE_EVENT_RECV_DATA;
    msg.data = malloc( len );
    require( msg.data != NULL, exit );

    memset( msg.data, 0, len );
    memcpy( msg.data, data, len );

    msg.len = len;

    err = ble_push_msg( &msg );
    require_noerr( err, exit );

    exit:
    return;
}
