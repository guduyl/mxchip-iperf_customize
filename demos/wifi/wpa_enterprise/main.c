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
    "JAYDVQQDDB1FeGFtcGxlIENlcnRpZmljYXRlIEF1dGhvcml0eTAeFw0yMDA2MDkw\r\n" \
    "ODE3NDZaFw0yMDA4MDgwODE3NDZaMHExCzAJBgNVBAYTAkZSMQ8wDQYDVQQIDAZS\r\n" \
    "YWRpdXMxFTATBgNVBAoMDEV4YW1wbGUgSW5jLjEZMBcGA1UEAwwQdXNlckBleGFt\r\n" \
    "cGxlLm9yZzEfMB0GCSqGSIb3DQEJARYQdXNlckBleGFtcGxlLm9yZzCCASIwDQYJ\r\n" \
    "KoZIhvcNAQEBBQADggEPADCCAQoCggEBALzlOdgYtjgw58H8kzmGoSb4/fmRmqDL\r\n" \
    "HdnS4jnEQ+UP1u9Ff+DBLtuD7OIfGPCHXvU7jsn9sHUO6bC/7P5TzvGaEHrEVE8/\r\n" \
    "SOQHv6oLVzZkqEaNZHYcityfXvkN3pALRRyD7nhaXxqSOJtM0HN7j6sVIclbTpyy\r\n" \
    "9ObLHX6gXpBEcJpCO7BvixML0t5rTn8x3x/9msJ++4qmTSahPkf1yf6h/Yxra58F\r\n" \
    "57DtOYBE9fo+Pg8qwgK2bnXxjT8fvXeCdjGamJS0IhBdE3omhFVkS9NX9gWnDLtt\r\n" \
    "TP+xuSEqIdu9xlcKJbzVWFA9kCOmnS+q4k9f7bufd04rSDVRKzDirfMCAwEAAaNP\r\n" \
    "ME0wEwYDVR0lBAwwCgYIKwYBBQUHAwIwNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDov\r\n" \
    "L3d3dy5leGFtcGxlLmNvbS9leGFtcGxlX2NhLmNybDANBgkqhkiG9w0BAQsFAAOC\r\n" \
    "AQEAhNfXATF5FXGYObEeWYJFhQYZWhtrHF/JbdapxFpVpp+HojiEdWVrHfLHesIP\r\n" \
    "g+KJHsbiLo0+/GZSMIkGsmk7jbp/oH5EtzXmo4XZ7CpTGlYSthPskksEf/EQ/Cyj\r\n" \
    "Zfskfl6F4SNaw/f3+1E6BD5plZQ7jY5MTBbwD9fDd6MLLqzFuNpa3DvnIa0kKRHw\r\n" \
    "JPeWjTIATY04jaU+EFz9CBOF6qN7DwrgkCsi++vwskbkD0kccXbRgzuNqmcjGz6l\r\n" \
    "zcs/+GnX02+fgyfTvxXbDjRd7bnukCmMYWdQFlzQZwHtTrqnoJUXjE+0ItAdSNt1\r\n" \
    "ZcmzDGgGMurAEiGX8ZqikRnfUQ==\r\n" \
    "-----END CERTIFICATE-----\r\n";

