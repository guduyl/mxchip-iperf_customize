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

// #define SSID "Xiaomi_yhb"
// #define PASSPHRASE "stm32f215"
#define SSID "test_zhy"
#define PASSPHRASE "123456789"

static mos_semphr_id_t wait_sem = NULL;

void wifi_status_notify(int event, void *arg);





/**
 * 1 - ip
 * 2 - port
 * 3 - packet length
 * iperftest 192.168.199.119 11976 1024
 **/
void test_iperf_Command(int argc, char **argv)
{
  app_log("test_iperf_Command");

  int sockfd = -1;
  struct sockaddr_in servaddr;
  int nbytes = 0; /* the number of send */
  uint8_t *buffer = NULL;

  char data_temp[] = "0123456789ABCDEF";

  size_t packet_len = atoi(argv[3]);

  int sendlen = 0;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(argv[1]);
  servaddr.sin_port = htons(atoi(argv[2]));

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    app_log("socket failed");
    goto exit;
  }

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
  {
    app_log("connect failed");
    goto exit;
  }

  buffer = (uint8_t *)malloc(packet_len + 1);
  memset(buffer, 0x00, packet_len + 1);
  for (int i = 0; i < packet_len; ++i)
  {
    buffer[i] = data_temp[i % 16];
  }

  app_log("start sending");

  while (true)
  {
    nbytes = send(sockfd, buffer, packet_len, 0);
    // app_log("%d bytes sended", nbytes);
    if (nbytes < 0)
    {
      app_log("connection lost");
      break;
    }
    else
    {
      sendlen += nbytes;
    }
  }

  app_log("sended length = %d bits", (sendlen * 8));

exit:

  if (buffer != NULL)
  {
    free(buffer);
    buffer = NULL;
  }

  if (sockfd >= 0)
  {
    close(sockfd);
  }
}

mcli_cmd_t test_cmds[] = {
    {"iperftest", "iperf ip port packet_length", test_iperf_Command},
};





int main(void)
{
  /* Initialize network(Wi-Fi, TCP/IP) */
  mxos_network_init();

  app_log("mwifi_ps_off");
  mwifi_ps_off();

  cli_init();

  mcli_cmds_add(test_cmds, sizeof(test_cmds) / sizeof(mcli_cmd_t));

  wait_sem = mos_semphr_new(1);
  /* Register Wi-Fi status notification */
  mxos_system_notify_register(mxos_notify_WIFI_STATUS_CHANGED, wifi_status_notify, NULL);

  app_log("connecting to %s...", SSID);
  /* Connect now! */
  mwifi_connect(SSID, PASSPHRASE, strlen(PASSPHRASE), NULL, NULL);

  /* Wait for wlan connection*/
  mos_semphr_acquire(wait_sem, MOS_WAIT_FOREVER);

  while (1)
  {
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
    mos_semphr_release(wait_sem);
    break;
  case NOTIFY_STATION_DOWN:
    app_log("Disonnected");
    break;
  }
}
