/**
 ******************************************************************************
 * @file    main.c
 * @author  Yang Haibo
 * @version V1.0.0
 * @date    08-Jan-2020
 * @brief   Wi-Fi WPA enterprise demo.
 ******************************************************************************
 */

#include "mxos.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

#define SSID "wifitest"
#define PASSPHRASESSID "snowyang"
#define PASSPHRASE "mxchip123"

const char  *client_cert = \
    "-----BEGIN CERTIFICATE-----\r\n" \
    "MIIDzzCCAregAwIBAgIBAjANBgkqhkiG9w0BAQsFADCBkzELMAkGA1UEBhMCRlIx\r\n" \
    "DzANBgNVBAgMBlJhZGl1czESMBAGA1UEBwwJU29tZXdoZXJlMRUwEwYDVQQKDAxF\r\n" \
    "eGFtcGxlIEluYy4xIDAeBgkqhkiG9w0BCQEWEWFkbWluQGV4YW1wbGUub3JnMSYw\r\n" \
    "JAYDVQQDDB1FeGFtcGxlIENlcnRpZmljYXRlIEF1dGhvcml0eTAeFw0yMDA0MjIw\r\n" \
    "ODE2MzZaFw0yMDA2MjEwODE2MzZaMHExCzAJBgNVBAYTAkZSMQ8wDQYDVQQIDAZS\r\n" \
    "YWRpdXMxFTATBgNVBAoMDEV4YW1wbGUgSW5jLjEZMBcGA1UEAwwQdXNlckBleGFt\r\n" \
    "cGxlLm9yZzEfMB0GCSqGSIb3DQEJARYQdXNlckBleGFtcGxlLm9yZzCCASIwDQYJ\r\n" \
    "KoZIhvcNAQEBBQADggEPADCCAQoCggEBAMWJ+bCciMxKzioR+crNJuj8RYAUL8ZT\r\n" \
    "MPyK8RMf+8+SOJJfxWqzcFfgwCc/ZwoDCiXr/YOxLo4afFpA2jvKsNFO/4UCIjf8\r\n" \
    "YV1F99sCL3nYfrXHxK3mkuA3GkEuNIdfx78GwUEUvvgOztgewaTkWVSKczqFscHt\r\n" \
    "8j0vhHwFq3O+ihP14YfhTWSId88+27e30zXCoyZ9Y9AtyVZlA1GoEjS0svkLjQZO\r\n" \
    "dwbmupLJV64JaCidIXUXDJ2OkFEXd0iWqOGhGG6I8kzjbIV8AOcYjcSzjHG+unfR\r\n" \
    "hEJCzFmK/ADTCc0Zi+0aU5YvPqeRN5FHpcZ4gvcfBRaBxiAB72EYdzcCAwEAAaNP\r\n" \
    "ME0wEwYDVR0lBAwwCgYIKwYBBQUHAwIwNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDov\r\n" \
    "L3d3dy5leGFtcGxlLmNvbS9leGFtcGxlX2NhLmNybDANBgkqhkiG9w0BAQsFAAOC\r\n" \
    "AQEAdJfYulJyqk/4+7rXNUw9MD9WisCVU0r+TyolR7wNw5qaQHaVn3PZO9yJ5KLe\r\n" \
    "Ghqq9WGwBKN1NsGvrMuCfuL/p5g4jTC3TpxMRBrdvfqGhXEpscUpSRMhpSKnh+h1\r\n" \
    "+gmIvd0rK4SmDpPxh5sSs9Gwk+cj6HBokRc0HL+iUOhT+oah3A95Jmxgb5THH2Vl\r\n" \
    "naUQDai5GSp6nkDf7Mvouj8BdSN1y6IDadqt1+dudmIfTKpRSXeru9bqnNHmrw9y\r\n" \
    "3M1SAbIlhsn00A6cVd04VlxHNyYspvE7/AUi4ERuG4QMCABQQp6cxxagzW1gB5xa\r\n" \
    "lNfvV4PSngWxj6ZNlsYYXjMnbQ==\r\n" \
    "-----END CERTIFICATE-----\r\n";