const char  *client_key = \
    "-----BEGIN RSA PRIVATE KEY-----\r\n" \
    "MIIEpQIBAAKCAQEAvOU52Bi2ODDnwfyTOYahJvj9+ZGaoMsd2dLiOcRD5Q/W70V/\r\n" \
    "4MEu24Ps4h8Y8Ide9TuOyf2wdQ7psL/s/lPO8ZoQesRUTz9I5Ae/qgtXNmSoRo1k\r\n" \
    "dhyK3J9e+Q3ekAtFHIPueFpfGpI4m0zQc3uPqxUhyVtOnLL05ssdfqBekERwmkI7\r\n" \
    "sG+LEwvS3mtOfzHfH/2awn77iqZNJqE+R/XJ/qH9jGtrnwXnsO05gET1+j4+DyrC\r\n" \
    "ArZudfGNPx+9d4J2MZqYlLQiEF0TeiaEVWRL01f2BacMu21M/7G5ISoh273GVwol\r\n" \
    "vNVYUD2QI6adL6riT1/tu593TitINVErMOKt8wIDAQABAoIBAQCgn4sAZoMMZQmt\r\n" \
    "usi2MfT23uOYRGfoMs+/WcNku6/AKCfsbny+8YaWwye7YBYk8fedMHfK6x4RbAzr\r\n" \
    "b0hnO3zNlOROgIPwny+sN6qTPfF7Hzy4QaMbjEnIMPzhaxnGAs9R4B332WDlh1D7\r\n" \
    "PeZDTN5t7Bo+zpWaSdJWLoF2G4hNw27AZZrdOm0JzXTVZx29rmMiF1w+Jxx57sBT\r\n" \
    "Oz/BlcCRmr4gTq8J57HLrQtOhdEQuZVQ0x46T71AzjUO5pHFVqCus1/aN08ixRua\r\n" \
    "+LN2pdpiXKtsViHHTR8DROmxM/L+7pPbNrTsJ+GyC0PilfhZH2/P7OjhsGNH5iJj\r\n" \
    "4tmI0nsBAoGBANxTbw7UGMjbn38giXAH82AksEM8qVxw+6Dm6O0Enre72gBcXwrh\r\n" \
    "ztZGY9S8oa/MCNAafV+GT8luU3tPjnCcyHY1qPnKN5Tpkqe6/1goDKKW20J1toAi\r\n" \
    "/Bwq2VpFotTcTlT4zeIAj0yZW48LcEkuWRdZM6ZhByfgwemCm9L6r/U7AoGBANt6\r\n" \
    "/WLL7JPkZMNNJvHGM2TQQam+afgHwTCvwQzZZspYHiHQOB0BnM/D6vgm24DgeG8C\r\n" \
    "rVDIxtI6y1GsaKbWRypefQ5f88x2TE124gQyOV/YfrIyqUNT8gHKK3B4KuRk7nAS\r\n" \
    "2Xrrshm1Dy3weh5H6sdKH/a7IZXNW/IXvyHEqL6pAoGBAI5M9PXG1xsjfihFg37w\r\n" \
    "I6h9Cbrjex9RebO6b8ZlrX1Nvsds4ZXNsiGC5QeJrYQ1a5u9ACyqOu4Ga7TA9WaW\r\n" \
    "XIK8tZ5flPaAxU+/3xu5AjAg7wxFqtGnxt0Kb8d2Im0IvS70Y0UMx4DZq+D/qk4Z\r\n" \
    "uUDzRAC5uj1vVM7UpH/nma3xAoGAQgjhD8aXKXAAORqPaxLdB6n4959Its1ufL2E\r\n" \
    "cd8/JAYbGsVQ/Lktev1RRtQ6Gq5uzceaacHewAXcgnhDDF9QDhlbuuV3rr6w+mWY\r\n" \
    "XvbVeFbk16ySbKVnHM68/xvIm+saKjIhqPAdJ8roBERkGZQ9uTQ+JbfrxOyCi3ZF\r\n" \
    "lV1b//ECgYEA2Rd+54s0LgYGl2akQCQMKKoJ8f5P7EmP8VN/z/pyXu/FjRx0LiDn\r\n" \
    "J1m5y6zLmE49d78AzXvPZ43nI/b7H8v1iRL9+kDUkrDIH49+Y5Xctr7Av78wHsQO\r\n" \
    "3g3FrcsVLIikXWhFKF8vrwaZZdP+Biweq6CiW0m24hIVyRRe/zMkFl4=\r\n" \
    "-----END RSA PRIVATE KEY-----\r\n";

