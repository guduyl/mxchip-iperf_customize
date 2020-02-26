/**
 ******************************************************************************
 * @file    tcp_server.c
 * @author  Yang Haibo
 * @version V1.0.0
 * @date    10-Sep-2015
 * @brief   MiCO tcp server Demo
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

#define tcp_server_log(M, ...) custom_log("TCP", M, ##__VA_ARGS__)

#define V6_ONLY_PORT 20001 /* Only accept IPv6 TCP client */
#define V6_V4_PORT   20000 /* Support IPv4&IPv6 TCP client */
#define V4_ONLY_PORT 20002 /* Only accept IPv4 TCP client */

void _WifiStatusHandler( WiFiEvent event, void* const inContext )
{
    mwifi_ip_attr_t para;
    switch ( event )
    {
        case NOTIFY_STATION_UP:
            mwifi_get_ip( &para, Station );
            tcp_server_log("Server established at ip: %s",para.localip);
            break;
        case NOTIFY_STATION_DOWN:
            case NOTIFY_AP_UP:
            case NOTIFY_AP_DOWN:
            break;
    }
}

void tcp_client_thread( void *arg )
{
    merr_t err = kNoErr;
    int fd = (int) arg;
    int len = 0;
    fd_set readfds;
    char *buf = NULL;
    struct timeval t;

    buf = (char*) malloc( 1024 );
    require_action( buf, exit, err = kNoMemoryErr );
    
    t.tv_sec = 5;
    t.tv_usec = 0;
    
    while ( 1 )
    {
        FD_ZERO( &readfds );
        FD_SET( fd, &readfds );

        require_action( select( fd+1, &readfds, NULL, NULL, &t) >= 0, exit, err = kConnectionErr );

        if ( FD_ISSET( fd, &readfds ) ) /*one client has data*/
        {
            len = recv( fd, buf, 1024, 0 );
            require_action( len >= 0, exit, err = kConnectionErr );

            if ( len == 0 )
            {
                tcp_server_log( "TCP Client is disconnected, fd: %d", fd );
                goto exit;
            }

            tcp_server_log("fd: %d, recv data %d from client", fd, len);
            len = send( fd, buf, len, 0 );
            tcp_server_log("fd: %d, send data %d to client", fd, len);
        }
    }
    exit:
    if ( err != kNoErr ) tcp_server_log( "TCP client thread exit with err: %d", err );
    if ( buf != NULL ) free( buf );
    SocketClose( &fd );
    mos_thread_delete( NULL );
}

/* TCP server listener thread */
void v4_server_thread( void *arg )
{
    merr_t err = kNoErr;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sockaddr_t_size = sizeof(client_addr);
    char client_ip_str[16];
    int tcp_listen_fd = -1, client_fd = -1;
    fd_set readfds;

    tcp_listen_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    require_action( IsValidSocket( tcp_listen_fd ), exit, err = kNoResourcesErr );

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;/* Accept conenction request on all network interface */
    server_addr.sin_port = htons( V4_ONLY_PORT );

    err = bind( tcp_listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr) );
    require_noerr( err, exit );
    
    err = listen( tcp_listen_fd, 0 );
    require_noerr( err, exit );
    
    while ( 1 )
    {
        FD_ZERO( &readfds );
        FD_SET( tcp_listen_fd, &readfds );

        require( select( tcp_listen_fd + 1, &readfds, NULL, NULL, NULL) >= 0, exit );

        if ( FD_ISSET( tcp_listen_fd, &readfds ) )
        {
            client_fd = accept( tcp_listen_fd, (struct sockaddr *) &client_addr, &sockaddr_t_size );
            if ( IsValidSocket( client_fd ) )
            {
//                inet_ntoa( client_ip_str, client_addr.s_ip );
                strcpy( client_ip_str, inet_ntoa( client_addr.sin_addr ) );
                tcp_server_log( "TCP Client %s:%d connected, fd: %d", client_ip_str, client_addr.sin_port, client_fd );
                if ( NULL == mos_thread_new(MOS_APPLICATION_PRIORITY, "TCP Clients",
                        tcp_client_thread, 0x800, client_fd ) )
                    SocketClose( &client_fd );
            }
        }
    }
    exit:
    if ( err != kNoErr ) tcp_server_log( "Server listerner thread exit with err: %d", err );
    SocketClose( &tcp_listen_fd );
    mos_thread_delete( NULL );
}


