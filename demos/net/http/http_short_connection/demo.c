#include "mxos.h"
#include "http_short_connection.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

static mos_semphr_id_t wait_sem = NULL;
static uint8_t revbuf[20480] = {0};

void wifi_status_notify( int event, void *arg )
{
    switch ( event )
    {
        case NOTIFY_STATION_UP:
            app_log("Connected");
            mos_semphr_release( wait_sem );
            break;
        case NOTIFY_STATION_DOWN:
            app_log("Disonnected");
            break;
    }
}


#define SIMPLE_GET_REQUEST    \
    "GET / HTTP/1.1\r\n"      \
    "Host: www.baidu.com\r\n" \
    "Connection: close\r\n"   \
    "\r\n"

void http_short_connection_test(void)
{
    int err = kGeneralErr;
    HTTP_REQ_S http_req;

    memset( &http_req, 0, sizeof(http_req) );
    memset( revbuf, 0, sizeof(revbuf) );

    http_req.domain_name = "www.baidu.com";
    http_req.port = 443;
    http_req.is_success = false;
    http_req.is_ssl = true;
    http_req.is_sni = true;
    http_req.timeout_ms = 5000;
    http_req.user_args = 0;

    http_req.http_req = SIMPLE_GET_REQUEST;
    http_req.req_len = strlen( SIMPLE_GET_REQUEST );
    http_req.http_res = revbuf;
    http_req.res_len = sizeof(revbuf);

//    app_log("http request, len:%ld, data:\r\n%s", http_req.req_len, http_req.http_req);

    err = http_short_connection_ssl( &http_req );
    require_noerr( err, exit );

    require_action_string( http_req.is_success == true && http_req.status_code == 200, exit, err = kGeneralErr, "http request error!" );
    app_log( "http response:[%ld]%s", http_req.real_res_len, http_req.http_res );

    exit:
    return;
}

int main( void )
{
    wait_sem = mos_semphr_new( 1 );

    mxos_system_init( );

    mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED, wifi_status_notify, NULL );

    mos_semphr_acquire( wait_sem, MOS_WAIT_FOREVER );

    http_short_connection_test( );

    return 0;
}