const char  *rootca =
    "-----BEGIN CERTIFICATE-----\r\n" \
    "MIIE5DCCA8ygAwIBAgIJALOCw1kFgrHSMA0GCSqGSIb3DQEBCwUAMIGTMQswCQYD\r\n" \
    "VQQGEwJGUjEPMA0GA1UECAwGUmFkaXVzMRIwEAYDVQQHDAlTb21ld2hlcmUxFTAT\r\n" \
    "BgNVBAoMDEV4YW1wbGUgSW5jLjEgMB4GCSqGSIb3DQEJARYRYWRtaW5AZXhhbXBs\r\n" \
    "ZS5vcmcxJjAkBgNVBAMMHUV4YW1wbGUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5MB4X\r\n" \
    "DTIwMDYwOTA4MTc0NVoXDTIwMDgwODA4MTc0NVowgZMxCzAJBgNVBAYTAkZSMQ8w\r\n" \
    "DQYDVQQIDAZSYWRpdXMxEjAQBgNVBAcMCVNvbWV3aGVyZTEVMBMGA1UECgwMRXhh\r\n" \
    "bXBsZSBJbmMuMSAwHgYJKoZIhvcNAQkBFhFhZG1pbkBleGFtcGxlLm9yZzEmMCQG\r\n" \
    "A1UEAwwdRXhhbXBsZSBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkwggEiMA0GCSqGSIb3\r\n" \
    "DQEBAQUAA4IBDwAwggEKAoIBAQC5O7AiHeRog/2pXtmqnqcePFPRZKBVE031pmrN\r\n" \
    "/Ih3fGg2om6YSI4GgH0ENepiKJINGEqGLEAg21fWoH87K0QGom4FKXMDGMc54PoS\r\n" \
    "gyJjKdx//CEvcQTJ7VxrodG4zmw94wkyJ9yPRZ6dFvCVy6A4aRwaJU5s/tGoly9c\r\n" \
    "p9aHc45X1N9EjvhNywjeBF+pPhJDOvrdxJko90dftY4Abys1ZJZUUE61YTuAp8ag\r\n" \
    "WEX08c4rxX/s3D9oTnlGRnaBdhdA7VpPhm6kT40tdZJg+7eHkTbDZtmcibefJPFO\r\n" \
    "Xsq0xJzqx99GNSt6y1+wSKsSbGeu50HAoJy4PeorKLtOoBt7AgMBAAGjggE3MIIB\r\n" \
    "MzAdBgNVHQ4EFgQU3tTkRrIhGrtB8iqq08YX35u5YfUwgcgGA1UdIwSBwDCBvYAU\r\n" \
    "3tTkRrIhGrtB8iqq08YX35u5YfWhgZmkgZYwgZMxCzAJBgNVBAYTAkZSMQ8wDQYD\r\n" \
    "VQQIDAZSYWRpdXMxEjAQBgNVBAcMCVNvbWV3aGVyZTEVMBMGA1UECgwMRXhhbXBs\r\n" \
    "ZSBJbmMuMSAwHgYJKoZIhvcNAQkBFhFhZG1pbkBleGFtcGxlLm9yZzEmMCQGA1UE\r\n" \
    "AwwdRXhhbXBsZSBDZXJ0aWZpY2F0ZSBBdXRob3JpdHmCCQCzgsNZBYKx0jAPBgNV\r\n" \
    "HRMBAf8EBTADAQH/MDYGA1UdHwQvMC0wK6ApoCeGJWh0dHA6Ly93d3cuZXhhbXBs\r\n" \
    "ZS5vcmcvZXhhbXBsZV9jYS5jcmwwDQYJKoZIhvcNAQELBQADggEBAGPa/fRg9gbe\r\n" \
    "Im1bosewzXLYtX12Ph6jnkV5FtB8exGiejfjAQnAdsxGyLviKP/EFDUnwK56d+BB\r\n" \
    "vbEQFNXIYLNeXMtcNOLR9DwZQBr8/0ISNkJ4C3MzBtQpSlUjTQr9uXcDOL4RsHvP\r\n" \
    "UdN0ZBylfeNd8x44RSrCU1C5R92Frj2hgjRDvLAwtMjeG20RB5YqKgZdn+twZBlT\r\n" \
    "vtvoLMPzhp43dmwKKEjJyZnktqXd99LnGnqot+oZUAfeC44IHanFIO4tcRYPFAdf\r\n" \
    "ECmleX6jSMMbmI6+dqm3ffya+XNi5IZsw83CjWZE3Gq2AvdjUgRusdNC0jetaKvF\r\n" \
    "l3s95kYdkXA=\r\n" \
    "-----END CERTIFICATE-----\r\n";


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
