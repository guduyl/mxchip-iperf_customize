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

#define SSID "ST3080"
#define PASSPHRASE "stm32l475"

static mos_semphr_id_t wait_sem = NULL;

void wifi_status_notify(int event, void *arg);

int main(void)
{
  /* Initialize network(Wi-Fi, TCP/IP) */
  mxos_network_init();

  app_log("mwifi_ps_off");
  mwifi_ps_off();

  wait_sem = mos_semphr_new(1);
  /* Register Wi-Fi status notification */
  mxos_system_notify_register(mxos_notify_WIFI_STATUS_CHANGED, wifi_status_notify, NULL);

  app_log("connecting to %s...", SSID);
  /* Connect now! */
  mwifi_connect(SSID, PASSPHRASE, strlen(PASSPHRASE), NULL, NULL);

  /* Wait for wlan connection*/
  mos_semphr_acquire( wait_sem, MOS_WAIT_FOREVER );

  while(1){
    mos_sleep(10);
    app_log("mwifi_ps_on");
    mwifi_ps_on();
     
    mos_sleep(10);
    app_log("mwifi_ps_off");
    mwifi_ps_off();
  }

  return 0;
}

void wifi_status_notify(int event, void *arg)
{
  switch (event)
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
