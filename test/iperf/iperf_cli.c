/**
 ******************************************************************************
 * @file    iperf_cli.c
 * @author  Libo
 * @version V1.0.0
 * @date    5-Jan-2018
 * @brief   This file provide the iperf command line functions.
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

#include "cli/mcli.h"

#include "iperf_debug.h"
#include "iperf_task.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/


/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
void iperf_Command( int argc, char **argv );
static void ap_command(int argc, char **argv);
static void conn_command(int argc, char **argv);

static void ce_command(int argc, char **argv) 
{
#ifdef CONFIG_MX1290
    wext_set_adaptivity(1);
    wifi_set_country(10);   
#endif
}

mcli_cmd_t iperf_test_message_cmd[] = {
    {"ce", "enable CE test", ce_command}, 
    {"ap", "<SSID> [<key>] start soft-AP mode", ap_command},
    {"conn", "<SSID> [<key>] connect to an AP", conn_command},
    { "iperf", "iperf test", iperf_Command },
};

/******************************************************
 *               Function Declarations
 ******************************************************/
static void iperf_udp_run_server_thread( void* arg );
static void iperf_tcp_run_server_thread( void* arg );
static void iperf_udp_run_client_thread( void* arg );
static void iperf_tcp_run_client_thread( void* arg );

static void _cli_iperf_server_Command( int argc, char **argv );
static void _cli_iperf_client_Command( int argc, char **argv );
static void _cli_iperf_help_Command( int argc, char **argv );

/******************************************************
 *               Variables Definitions
 ******************************************************/
extern uint8_t uap_up;
/******************************************************
 *               Function Definitions
 ******************************************************/

static void iperf_udp_run_server_thread( void* arg )
{
    iperf_udp_run_server( (char **) arg );
}

static void iperf_tcp_run_server_thread( void* arg )
{
    iperf_tcp_run_server( (char **) arg );
}

static void iperf_udp_run_client_thread( void* arg )
{
    iperf_udp_run_client( (char **) arg );
}

static void iperf_tcp_run_client_thread( void* arg )
{
    iperf_tcp_run_client( (char **) arg );
}

static void _cli_iperf_server_Command( int argc, char **argv )
{
    int i;
    char **g_iperf_param = NULL;
    int is_create_task = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    g_iperf_param = malloc( IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );
    if ( g_iperf_param == NULL )
    {
        mcli_printf( "Warning: No enough memory to running iperf.\r\n" );
    }
    memset( g_iperf_param, 0, IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );

    for ( i = 0; i < argc; i++ )
    {
        strcpy( (char *) &g_iperf_param[i * offset], argv[i] );
#if defined(IPERF_DEBUG_INTERNAL)
        mcli_printf("_cli_iperf_client, g_iperf_param[%d] is \"%s\"\r\n", i, (char *)&g_iperf_param[i * offset]);
#endif
    }

    for ( i = 0; i < argc; i++ )
    {
        if ( strcmp( argv[i], "-u" ) == 0 )
        {
            mcli_printf( "Iperf UDP Server: Start!\r\n" );
            mcli_printf( "Iperf UDP Server Receive Timeout = 20 (secs)\r\n" );
            mos_thread_new(IPERF_PRIO, IPERF_NAME, iperf_udp_run_server_thread, IPERF_STACKSIZE,
                                     (void*) g_iperf_param );
            is_create_task = 1;
            break;
        }
    }
    if ( is_create_task == 0 )
    {
        mcli_printf( "Iperf TCP Server: Start!\r\n" );
        mcli_printf( "Iperf TCP Server Receive Timeout = 20 (secs)\r\n" );
        mos_thread_new(IPERF_PRIO, IPERF_NAME, iperf_tcp_run_server_thread, IPERF_STACKSIZE,
                                 (void*) g_iperf_param );
        is_create_task = 1;
    }

    if ( is_create_task == 0 )
    {
        free( g_iperf_param );
    }
}