const char  *client_key = \
    "-----BEGIN RSA PRIVATE KEY-----\r\n" \
    "MIIEowIBAAKCAQEAxYn5sJyIzErOKhH5ys0m6PxFgBQvxlMw/IrxEx/7z5I4kl/F\r\n" \
    "arNwV+DAJz9nCgMKJev9g7Eujhp8WkDaO8qw0U7/hQIiN/xhXUX32wIvedh+tcfE\r\n" \
    "reaS4DcaQS40h1/HvwbBQRS++A7O2B7BpORZVIpzOoWxwe3yPS+EfAWrc76KE/Xh\r\n" \
    "h+FNZIh3zz7bt7fTNcKjJn1j0C3JVmUDUagSNLSy+QuNBk53Bua6kslXrgloKJ0h\r\n" \
    "dRcMnY6QURd3SJao4aEYbojyTONshXwA5xiNxLOMcb66d9GEQkLMWYr8ANMJzRmL\r\n" \
    "7RpTli8+p5E3kUelxniC9x8FFoHGIAHvYRh3NwIDAQABAoIBAQDAsr7zbU/5UgPZ\r\n" \
    "gF2XWxdY8jbuIUMVibPgt71B9dJUA4TSLUDMvKyb3TA1WSEv4BU3NwXAQMh0cRvb\r\n" \
    "gyYga7KfDBww6QruaHGmZonJiFE94FaDB5QyPBoNRVOEQAmYAXQ+Qd2lmhJMs01O\r\n" \
    "AHrQWr1VvhV4AlGMJ41F6KOpxHIjXgY0T1zPQ4sEsoQ7ILq1N6gNolxoOWvWgvGN\r\n" \
    "GpgEuKbQutgZpmwqSG2rjuw8OnEnz1q/RdKHyOQ4qls+K7qaWj96HDrozHQRdyhw\r\n" \
    "EjGTG2gyDDuhmSoXa1LnVDYusJt+p8DiZKkYqOVMKEjG3zHbQiD5fLa4YCWT4p4q\r\n" \
    "fimQ9f8pAoGBAOdI0ihd4jce05rozFqcQcKQDteIulP3I+ni+3ug1s+Eis/i4c5Y\r\n" \
    "hhwzXq1RoJXNbryRtZAN2KHZMo9oMcRmDpxSdbrdcYcyYItr22eRJ03ybUACIvq4\r\n" \
    "qamnTD/rlNu4lVlFL4CrGNn9DJ1Dc2gxHwWJUpoTkAoNyNlD0ydgYEP9AoGBANql\r\n" \
    "/T3H0CvZwID0WK9lu1e2+KZB9Kavy4NsZWWoIBhotkJSwiA4nzf4ASCL9g2aHDIY\r\n" \
    "kfDBc43oHJNWZ0eFU9p84wkSUT3CTaFZt8kO1nmobYKYVEDPv3bOn0/9H+cD8T2x\r\n" \
    "zBXxruXsOf3sFk+h0EQHa14mpbq0X+i4HEwOgxxDAoGAYkm9HfB/8SLFlGB4b81V\r\n" \
    "2OQ1mnz9ii9N8NhIPUql3zyZbS0i/vj5zUQ7aPZ2gxLxi2t0uEpaFLXj2bPS6n7g\r\n" \
    "mKgmZgozc9wB0V49uG6kQGjU6s29pcUSPQneyo9IDHQN7AkN3eVYQ1NZp/+BLPcr\r\n" \
    "EsQ/+UB1BwDCqSo8stAvONUCgYBzg76HJUmV4TUS7ATupZCUhtZXAAGBDt82T85C\r\n" \
    "JtDOFIQ2BgMJguCbcSsnRsp35TOAQ/rWd13VWwu9y6hE4OusQs6vvD2vozfgRsYS\r\n" \
    "MwScwZ8Zn+DjXN6Kph8E43Y3Vp3dOlHYRhdg4azBujkPWS3uwvy2gTSNrz38dUh9\r\n" \
    "5izegQKBgF4yWb+kEM7yQOOUay903ubdcyhN3Ggkgev/N6m3wqLUppDh3+zjQTJn\r\n" \
    "/LbqfNvXYhn5eIEpHCoJkxaMmYPe14UBY78ejLqq+Keh/LZvWJ3c7Mb6ydfwfjmd\r\n" \
    "vEFaTnhm8r6WTRem/f+TnPEqIhcggp8HvImTU5h6kVE5uoWNVMFT\r\n" \
    "-----END RSA PRIVATE KEY-----\r\n";

