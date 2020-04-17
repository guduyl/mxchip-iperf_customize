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
    "MIID1zCCAr+gAwIBAgIBAzANBgkqhkiG9w0BAQsFADCBkzELMAkGA1UEBhMCRlIx\r\n" \
    "DzANBgNVBAgMBlJhZGl1czESMBAGA1UEBwwJU29tZXdoZXJlMRUwEwYDVQQKDAxF\r\n" \
    "eGFtcGxlIEluYy4xIDAeBgkqhkiG9w0BCQEWEWFkbWluQGV4YW1wbGUub3JnMSYw\r\n" \
    "JAYDVQQDDB1FeGFtcGxlIENlcnRpZmljYXRlIEF1dGhvcml0eTAeFw0yMDA0MTYw\r\n" \
    "ODAxMjdaFw0zNjA5MTkwODAxMjdaMHkxCzAJBgNVBAYTAkZSMQ8wDQYDVQQIDAZS\r\n" \
    "YWRpdXMxFTATBgNVBAoMDEV4YW1wbGUgSW5jLjEdMBsGA1UEAwwUeWhiX3Rlc3RA\r\n" \
    "ZXhhbXBsZS5vcmcxIzAhBgkqhkiG9w0BCQEWFHloYl90ZXN0QGV4YW1wbGUub3Jn\r\n" \
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwRikKZuHOw6IPPBaTYFb\r\n" \
    "7MZLT1PQwghZq1nXWb9RK/EV6Kvgo5JoxltKFhqhq6QA0VwvvSpD7rwd5EIc1UAL\r\n" \
    "esgHKqFLNUpOG5WA0QEtZiOJgL+4H9bnZBkJ80hvtwWEHo/0vCqTYyhy7qbd9U+t\r\n" \
    "Bb+oHk+qFnqmRGJgKGTy/CIKVTtE7/fTJZBmIROqhRukAyuDfKdTo6yUlD0XQX3X\r\n" \
    "nvxZO9fVaT5cs5cw8d07PExXRDyWnTzVjUULC9dngIkZzLsY+yk5tCMVy517YR98\r\n" \
    "ksEImejBUi7iWIH4CDVLzz3OuXXjwzzduLMGwcEcBdtBait39DjbTMOkbKt03Oor\r\n" \
    "PQIDAQABo08wTTATBgNVHSUEDDAKBggrBgEFBQcDAjA2BgNVHR8ELzAtMCugKaAn\r\n" \
    "hiVodHRwOi8vd3d3LmV4YW1wbGUuY29tL2V4YW1wbGVfY2EuY3JsMA0GCSqGSIb3\r\n" \
    "DQEBCwUAA4IBAQB8IRqQNuIOWb7qEMvlF3k233jTIxzW3v82AcEALPcSsjXsQ6HJ\r\n" \
    "JpaHJk04qrK5YlH562Ie3tCfgxebUvwU8xNGLCfuNMv/SpVUI2oHl9DbDL5qKVv0\r\n" \
    "bepDy5ugBUN4mlYoFy25L5LcL6Nq3bfs2cSTxrUcMfSytJcvgDlaXlIgBJvng3Uj\r\n" \
    "npDiC5eItas29QPmJIsjL0w+6EvoU2I26mrP3Oa9/sl8eMJMzhm59Mz70k1vL+mP\r\n" \
    "TKyDKWBqJV1esO6Wd7G+623iPr4xDGoTVTo/Lc3M6pGX8GMRNfLYZMpP9jsEMoZE\r\n" \
    "g4NCSioQDUKZ5tfRWMbyT08Xwmkr0r5nHL1j\r\n" \
    "-----END CERTIFICATE-----\r\n";


