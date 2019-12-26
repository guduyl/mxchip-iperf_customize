/**
 ******************************************************************************
 * @file    main.c
 * @author  Snow Yang
 * @version V1.0.0
 * @date    21-May-2019
 * @brief   Wi-Fi scan demo.
 ******************************************************************************
 */

#include "mxos.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

static void wifi_scan_notify(int num, mwifi_ap_info_t *ap_list);

int main(void)
{
    /* Initialize network(Wi-Fi, TCP/IP) */
    mxos_network_init();

    /* Register Wi-Fi scan notification */
    mxos_system_notify_register(mxos_notify_WIFI_SCAN_COMPLETED, wifi_scan_notify, NULL);

    app_log("Start scan, please wait...");
    /* Scan now! */
    mwifi_scan(NULL);

    return 0;
}

static void wifi_scan_notify(int num, mwifi_ap_info_t *ap_list)
{
    int i = 0;
    app_log("AP number: %d", num);
    for (i = 0; i < num; i++)
    {
        app_log("AP%d: SSID=%s, RSSI=%ddBm", i, ap_list[i].ssid, ap_list[i].rssi);
    }
}