const char  *rootca =
    "-----BEGIN CERTIFICATE-----\r\n" \
    "MIIE5DCCA8ygAwIBAgIJAO1EzOg/Zx8OMA0GCSqGSIb3DQEBCwUAMIGTMQswCQYD\r\n" \
    "VQQGEwJGUjEPMA0GA1UECAwGUmFkaXVzMRIwEAYDVQQHDAlTb21ld2hlcmUxFTAT\r\n" \
    "BgNVBAoMDEV4YW1wbGUgSW5jLjEgMB4GCSqGSIb3DQEJARYRYWRtaW5AZXhhbXBs\r\n" \
    "ZS5vcmcxJjAkBgNVBAMMHUV4YW1wbGUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5MB4X\r\n" \
    "DTIwMDQyMjA4MTYzNVoXDTIwMDYyMTA4MTYzNVowgZMxCzAJBgNVBAYTAkZSMQ8w\r\n" \
    "DQYDVQQIDAZSYWRpdXMxEjAQBgNVBAcMCVNvbWV3aGVyZTEVMBMGA1UECgwMRXhh\r\n" \
    "bXBsZSBJbmMuMSAwHgYJKoZIhvcNAQkBFhFhZG1pbkBleGFtcGxlLm9yZzEmMCQG\r\n" \
    "A1UEAwwdRXhhbXBsZSBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkwggEiMA0GCSqGSIb3\r\n" \
    "DQEBAQUAA4IBDwAwggEKAoIBAQDA6fLwVSnuep+o/nw5vMw7042HN0vVHqxvnuHj\r\n" \
    "NZe1BfWws0kYdDah+CU5fXRQa1cvMZAdIZeRGbNEyh3c3M6bMjd/UizRRCsLWxxx\r\n" \
    "/NNzIWpAtV8V3NAwJ8PUtOrKqqeCJMtT3RewMxSIX1ym+XC0REJZIEcYyLujbp0B\r\n" \
    "jDkdNZNcIWjWzYeRemv2AMfOCBb0dkYCJNjvmNTU8MaiD2UuevJ6Tm2VSCmVNHQ2\r\n" \
    "9ouPad8N3HGSONnkBRWCa/xX557qoTlKBSHzs3V45feSEWqPuVg6smrrSE+CwYKm\r\n" \
    "lVuDlULfpw54upzgXSBrl/SmjUkCYdiP9VolUWVa2VLH+UntAgMBAAGjggE3MIIB\r\n" \
    "MzAdBgNVHQ4EFgQUv6z62GtIVa1nuT3NsSZUJVRNRWgwgcgGA1UdIwSBwDCBvYAU\r\n" \
    "v6z62GtIVa1nuT3NsSZUJVRNRWihgZmkgZYwgZMxCzAJBgNVBAYTAkZSMQ8wDQYD\r\n" \
    "VQQIDAZSYWRpdXMxEjAQBgNVBAcMCVNvbWV3aGVyZTEVMBMGA1UECgwMRXhhbXBs\r\n" \
    "ZSBJbmMuMSAwHgYJKoZIhvcNAQkBFhFhZG1pbkBleGFtcGxlLm9yZzEmMCQGA1UE\r\n" \
    "AwwdRXhhbXBsZSBDZXJ0aWZpY2F0ZSBBdXRob3JpdHmCCQDtRMzoP2cfDjAPBgNV\r\n" \
    "HRMBAf8EBTADAQH/MDYGA1UdHwQvMC0wK6ApoCeGJWh0dHA6Ly93d3cuZXhhbXBs\r\n" \
    "ZS5vcmcvZXhhbXBsZV9jYS5jcmwwDQYJKoZIhvcNAQELBQADggEBAFYppsyiGIPE\r\n" \
    "kvC2bDqQ5VgQWDnl5WDxgagDeXRWYHmeiLHYXwTz/EYAcx53MiDztLW5xV0g1wt7\r\n" \
    "Z17tdGuvQgEiPwQN+vitK7T5Joq0ZXASCjHsuAITIjgXYw4yEl6uq0TPfhedVJKb\r\n" \
    "AtXP0nrlStRFv/YwI/3yo3etMBQy30qi6adTaIYT7ivEEeKBdfOn0zF76Fh3GN11\r\n" \
    "uDyHe0CMoBc3DwxLFOpradQTLhzpoad/4pnnoT62oT8zkomiahEoSTpOKLFIMNbp\r\n" \
    "Guz2ZG1VXMbSjisc4OrIrsZChoDWUW2lRxS7l3I2h2b1fzP9LZ8ugkC8UKhV0mZH\r\n" \
    "QKtMp19BreQ=\r\n" \
    "-----END CERTIFICATE-----\r\n" ;