/* TCP server listener thread */
void v6_server_thread( void *arg )
{
    merr_t err = kNoErr;
    struct sockaddr_in6 server_addr, client_addr;
    socklen_t sockaddr_t_size = sizeof(client_addr);
    char client_ip_str[64];
    int tcp_listen_fd = -1, client_fd = -1;
    fd_set readfds;
    int v6only=(int)arg;

    tcp_listen_fd = socket( AF_INET6, SOCK_STREAM, IPPROTO_TCP );
    require_action( IsValidSocket( tcp_listen_fd ), exit, err = kNoResourcesErr );

    server_addr.sin6_family = AF_INET6;
    memcpy(&server_addr.sin6_addr, IP6_ADDR_ANY, sizeof(server_addr.sin6_addr));
    if (v6only == 1) {
        server_addr.sin6_port = htons( V6_ONLY_PORT);
        setsockopt(tcp_listen_fd, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(v6only));
        tcp_server_log("IPv6 ONLY on %d", V6_ONLY_PORT);
    } else {
        server_addr.sin6_port = htons( V6_V4_PORT);
    }

    err = bind( tcp_listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr) );
    require_noerr( err, exit );
    
    err = listen( tcp_listen_fd, 0 );
    require_noerr( err, exit );
    
    while ( 1 )
    {
        FD_ZERO( &readfds );
        FD_SET( tcp_listen_fd, &readfds );

        require( select( tcp_listen_fd + 1, &readfds, NULL, NULL, NULL) >= 0, exit );

        if ( FD_ISSET( tcp_listen_fd, &readfds ) )
        {
            client_fd = accept( tcp_listen_fd, (struct sockaddr *) &client_addr, &sockaddr_t_size );
            if ( IsValidSocket( client_fd ) )
            {
                if (client_addr.sin6_family == AF_INET) {
                    struct sockaddr_in v4;
                    memcpy(&v4, &client_addr, sizeof(v4));
                    inet_ntop(AF_INET, &v4.sin_addr, client_ip_str, sizeof(client_ip_str));
                } else {
                    inet_ntop(AF_INET6, &client_addr.sin6_addr, client_ip_str, sizeof(client_ip_str));
                }
                tcp_server_log( "TCP Client %s@%d connected, fd: %d", client_ip_str, client_addr.sin6_port, client_fd );
                if ( NULL == mos_thread_new( MOS_APPLICATION_PRIORITY, "TCP Clients",
                                tcp_client_thread, 0x800, client_fd ) )
                    SocketClose( &client_fd );
            }
        }
    }
    exit:
    if ( err != kNoErr ) tcp_server_log( "Server listerner thread exit with err: %d", err );
    SocketClose( &tcp_listen_fd );
    mos_thread_delete( NULL );
}

int main( void )
{
    merr_t err = kNoErr;

    /*Register user function for MiCO nitification: WiFi status changed */
    err = mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED,
                                       (void *) _WifiStatusHandler,
                                       NULL );
    require_noerr( err, exit );

    /* Start MiCO system functions according to mxos_config.h */
    err = mxos_system_init( );
    require_noerr( err, exit );

    /* Start TCP server listener thread*/
    mos_thread_new( MOS_APPLICATION_PRIORITY, "v4_server", v4_server_thread, 0x800, 0 );

    mos_thread_new( MOS_APPLICATION_PRIORITY, "dual_server", v6_server_thread,0x800,0 );

    mos_thread_new( MOS_APPLICATION_PRIORITY, "v6_server", v6_server_thread,0x800,1 );


    exit:
    mos_thread_delete( NULL );
    return err;
}

