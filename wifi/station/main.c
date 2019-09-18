/**
 ******************************************************************************
 * @file    main.c
 * @author  Snow Yang
 * @version V1.0.0
 * @date    21-May-2019
 * @brief   Wi-Fi station demo.
 ******************************************************************************
 */

#include "mxos.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

#define SSID "snowyang"
#define PASSPHRASE "mxchip123"

void wifi_status_notify(int event, void *arg);

int main(void)
{
  /* Initialize network(Wi-Fi, TCP/IP) */
  mxos_network_init();

  /* Register Wi-Fi status notification */
  mxos_system_notify_register(mxos_notify_WIFI_STATUS_CHANGED, wifi_status_notify, NULL);

  app_log("connecting to %s...", SSID);
  /* Connect now! */
  mwifi_connect(SSID, PASSPHRASE, strlen(PASSPHRASE), NULL, NULL);

  return 0;
}

void wifi_status_notify(int event, void *arg)
{
  switch (event)
  {
  case NOTIFY_STATION_UP:
    app_log("Connected");
    break;
  case NOTIFY_STATION_DOWN:
    app_log("Disonnected");
    break;
  }
}