static void _cli_iperf_client_Command( int argc, char **argv )
{
    int i;
    char **g_iperf_param = NULL;
    int is_create_task = 0;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    
    g_iperf_param = malloc( IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );
    if ( g_iperf_param == NULL )
    {
        mcli_printf( "Warning: No enough memory to running iperf.\r\n" );
    }
    memset( g_iperf_param, 0, IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE );
    for ( i = 0; i < argc; i++ )
    {
        strcpy( (char *) &g_iperf_param[i * offset], argv[i] );
#if defined(IPERF_DEBUG_INTERNAL)
        mcli_printf("_cli_iperf_client, g_iperf_param[%d] is \"%s\"\r\n", i, (char *)&g_iperf_param[i * offset]);
#endif
    }

    for ( i = 0; i < argc; i++ )
    {
        if ( strcmp( argv[i], "-u" ) == 0 )
        {
            mcli_printf( "Iperf UDP Client: Start!\r\n" );
            mos_thread_new(IPERF_PRIO, IPERF_NAME, iperf_udp_run_client_thread, IPERF_STACKSIZE,
                                     (void*) g_iperf_param );
            is_create_task = 1;
            break;
        }
    }

    if ( is_create_task == 0 ) {
        mcli_printf( "Iperf TCP Client: Start!\r\n" );
        mos_thread_new(IPERF_PRIO, IPERF_NAME, iperf_tcp_run_client_thread, IPERF_STACKSIZE,
                                 (void*) g_iperf_param );
        is_create_task = 1;
    }

    if ( is_create_task == 0 )
    {
        free( g_iperf_param );
    }
}

static void _cli_iperf_help_Command( int argc, char **argv )
{
    mcli_printf( "Usage: iperf [-s|-c] [options]\r\n" );
    mcli_printf( "       iperf [-h]\r\n\n" );
    mcli_printf( "Client/Server:\r\n" );
    mcli_printf( "  -u,        use UDP rather than TCP\r\n" );
    mcli_printf( "  -p,        #server port to listen on/connect to (default 5001)\r\n" );
    mcli_printf( "  -n,        #[kmKM]    number of bytes to transmit \r\n" );
    mcli_printf( "  -b,        #[kmKM]    for UDP, bandwidth to send at in bits/sec\r\n" );
    mcli_printf( "  -i,        10 seconds between periodic bandwidth reports \r\n\n" );
    mcli_printf( "Server specific:\r\n" );
    mcli_printf( "  -s,        run in server mode\r\n" );
    mcli_printf( "  -B,        <ip>    bind to <ip>, and join to a multicast group (only Support UDP)\r\n" );
    mcli_printf( "  -r,        for UDP, run iperf in tradeoff testing mode, connecting back to client\r\n\n" );
    mcli_printf( "Client specific:\r\n" );
    mcli_printf( "  -c,        <ip>run in client mode, connecting to <ip>\r\n" );
    mcli_printf( "  -w,        #[kmKM]    TCP window size\r\n" );
    mcli_printf( "  -l,        #[kmKM]    UDP datagram size\r\n" );
    mcli_printf( "  -t,        #time in seconds to transmit for (default 10 secs)\r\n" );
    mcli_printf( "  -S,        #the type-of-service of outgoing packets\r\n\n" );
    mcli_printf( "Miscellaneous:\r\n" );
    mcli_printf( "  -h,        print this message and quit\r\n\n" );
    mcli_printf( "[kmKM] Indicates options that support a k/K or m/M suffix for kilo- or mega-\r\n\n" );
    mcli_printf( "TOS options for -S parameter:\r\n" );
    mcli_printf( "BE: -S 0\r\n" );
    mcli_printf( "BK: -S 32\r\n" );
    mcli_printf( "VI: -S 160\r\n" );
    mcli_printf( "VO: -S 224\r\n\n" );
    mcli_printf( "Tradeoff Testing Mode:\r\n" );
    mcli_printf( "Command: iperf -s -u -n <bits/bytes> -r \r\n\n" );
    mcli_printf( "Example:\r\n" );
    mcli_printf( "Iperf TCP Server: iperf -s\r\n" );
    mcli_printf( "Iperf UDP Server: iperf -s -u\r\n" );
    mcli_printf( "Iperf TCP Client: iperf -c <ip> -w <window size> -t <duration> -p <port> \r\n" );
    mcli_printf( "Iperf UDP Client: iperf -c <ip> -u -l <datagram size> -t <duration> -p <port>\r\n" );
}

