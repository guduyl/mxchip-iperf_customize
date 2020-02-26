/**
 ******************************************************************************
 * @file    main.c
 * @author  Yang Haibo
 * @version V1.0.0
 * @date    26-Feb-2020
 * @brief   Wi-Fi WPS demo.
 ******************************************************************************
 */

#include "mxos.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

void wifi_status_notify(int event, void *arg);

static void start_cmd_handler(int argc, char **argv)
{
    mcli_printf("Start WPS");
    mwifi_wps_connect();
}

static void stop_cmd_handler(int argc, char **argv)
{
    mcli_printf("Stop WPS");
    mwifi_wps_stop();
}


static const mcli_cmd_t usr_clis[] = {
    {"start", "Start WPS mode", start_cmd_handler},
    {"stop", "Stop WPS mode", stop_cmd_handler},
};

int main(void)
{
  /* Initialize network(Wi-Fi, TCP/IP) */
  mxos_network_init();
  cli_init();
  wifimgr_debug_enable(1);
  /* Register Wi-Fi status notification */
  mxos_system_notify_register(mxos_notify_WIFI_STATUS_CHANGED, wifi_status_notify, NULL);
  mcli_cmds_add(usr_clis, sizeof(usr_clis)/sizeof(mcli_cmd_t));
  
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
