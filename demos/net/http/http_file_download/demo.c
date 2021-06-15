#include "mxos.h"
#include "http_file_download.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

FILE_DOWNLOAD_CONTEXT http_file_context;
static mos_semphr_id_t wait_sem = NULL;

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

//file download state callback
void file_download_state_cb( void *context, HTTP_FILE_DOWNLOAD_STATE_E state, uint32_t sys_args, uint32_t user_args )
{
    int err = kGeneralErr;
    FILE_DOWNLOAD_CONTEXT file_download_context = (FILE_DOWNLOAD_CONTEXT) context;

    require(file_download_context != NULL && state < HTTP_FILE_DOWNLOAD_STATE_MAX, exit);

    app_log("file download state:%d, sys_args:%lu", state, sys_args);

    exit:
    return;
}

//file download data callback
bool file_download_data_cb( void *context, const char *data, uint32_t data_len, uint32_t user_args)
{
    int err = kGeneralErr;
    FILE_DOWNLOAD_CONTEXT file_download_context = (FILE_DOWNLOAD_CONTEXT)context;

    app_log("file download, total len:%lu, get:%lu, len:%lu", file_download_context->file_info.file_total_len, file_download_context->file_info.download_len, data_len);

    return true;
}

static void file_download_cmd( int argc, char** argv )
{
    int err = kGeneralErr;

    if ( argc == 2 )
    {
        err = http_file_download_start( &http_file_context, argv[1], file_download_state_cb, file_download_data_cb, 0 );
        require_noerr_string( err, exit, "http file download error" );
    }else
    {
        app_log("param error!");
    }

    exit:
    return;
}


mcli_cmd_t file_download_cli = {
    .name = "file_download",
    .help = "file_download <url>",
    .func = file_download_cmd,
};

int main( void )
{
    wait_sem = mos_semphr_new( 1 );

    mxos_system_init( );

    mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED, wifi_status_notify, NULL );

    mos_semphr_acquire( wait_sem, MOS_WAIT_FOREVER );

    mcli_cmd_add( &file_download_cli );

    return 0;
}
