/**
 ******************************************************************************
 * @file    mxos_main.c
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    05-Aug-2020
 * @brief   qcloud sdk application
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
#include "sntp.h"
#include "rgb_led.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

static mos_semphr_id_t wifi_wait_sem = NULL;
static mos_semphr_id_t time_wait_sem = NULL;

extern int qcloud_main(int argc, char **argv);

static void ntp_time_synced_fun(void)
{
  app_log("NTP sync time successfuly.");
  mos_semphr_release(time_wait_sem);
}

static int board_init(void)
{
    rgb_led_init();
    rgb_led_open(0, 0, 0);

    return 0;
}

/* wifi status changed callback */
static void micoNotify_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
    switch ( status )
    {
        case NOTIFY_STATION_UP:
            mos_semphr_release( wifi_wait_sem );
            break;
        case NOTIFY_STATION_DOWN:
        case NOTIFY_AP_UP:
        case NOTIFY_AP_DOWN:
            break;
    }
}

/* when wifi connected success,create homekit server */
static void qcloud_app_thread(void *arg)
{
    merr_t err;
	(void)arg;

    /* NTP time sync */
    time_wait_sem = mos_semphr_new(1);
    require( NULL != time_wait_sem, exit );

    err = sntp_start_auto_time_sync(10 * 1000, ntp_time_synced_fun);
    require_noerr( err, exit );
    app_log("network time sync ...");
    (void)mos_semphr_acquire(time_wait_sem, MOS_WAIT_FOREVER);

	/* start qcloud app */
    app_log("qcloud app start ...");
    (void)qcloud_main(0, NULL);

exit:
    app_log("*** qcloud app thread exit !!!");
	mos_thread_delete( NULL );
}

/**
 * main entrance
 */
int main(void)
{
    merr_t err = kNoErr;

    /* board hardware init */
    (void)board_init();

    wifi_wait_sem = mos_semphr_new(1);
    require( NULL != wifi_wait_sem, exit );

    /*Register user function for MXOS notification: WiFi status changed */
    err = mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED,
                                       (void *) micoNotify_WifiStatusHandler, NULL );
    require_noerr( err, exit );

    /* Start MXOS system functions according to mxos_config.h */
    err = mxos_system_init(  );
    require_noerr( err, exit );

    /* Wait for wlan connection */
    mos_semphr_acquire( wifi_wait_sem, MOS_WAIT_FOREVER );
    app_log( "wifi connected successful" );

    /* Start qcloud app thread */
    if(NULL == mos_thread_new( MOS_APPLICATION_PRIORITY, "qcloud_app", qcloud_app_thread, 0x4000, 0 )){
        app_log("*** start qcloud_app thread failed !!!");
    }

    exit:
    mos_thread_delete( NULL );
    return err;
}
