/**
 ******************************************************************************
 * @file    main.c
 * @author  Snow Yang
 * @version V1.0.0
 * @date    21-May-2019
 * @brief   Wi-Fi SoftAP demo.
 ******************************************************************************
 */

#include "mxos.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

#define SOFTAP_SSID "areyouok?"
#define SOFTAP_KEY "66666666"

void wifi_status_notify(int event, void *arg);

int main(void)
{
    mwifi_ip_attr_t ip_attr;

    /* Initialize network(Wi-Fi, TCP/IP) */
    mxos_network_init();

    /* Register Wi-Fi status notification */
    mxos_system_notify_register(mxos_notify_WIFI_STATUS_CHANGED, wifi_status_notify, NULL);

    /* Set SoftAP IP address */
    strcpy((char *)ip_attr.localip, "192.168.0.1");
    strcpy((char *)ip_attr.netmask, "255.255.255.0");
    strcpy((char *)ip_attr.dnserver, "192.168.0.1");
    strcpy((char *)ip_attr.gateway, "192.168.0.1");

    app_log("Establish Sodt AP, SSID:%s and KEY:%s", SOFTAP_SSID, SOFTAP_KEY);
    /* Start SoftAP now! */
    mwifi_softap_start(SOFTAP_SSID, SOFTAP_KEY, 6, &ip_attr);

    return 0;
}

void wifi_status_notify(int event, void *arg)
{
    switch (event)
    {
    case NOTIFY_AP_UP:
        app_log("SoftAP established");
        break;
    case NOTIFY_AP_DOWN:
        app_log("SoftAP down");
        break;
    }
}