const char  *client_key = \
    "-----BEGIN RSA PRIVATE KEY-----\r\n" \
    "MIIEpQIBAAKCAQEAwRikKZuHOw6IPPBaTYFb7MZLT1PQwghZq1nXWb9RK/EV6Kvg\r\n" \
    "o5JoxltKFhqhq6QA0VwvvSpD7rwd5EIc1UALesgHKqFLNUpOG5WA0QEtZiOJgL+4\r\n" \
    "H9bnZBkJ80hvtwWEHo/0vCqTYyhy7qbd9U+tBb+oHk+qFnqmRGJgKGTy/CIKVTtE\r\n" \
    "7/fTJZBmIROqhRukAyuDfKdTo6yUlD0XQX3XnvxZO9fVaT5cs5cw8d07PExXRDyW\r\n" \
    "nTzVjUULC9dngIkZzLsY+yk5tCMVy517YR98ksEImejBUi7iWIH4CDVLzz3OuXXj\r\n" \
    "wzzduLMGwcEcBdtBait39DjbTMOkbKt03OorPQIDAQABAoIBAECAfrX3YiD0JcZV\r\n" \
    "c5TYVWCujJGfKejm6S04AZvVMMRA8FSctZluWtmHvSPNGZxAWJWW+ctyxuaEqGeL\r\n" \
    "Ph5uGmeVUe+HmZvfil/kfPdUMVohHNmJMXRRUVI6mvxNgiPdD9k1tUb14e4JoPrf\r\n" \
    "et1rFB0LfJlDe0mvBsNpKT3VGuXl0ztorMl7hDAGQbSKEi4JpzUN5vF0xCIF7aE+\r\n" \
    "M0FN1Wfnm05mJt73vsH0fUo89aMfSQqrPQCLe7+zx1LwVW0ea9NBLAZx/oSdf6r8\r\n" \
    "TAGg+zgB4n8lqKqYzBBew+Hx0/dx0PdXHAuOY/j9StQX4qlV5/tQFj605W60GyGF\r\n" \
    "stpK9/ECgYEA/e7HBpjeF+QnkQc3Gp0OMvLBIuJDXw3kZmQJz2dH9jhk+9ioE3oK\r\n" \
    "KUmmbFejHVh5g3g9xd1Ta769ocaQgeUWcViNhlFM7lrizrIz6ttTB2sZbgyoZHVJ\r\n" \
    "g5IVn7WsDWVzuMR5Ii1EjG1XqXtIMnWnPCswZbxYVuAurVJ5S9joZL8CgYEAwqsS\r\n" \
    "/u9NleM5AhDiJLJbTr8PikmFDberdu7nf80ZyKLrVdVM+iUa4BZotUZtMlRmfozA\r\n" \
    "j8gXe7sraoBzkBXcK+tBdfK2ZS9LLEqewnS21m99AlVmcJlV7T0DASwRJlsgwVPM\r\n" \
    "rbjV8YGZ556oi0AeHUYu7S+/igq135opsIXtQwMCgYEAzKujoU6fXBRaS8vL/1ht\r\n" \
    "dGUzhIFomfnhiSHCwnfVicCErHVBa4lnHUYurI6qe2Gvd2b4Kz4sGeEAiZ85JGlY\r\n" \
    "gH1GfOeUoV2CB+WccQTcVFNfiNMM+wOKAFFDupm8loOaFWfO1twzrxgGmyUx2PH5\r\n" \
    "nEaXWLjJXySMmsip2Zd4IJMCgYEAkoEBWM9sSoOpIb7nEssQIKF15pMSCZLS/j0q\r\n" \
    "CGqpHvBNwFk5B4DpSwBSaSrJf3k5ccAoI41o7vub3xkw3500B/GTYFr7ngcLwhee\r\n" \
    "h6Yi/BsHr7vpzo/3tQWXv0gU2BL5AcY5YfybNphc5Y3oNnRYtZteMa8frZoF6nQx\r\n" \
    "L+J6/1UCgYEA1XpcxlsM005TKE7MZeEhKQd77BiSMRiw+Z1Ry6yhknXlpPog7Pzd\r\n" \
    "ufX51Am7Wrm+1EWPIWJttioA58DnQp02RcXmL4TnZHW5oiEka83zVUxzMOlhP2Qy\r\n" \
    "H+r5XhwqR1cRnOcYyMIdz0J8/iH60oaQy6thxWfF53+i+/TaRbMVO5s=\r\n" \
    "-----END RSA PRIVATE KEY-----\r\n" ;