#if defined(MICO_IPERF_DEBUG_ENABLE)
static uint8_t _cli_iperf_debug(int argc, char **argv)
{
    int debug;
    debug = atoi(argv[0]);
    mcli_printf("Set iperf debug to %d(0x%x)\r\n", debug, debug);
    iperf_set_debug_mode(debug);
    return 0;
}
#endif

void iperf_Command(int argc, char **argv )
{
    if ( argc < 2 )
    {
        mcli_printf( "Invalid command\r\n" );
    }
    if ( strcmp( argv[1], "-s" ) == 0 )
    {
        _cli_iperf_server_Command( argc - 2, &argv[2] );
    }
    else
    if ( strcmp( argv[1], "-c" ) == 0 )
    {
        _cli_iperf_client_Command( argc - 2, &argv[2] );
    }
    else
    if ( strcmp( argv[1], "-h" ) == 0 )
    {
        _cli_iperf_help_Command( argc - 2, &argv[2] );
    }
#if defined(MICO_IPERF_DEBUG_ENABLE)
    else
    if ( strcmp( argv[1], "-d" ) == 0 )
    {
        _cli_iperf_debug( argc - 2, &argv[2] );
    }
#endif
}


merr_t iperf_cli_register( void )
{
    if( 0 == mcli_cmds_add( iperf_test_message_cmd, sizeof(iperf_test_message_cmd)/sizeof(mcli_cmd_t) ))
        return kNoErr;
    else
        return kGeneralErr;
}

/*++++++++++++++++ ADD BY SWYANG@MICO@MXCHIP ++++++++++++++++*/
/*
Add CLI commands to connect an AP or start soft-AP
*/

static void connect_ap(char *ssid, char *key)
{
    /* Connect Now! */
    mcli_printf ("connecting to %s...\r\n", ssid);
    mwifi_connect(ssid, key, strlen(key), NULL, NULL);
}

static void ap_start(char *ssid, char *key)
{
    mwifi_ip_attr_t ip;
    strcpy(ip.localip, "192.168.0.1");
    strcpy(ip.netmask, "255.255.255.0");
    strcpy(ip.dnserver, "192.168.0.1");

    mcli_printf("start softap: ssid:%s  key:%s\r\n", ssid, key);
    mwifi_softap_start(ssid, key, 1, &ip);
}

static void ap_command(int argc, char **argv)
{
    if (uap_up == 1)
        return;
    
    if (argc == 2) {
        mcli_printf("start AP ssid=%s\r\n", argv[1]);
        ap_start(argv[1], NULL);
    } else if (argc == 3) {
        mcli_printf("start AP ssid=%s, key=%s\r\n", argv[1], argv[2]);
        ap_start(argv[1], argv[2]);
    } else {
        mcli_printf("Usage: ap <ssid> [<key>]");
    }
}

static void conn_command(int argc, char **argv)
{
    if (argc == 2) {
        mcli_printf("Connect to AP ssid=%s\r\n", argv[1]);
        connect_ap(argv[1], NULL);
    } else if (argc == 3) {
        mcli_printf("Connect to AP ssid=%s, key=%s\r\n", argv[1], argv[2]);
        
        connect_ap(argv[1], argv[2]);
    } else {
        mcli_printf("Usage: connect <ssid> [<key>]");
    }
}
