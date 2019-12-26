/**
 ******************************************************************************
 * @file    http_client.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   MiCO http client demo to read data from www.baidu.com
 ******************************************************************************
 *  The MIT License
 *  Copyright (c) 2016 MXCHIP Inc.
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
#include "HTTPUtils.h"
#include "SocketUtils.h"
#include "StringUtils.h"

#define ws_log(M, ...) custom_log("WS", M, ##__VA_ARGS__)

#define REMOTE_URL "ws://10.0.1.61:8080/examples/websocket/chat"

int test_ws_client(char *url);
static mos_semphr_id_t wait_sem = NULL;

static void micoNotify_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
    UNUSED_PARAMETER( inContext );
    switch ( status )
    {
        case NOTIFY_STATION_UP:
            mos_semphr_release( wait_sem );
            break;
        case NOTIFY_STATION_DOWN:
            case NOTIFY_AP_UP:
            case NOTIFY_AP_DOWN:
            break;
    }
}

int main( void )
{
    merr_t err = kNoErr;

    wait_sem = mos_semphr_new( 1 );

    /*Register user function for MiCO nitification: WiFi status changed */
    err = mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED,
                                       (void *) micoNotify_WifiStatusHandler, NULL );
    require_noerr( err, exit );

    /* Start MiCO system functions according to mxos_config.h */
    err = mxos_system_init( );
    require_noerr( err, exit );

    /* Wait for wlan connection*/
    mos_semphr_acquire( wait_sem, MOS_WAIT_FOREVER );
    ws_log( "wifi connected successful" );

	//ssl_set_loggingcb(ssl_log);
//	lws_set_log_level(0xF,NULL);
    test_ws_client(REMOTE_URL);

exit:
    return err;
}