const char  *rootca =
    "-----BEGIN CERTIFICATE-----\r\n" \
    "MIIE+jCCA+KgAwIBAgIUK0dY9yore83e9r27aSCeCda3PNMwDQYJKoZIhvcNAQEL\r\n" \
    "BQAwgZMxCzAJBgNVBAYTAkZSMQ8wDQYDVQQIDAZSYWRpdXMxEjAQBgNVBAcMCVNv\r\n" \
    "bWV3aGVyZTEVMBMGA1UECgwMRXhhbXBsZSBJbmMuMSAwHgYJKoZIhvcNAQkBFhFh\r\n" \
    "ZG1pbkBleGFtcGxlLm9yZzEmMCQGA1UEAwwdRXhhbXBsZSBDZXJ0aWZpY2F0ZSBB\r\n" \
    "dXRob3JpdHkwHhcNMjAwNDE2MDc1NzE4WhcNMjAwNjE1MDc1NzE4WjCBkzELMAkG\r\n" \
    "A1UEBhMCRlIxDzANBgNVBAgMBlJhZGl1czESMBAGA1UEBwwJU29tZXdoZXJlMRUw\r\n" \
    "EwYDVQQKDAxFeGFtcGxlIEluYy4xIDAeBgkqhkiG9w0BCQEWEWFkbWluQGV4YW1w\r\n" \
    "bGUub3JnMSYwJAYDVQQDDB1FeGFtcGxlIENlcnRpZmljYXRlIEF1dGhvcml0eTCC\r\n" \
    "ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKTmssoPL9iBqNPo7P6/Bt4l\r\n" \
    "o9BbQJ9ZT0LZjwp/XRE4yn9+JGz8ptVnDO7KxMzjwx/5tGIT6/h+z0jDmA22+H8I\r\n" \
    "/U5Kc61w1zVE62/UhJ104UB5VfDxiFe28Ops36E4xPxh7JYvHUYKnWqZoz/gtkmm\r\n" \
    "OuUcZipxzzC+lN+T5JkIMgpnFiB11jlBs25BlHQU5vbxBWt/0ekoMbG4k697WxDH\r\n" \
    "4jVTTNF/dwHGAeGembGypazZAvvtKbPNwR96x8dC8gkN/XFOh2dMPezNmt7utqOO\r\n" \
    "eTfQooAPA2ZqpjAyNFgQeIXSwBo71rrgVK5aaJChyR/2vN5Eo9Xe9Y/j/lww/G8C\r\n" \
    "AwEAAaOCAUIwggE+MB0GA1UdDgQWBBR6DbuOeqh7NlZOTRU1twHS5bmlVTCB0wYD\r\n" \
    "VR0jBIHLMIHIgBR6DbuOeqh7NlZOTRU1twHS5bmlVaGBmaSBljCBkzELMAkGA1UE\r\n" \
    "BhMCRlIxDzANBgNVBAgMBlJhZGl1czESMBAGA1UEBwwJU29tZXdoZXJlMRUwEwYD\r\n" \
    "VQQKDAxFeGFtcGxlIEluYy4xIDAeBgkqhkiG9w0BCQEWEWFkbWluQGV4YW1wbGUu\r\n" \
    "b3JnMSYwJAYDVQQDDB1FeGFtcGxlIENlcnRpZmljYXRlIEF1dGhvcml0eYIUK0dY\r\n" \
    "9yore83e9r27aSCeCda3PNMwDwYDVR0TAQH/BAUwAwEB/zA2BgNVHR8ELzAtMCug\r\n" \
    "KaAnhiVodHRwOi8vd3d3LmV4YW1wbGUub3JnL2V4YW1wbGVfY2EuY3JsMA0GCSqG\r\n" \
    "SIb3DQEBCwUAA4IBAQCflsd2PtPxwmsbQReEQUxtjCDueOnsW2VrrzE2cEd6/MRU\r\n" \
    "fShyawgETdQtf7DF8Ujrjvl64jskNj9sj0bKR4YF0mGV8FRxUQUYZjW0YdEXq4AS\r\n" \
    "CpE/J4Le+XMF3HIvYQ1ayD4wfzVRQ4MLmEguSeXpmbVKxACxo/UadG2QkovQCmks\r\n" \
    "ToyRWEMC8N93wclfzQkCVmJdcCwZmdN3E9rINMOTd4gcfO3G/LDXFFlv1+k1y0AM\r\n" \
    "7rPE8keLKch6AQnSubCOKuhKjjGQVOzJpmlEfGkEzCltI/Bntp4cyvjrWl5nINE2\r\n" \
    "WasP75Zko4lf2LV631DOeev+OwnMXJmZUOmJ9ebi\r\n" \
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
  
  //ssl_set_loggingcb(ssl_log);
  wifimgr_debug_enable(1);
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
      attr.client_cert = client_cert;
      attr.client_key = client_key;
      attr.rootca = rootca;
      /* demo EAP-PEAP mode, check server certificate*/
      // mwifi_eap_connect("wifitest", "bob", "hello", &attr, NULL);
      // sleep(20);
      // attr.eap_type = EAP_TYPE_TTLS;
      // /* demo EAP-TTLS mode, check server certificate*/
      // mwifi_eap_connect("wifitest", "bob", "hello", &attr, NULL);
      // sleep(20);
      attr.eap_type = EAP_TYPE_TLS;
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
