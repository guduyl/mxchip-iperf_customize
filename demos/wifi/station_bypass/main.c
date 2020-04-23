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
#include "mxos_network_bypass.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

#define SSID "Xiaomi_yhb"
#define PASSPHRASE "stm32f215"

void wifi_status_notify(int event, void *arg);


typedef void (*ping_result_t)(int32_t each_delay_ms[], int num);

void _ping_cb(int32_t each_delay_ms[], int num)
{
    int i;
    for(i=0;i<num;i++) {
        app_log("[%d] delay %d", i, each_delay_ms[i]);
    }
}

int net_ping(char *ip, int32_t count, int32_t delay, ping_result_t cb);

int netlink_input(int idx, struct pbuf*p)
{
    app_log("idx %d, input p: (%d)", idx, p->tot_len);
}

int main(void)
{
  set_network_bypass_mode(1);
  /* Initialize network(Wi-Fi, TCP/IP) */
  mxos_network_init();
  cli_init();
  wifimgr_debug_enable(1);
  /* Register Wi-Fi status notification */
  mxos_system_notify_register(mxos_notify_WIFI_STATUS_CHANGED, wifi_status_notify, NULL);

  app_log("connecting to %s...", SSID);
  /* Connect now! */
  mwifi_connect(SSID, PASSPHRASE, strlen(PASSPHRASE), NULL, NULL);

  sleep(5);
  //net_ping("www.baidu.com", 10, 200, _ping_cb);
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