void wifi_status_notify(int event, void *arg);


static void ssl_log(int level ,char *msg)
{
    app_log("%s", msg);
}

int main(void)
{
  mwifi_eap_attr_t attr;
  /* Initialize network(Wi-Fi, TCP/IP) */
  mxos_network_init();

  /* Register Wi-Fi status notification */
  mxos_system_notify_register(mxos_notify_WIFI_STATUS_CHANGED, wifi_status_notify, NULL);

  app_log("connecting to %s...", SSID);
  
  ssl_set_loggingcb(ssl_log);
  //wifimgr_debug_enable(1);
  cli_init();

#if 1    
AGAIN:
    /* demo default eap mode, EAP-PEAP*/
    app_log("~~~~~~~~~ connect eap PEAP ~~~~~~~~~");
    mwifi_eap_connect("wifitest", "bob", "hello", NULL, NULL);
    sleep(20);

    attr.eap_type = EAP_TYPE_PEAP;
    attr.client_cert = client_cert;
    attr.client_key = client_key;
    attr.rootca = rootca;
    /* demo EAP-PEAP mode, check server certificate*/
    app_log("~~~~~~~~~ connect eap PEAP server check ~~~~~~~~~");
    mwifi_eap_connect("wifitest", "bob", "hello", &attr, NULL);
    sleep(20);

    attr.eap_type = EAP_TYPE_TTLS;
    /* demo EAP-TTLS mode, check server certificate*/
    app_log("~~~~~~~~~ connect eap TTLS server check ~~~~~~~~~");
    mwifi_eap_connect("wifitest", "bob", "hello", &attr, NULL);
    sleep(20);

    attr.eap_type = EAP_TYPE_TLS;
    /* demo EAP-TLS mode, check server certificate. EAP-TLS mode need check client certificate */
    app_log("~~~~~~~~~ connect eap TLS server/client check ~~~~~~~~~");
    mwifi_eap_connect("wifitest", "bob", "hello", &attr, NULL);
    sleep(20);

    /* passphrase mode connect */
    app_log("~~~~~~~~~ connect ssid/key ~~~~~~~~~");
    mwifi_connect(PASSPHRASESSID, PASSPHRASE, strlen(PASSPHRASE), NULL, NULL);
    sleep(20);
goto AGAIN;
#else
  AGAIN:
      /* demo default eap mode, EAP-PEAP*/
      // mwifi_eap_connect("wifitest", "bob", "hello", NULL, NULL);
      // sleep(20);
      attr.eap_type = EAP_TYPE_PEAP;
      //attr.client_cert = client_cert;
      //attr.client_key = client_key;
      attr.rootca = NULL;//rootca;
      /* demo EAP-PEAP mode, check server certificate*/
      // mwifi_eap_connect("wifitest", "bob", "hello", &attr, NULL);
      // sleep(20);
      // attr.eap_type = EAP_TYPE_TTLS;
      // /* demo EAP-TTLS mode, check server certificate*/
      // mwifi_eap_connect("wifitest", "bob", "hello", &attr, NULL);
      // sleep(20);
      //attr.eap_type = EAP_TYPE_TLS;
      app_log("~~~~~~~~~ connect eap ~~~~~~~~~");
      /* demo EAP-TLS mode, check server certificate. EAP-TLS mode need check client certificate */
      mwifi_eap_connect("wifitest", "bob", "hello", &attr, NULL);
  
      sleep(20);
      /* passphrase mode connect */
      // mwifi_connect(PASSPHRASESSID, PASSPHRASE, strlen(PASSPHRASE), NULL, NULL);
      // sleep(20);
  goto AGAIN;
#endif

  return 0;
}

void wifi_status_notify(int event, void *arg)
{
  switch (event)
  {
  case NOTIFY_STATION_UP:
    app_log("----------------> Connected");
    break;
  case NOTIFY_STATION_DOWN:
    app_log("----------------> Disonnected");
    break;
  }
}
