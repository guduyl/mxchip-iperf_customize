/**
 ******************************************************************************
 * @file    mxos_main.c
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    07-July-2020
 * @brief   Homekit ADK application
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
 *
 ******************************************************************************
 */

#include "mxos.h"
#include "HAP.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

extern int homekit_main(int argc, char* _Nullable argv[_Nullable]);

static mos_semphr_id_t wait_sem = NULL;

static void micoNotify_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
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

/*when client connected wlan success,create socket*/
void homekit_app_thread(void *arg)
{
	(void)arg;

	/* start homekit app */
	homekit_main(0, NULL);

	mos_thread_delete( NULL );
}

int main(void)
{
    merr_t err = kNoErr;

    wait_sem = mos_semphr_new(1);

    /*Register user function for MiCO nitification: WiFi status changed */
    err = mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED,
                                       (void *) micoNotify_WifiStatusHandler, NULL );
    require_noerr( err, exit );

    /* Start MiCO system functions according to mxos_config.h */
    err = mxos_system_init(  );
    require_noerr( err, exit );

    /* Wait for wlan connection*/
    mos_semphr_acquire( wait_sem, MOS_WAIT_FOREVER );
    app_log( "wifi connected successful" );

    /* Start homekit app thread */
    mos_thread_new( MOS_APPLICATION_PRIORITY, "homekit_app", homekit_app_thread, 0x4000, 0 );

    exit:
    if ( wait_sem != NULL )
        mos_semphr_delete( wait_sem );
    mos_thread_delete( NULL );
    return err;
}
