#include "mxos.h"
#include <breeze_export.h>
#include "StringUtils.h"

#define app_log(format, ...)  custom_log("breeze", format, ##__VA_ARGS__)

#define SOFTWARE_VERSION "0.2.0"
#define SOFTWARE_VERSION_LEN 5

#ifndef CONFIG_MODEL_SECURITY
#define PRODUCT_ID     2344792
#define DEVICE_SECRET  "Wo0yfVGC9fO2aMdgylkpVEpKff5fXj8s"
#define DEVICE_NAME    "ble_demo"
#define PRODUCT_KEY    "a1CCp8uJFqE"
#define PRODUCT_SECRET "R91CnZsinTXDbID9"
#else
#define PRODUCT_ID     577245
#define DEVICE_SECRET  "FFbidcX3hIuMMFeg70WzCdNEjZL6NZLE"
#define DEVICE_NAME    "12345678901234567890"
#define PRODUCT_KEY    "a1qWr3Wk3Oo"
#define PRODUCT_SECRET "oFTYrP7idjkYEBL9"
#endif

static bool ble_connected = false;
char awss_success_flag = 0;

static merr_t ble_status_process( )
{
    awss_success_flag = 1;
    uint8_t rsp[] = { 0x01, 0x01, 0x01 };

    if ( ble_connected )
    {
        breeze_post( rsp, sizeof(rsp) );
    }

    return kNoErr;
}

static void ble_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
    UNUSED_PARAMETER( inContext );
    switch ( status )
    {
        case NOTIFY_STATION_UP:
            app_log("get wifi station up!");
            ble_status_process();
            breeze_end( );
            // awss_success_notify( );
            break;
        case NOTIFY_STATION_DOWN:
            case NOTIFY_AP_UP:
            case NOTIFY_AP_DOWN:
            break;
    }
}

/* @brief Event handler for Ali-SDK. */
static void dev_status_changed_handler(breeze_event_t event)
{
    switch (event) {
        case CONNECTED:
            ble_connected = true;
            app_log("dev_status_changed(): Connected.\n");
            break;

        case DISCONNECTED:
            ble_connected = false;
            app_log("dev_status_changed(): Disconnected.\n");
            break;

        case AUTHENTICATED:
            app_log("dev_status_changed(): Authenticated.\n");
            break;

        case TX_DONE:
            app_log("dev_status_changed(): Tx-done.\n");
            break;

        default:
            break;
    }
}

/* @brief Data handler for control command 0x00. */
static void set_dev_status_handler(uint8_t *buffer, uint32_t length)
{
    app_log("%s command (len: %lu) received.\r\n", __func__, length);
}

/* @brief Data handler for query command 0x02. */
static void get_dev_status_handler(uint8_t *buffer, uint32_t length)
{
    app_log("%s\r\n", buffer);
    /* echo the receiving data */
    uint8_t cmd = 0x03;
    breeze_post_ext(cmd, buffer, length);
}

static void apinfo_handler(breeze_apinfo_t *ap)
{
    char *token_str;

    app_log("ssid: %s, key: %s\r\n", ap->ssid, ap->pw);

    if(ap->apptoken_len > 0)
    {
        token_str = DataToHexString(ap->apptoken, ap->apptoken_len);
        app_log("token: %s", token_str);

        // awss_set_token( ap->apptoken );

        if(token_str)
            free(token_str);
    }

    mwifi_connect(ap->ssid, ap->pw, strlen(ap->pw), NULL, NULL);

}

static void ota_handler(breeze_otainfo_t *ota)
{
    /*need to move ota logic here*/
    if(ota != NULL){
        if(ota->type == OTA_CMD){
            app_log("RECV OTA CMD\n");
        } else if(ota->type == OTA_EVT){
            app_log("RECV OTA EVT (%d)\n", ota->cmd_evt.m_evt.evt);
        } else{
            app_log("unknown ota info\r\n");
        }

    }
}

static void alink_work(void *arg)
{
    bool                 ret;
    uint32_t             err_code;
    struct device_config init_bzlink;
    uint8_t              bd_addr[BD_ADDR_LEN] = { 0 };
#ifdef CONFIG_AIS_OTA
    ota_breeze_service_manage_t ota_module;
#endif

    (void)arg;

    memset(&init_bzlink, 0, sizeof(struct device_config));
    init_bzlink.product_id        = PRODUCT_ID;
    init_bzlink.status_changed_cb = dev_status_changed_handler;
    init_bzlink.set_cb            = set_dev_status_handler;
    init_bzlink.get_cb            = get_dev_status_handler;
    init_bzlink.apinfo_cb         = apinfo_handler;


    init_bzlink.product_secret_len = strlen(PRODUCT_SECRET);
    memcpy(init_bzlink.product_secret, PRODUCT_SECRET,
           init_bzlink.product_secret_len);

    init_bzlink.product_key_len = strlen(PRODUCT_KEY);
    memcpy(init_bzlink.product_key, PRODUCT_KEY, init_bzlink.product_key_len);

    init_bzlink.device_key_len = strlen(DEVICE_NAME);
    memcpy(init_bzlink.device_key, DEVICE_NAME, init_bzlink.device_key_len);

#ifndef CONFIG_MODEL_SECURITY
    init_bzlink.secret_len = strlen(DEVICE_SECRET);
    memcpy(init_bzlink.secret, DEVICE_SECRET, init_bzlink.secret_len);
#else
    init_bzlink.secret_len = 0;
#endif

#ifdef CONFIG_AIS_OTA
    ota_module.is_ota_enable = true;
    ota_module.verison.fw_ver_len = strlen(SOFTWARE_VERSION);
    if(ota_module.verison.fw_ver_len > 8) {
        app_log("breeze version too long, make sure < 8 bytes");
        return;
    }
    memcpy(ota_module.verison.fw_ver, SOFTWARE_VERSION, ota_module.verison.fw_ver_len);
    ota_module.get_dat_cb = NULL;
    ota_breeze_service_init(&ota_module);
    init_bzlink.ota_cb = ota_module.get_dat_cb;
#else
    init_bzlink.ota_cb = ota_handler;
#endif
    ret = breeze_start(&init_bzlink);
    if (ret != 0) {
        app_log("breeze_start failed.");
    } else {
        app_log("breeze_start succeed.\r\n");
    }
}

int main( void )
{
    merr_t err = kNoErr;

    /* Initialize network(Wi-Fi, TCP/IP) */
    mxos_network_init();

    /* Register user function for MXOS nitification: WiFi status changed */
    err = mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED, (void *)ble_WifiStatusHandler, NULL );
    require_noerr( err, exit );

    alink_work(NULL);

exit:
    return err;
}


