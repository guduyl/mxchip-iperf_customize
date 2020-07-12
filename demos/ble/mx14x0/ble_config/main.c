#include "mxos.h"
#include "ble_service.h"
#include "ble.h"

#define app_log(format, ...)        custom_log("app", format, ##__VA_ARGS__)

int main( void )
{
    merr_t err = kNoErr;
    BLE_MSG_S recv_msg;
    uint8_t recv_data[512] = { 0 };

    /* MXOS system initialize */
    err = mxos_system_init( );
    require_noerr( err, exit );

    ble_service_init( );

    while ( true )
    {
        err = ble_pop_msg( &recv_msg, 100 );
        if ( err != kNoErr )
        {
            continue;
        }

        if ( recv_msg.event == BLE_EVENT_RECV_DATA )
        {
            check( recv_msg.data != NULL && recv_msg.len > 0 && recv_msg.len < sizeof(recv_data) );

            memset( recv_data, 0, sizeof(recv_data) );
            memcpy( recv_data, recv_msg.data, recv_msg.len );

            app_log("recv data, len:%ld, %s", recv_msg.len, (char *)recv_data);

            ble_send_notify( recv_data, recv_msg.len );

            if ( recv_msg.data != NULL )
            {
                free( recv_msg.data );
                recv_msg.data = NULL;
            }
        }
    }

    exit:
    return err;
}


