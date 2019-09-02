#include "mxos.h"
#include "smtpclient.h"

#define smtp_log(M, ...) custom_log("SMTP", M, ##__VA_ARGS__)

#define SMTP_SERVER_ADDR "smtp.domainname.com"
#define SMTP_SERVER_PORT 465
#define SMTP_SENDER_ADDR "user_name@domainname.com"
//#define SMTP_SENDER_PASS "bunengshuodemimi"
#define SMTP_SENDER_PASS "smtppass"

static mos_semphr_id_t wait_sem = NULL;

int smtp_test()
{
    SMTP* smtp;
    int i = 0;
    char subject[64];
    
    smtp_log("start smtp test");
    smtp = SmtpInit(SMTP_SERVER_ADDR, SMTP_SERVER_PORT, SMTP_SENDER_ADDR, 
      SMTP_SENDER_ADDR, SMTP_SENDER_PASS);

    if (smtp == NULL) {
        smtp_log("smtp init error");
        return -1;
    }

    SmtpSetSmtpMode(smtp, SMTP_SECURITY_TLS);
    SmtpAddCc(smtp, SMTP_SENDER_ADDR);
    SmtpAddTo(smtp, "haibo344@sina.com");
    while(1) {
        sprintf(subject, "subject %d", i);
        SmtpSendMail(smtp, subject, "body");
        i++;
        if (i > 2)
            break;
    }

    smtp_log("smtp sent");
    SmtpDeinit(smtp);

    return 0;
}


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
    smtp_log( "wifi connected successful" );

    /* Start SMTP test */
    smtp_test();
    
    exit:
    if ( wait_sem ) mos_semphr_delete( wait_sem );
    return err;
}



