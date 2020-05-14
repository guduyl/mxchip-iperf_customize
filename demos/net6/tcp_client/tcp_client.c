/**
 ******************************************************************************
 * @file    tcp_client.c
 * @author  Yang Haibo
 * @version V1.0.0
 * @date    10-Sep-2019
 * @brief   MiCO IPv6 tcp client Demo
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
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
#include "SocketUtils.h"

#define tcp_client_log(M, ...) custom_log("TCP", M, ##__VA_ARGS__)

#define TCP_REMOTE_PORT_V4_V6   (20000) /* V4/V6 server port */
#define TCP_REMOTE_PORT_V6_ONLY (9001)//(20001) /* V6 server port */
#define TCP_REMOTE_PORT_V4_ONLY (20002) /* V4 server port */

// static char *tcp_remote_ip = "2408:84e2:159:db22:b2f8:93ff:fe20:c7b7"; /* server ip address */
// static char *tcp_remote_ip = "fe80::e0:4cff:fe86:8b64"; /* server ip address */
static char *tcp_remote_ip = "2001:2::aab1:8ed:832e:572f:6883"; /* server ip address */

static mos_semphr_id_t wait_sem = NULL;

static void _WifiStatusHandler( WiFiEvent status, void* const inContext )
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
void tcp_client_thread( void *arg )
{
    merr_t err;
    struct sockaddr_in6 addr;
    struct timeval t;
    fd_set readfds;
    int tcp_fd = -1, len;
    char *buf = NULL;
    int v6only = (int)arg;
    int remote_port;

    buf = (char*) malloc( 1024 );
    require_action( buf, exit, err = kNoMemoryErr );

AGAIN:
    tcp_fd = socket( AF_INET6, SOCK_STREAM, IPPROTO_TCP );
    require_action( IsValidSocket( tcp_fd ), exit, err = kNoResourcesErr );

    addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, tcp_remote_ip, &addr.sin6_addr);
    if(1 == v6only)
    {
        remote_port = TCP_REMOTE_PORT_V6_ONLY;
    }
    else
    {
       remote_port = TCP_REMOTE_PORT_V4_V6;
    }
    addr.sin6_port = htons(remote_port);

    tcp_client_log( "Connecting to server: ip=%s  port=%d ...", tcp_remote_ip, remote_port );
    err = connect( tcp_fd, (struct sockaddr *)&addr, sizeof(addr) );
    if (err != kNoErr) {
        tcp_client_log("*** connect error %d !", err);
        close(tcp_fd);
        mos_sleep(2);
        goto AGAIN;
    }
    tcp_client_log( "Connect %s success!", (1 == v6only) ? "V6_ONLY" : "V4/V6" );
    
    t.tv_sec = 2;
    t.tv_usec = 0;
    
    while ( 1 )
    {
        FD_ZERO( &readfds );
        FD_SET( tcp_fd, &readfds );

        require_action( select( tcp_fd + 1, &readfds, NULL, NULL, &t) >= 0, exit,
                        err = kConnectionErr );

        /* recv wlan data, and send back */
        if ( FD_ISSET( tcp_fd, &readfds ) )
        {
            len = recv( tcp_fd, buf, 1024, 0 );
            require_action( len >= 0, exit, err = kConnectionErr );

            if ( len == 0 )
            {
                tcp_client_log( "TCP Client is disconnected, fd: %d", tcp_fd );
                goto exit;
            }

            tcp_client_log("Client fd: %d, recv data %d", tcp_fd, len);
            len = send( tcp_fd, buf, len, 0 );
            tcp_client_log("Client fd: %d, send data %d", tcp_fd, len);
        }
    }

    exit:
    if ( err != kNoErr ) tcp_client_log( "TCP client thread exit with err: %d", err );
    if ( buf != NULL ) free( buf );
    SocketClose( &tcp_fd );
    mos_thread_delete( NULL );
}

int main( void )
{
    merr_t err = kNoErr;

    wait_sem = mos_semphr_new(1);

    /*Register user function for MiCO nitification: WiFi status changed */
    err = mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED,
                                       (void *) _WifiStatusHandler, NULL );
    require_noerr( err, exit );

    /* Start MiCO system functions according to mxos_config.h */
    err = mxos_system_init(  );
    require_noerr( err, exit );

    /* Wait for wlan connection*/
    mos_semphr_acquire( wait_sem, MOS_WAIT_FOREVER );
    tcp_client_log( "wifi connected successful" );

    /* Start TCP client thread */
    // mos_thread_new( MOS_APPLICATION_PRIORITY, "TCP_client_v4_v6", tcp_client_thread, 0x800, 0 );

    mos_thread_new( MOS_APPLICATION_PRIORITY, "TCP_client_v6", tcp_client_thread, 0x800, 1 );

    exit:
    if ( wait_sem != NULL )
        mos_semphr_delete( wait_sem );
    mos_thread_delete( NULL );
    return err;
}

