/**
 ******************************************************************************
 * @file    iperf_main.c
 * @author  Libo
 * @version V1.0.0
 * @date    5-Jan-2018
 * @brief   This file provide the iperf test application.
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "mxos.h"

#include "bluetooth.h"
#include "gatt.h"
#include "bas.h"
#include "hci_driver.h"

#define  ble_log(M, ...) custom_log("ble", M, ##__VA_ARGS__)

void ble_controller_init( uint8_t task_priority );

static void connected( struct bt_conn *conn, uint8_t err )
{
    if ( err )
    {
        ble_log("Connection failed (err %u)\n", err);
    } else
    {
        ble_log("Connected\n");
    }
}

static void disconnected( struct bt_conn *conn, u8_t reason )
{
    ble_log("Disconnected (reason %u)\n", reason);
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};

static void bt_enable_cb( int err )
{
    ble_log("bt_enable_cb\r\n");

    bas_init();
}

void ble_init( void )
{
    ble_controller_init( 0 );
    hci_driver_init( );

    bt_enable( bt_enable_cb );

    bt_conn_cb_register( &conn_callbacks );

    return;
}

void ble_advertising( void )
{
    int err;
    uint8_t flag = 0, service_uuid[2] = { 0x0F, 0x18 };
    struct bt_data ad[4];

    flag |= BT_LE_AD_GENERAL;
    flag |= BT_LE_AD_NO_BREDR;

    ad[0].type = BT_DATA_FLAGS;
    ad[0].data = &flag;
    ad[0].data_len = 1;

    ad[1].type = BT_DATA_UUID16_ALL;
    ad[1].data = service_uuid;
    ad[1].data_len = sizeof(service_uuid);

    ad[2].type = 0xFF;
    ad[2].data_len = 9;
    ad[2].data = (const uint8_t *) "123456789";

    ad[3].type = 0x09;
    ad[3].data_len = 2;
    ad[3].data = (const uint8_t *) "MX";

    err = bt_le_adv_start( BT_LE_ADV_CONN, ad, 4, NULL, 0 );
    if ( err )
    {
        ble_log("Advertising failed to start (err %d)\n", err);
    }

    return;
}

int main( void )
{
    merr_t err = kNoErr;

    ble_init( );

    ble_advertising( );

    while ( 1 )
    {
        mos_sleep( 2 );
        ble_log("ble running");
    }

    return 0;
}

