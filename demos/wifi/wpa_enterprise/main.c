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
    "JAYDVQQDDB1FeGFtcGxlIENlcnRpZmljYXRlIEF1dGhvcml0eTAeFw0xOTEyMTgw\r\n" \
    "MzIxNThaFw0yMDAyMTYwMzIxNThaMHExCzAJBgNVBAYTAkZSMQ8wDQYDVQQIDAZS\r\n" \
    "YWRpdXMxFTATBgNVBAoMDEV4YW1wbGUgSW5jLjEZMBcGA1UEAwwQdXNlckBleGFt\r\n" \
    "cGxlLm9yZzEfMB0GCSqGSIb3DQEJARYQdXNlckBleGFtcGxlLm9yZzCCASIwDQYJ\r\n" \
    "KoZIhvcNAQEBBQADggEPADCCAQoCggEBAOQTQ8TU3AdxOT9hPQRKiwCblLHBInm0\r\n" \
    "SZAqcs9TwSrh3KGg46EoynioBCFlrdazqG6RHKYhhxiSLgdxLH7H/mKrl9BBjszX\r\n" \
    "fub5WAhbb4c8eg5AjReOAqix1QoqbVdR3Q91Ok3/Mp6uUo8IX9xp5DVSjZQcPdRv\r\n" \
    "zb8sKW8iA8hulMcqa/ObPVzj3nQzbtqKiSV/g9w8KYbG6nwpKBlSp+3Gb6SP7qkH\r\n" \
    "9+PgYzArA7SQnNi0Z/Hck105M3HWHaBQKfPCMpGHL+CsjSd0EYrQ7O12ZyZkESkC\r\n" \
    "N/vJOSEsDFy0mfZ/ZmlZ7IPQ8PeZB+DoGx+kCzTyC+z/w3jqnJ3Z6eUCAwEAAaNP\r\n" \
    "ME0wEwYDVR0lBAwwCgYIKwYBBQUHAwIwNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDov\r\n" \
    "L3d3dy5leGFtcGxlLmNvbS9leGFtcGxlX2NhLmNybDANBgkqhkiG9w0BAQsFAAOC\r\n" \
    "AQEAnvpOXyzIPVqT8DXz4gJ6PNloOJOyQYEowl+IwkEXQ2tJA/Te2QN/PRjOMdC4\r\n" \
    "VIwK140cJQ+2w6vKUWLw/SddeN+/bYOwqm19eUXfBdqIFm+a6nCBJ3h4OsN00m3B\r\n" \
    "y0wJbHEQ9hNkttc3fOFdR8cyiWV03HNTEj0ZMgRQFyA4w/DEW0IZ9g8wG7ltsaqa\r\n" \
    "tyK13EaxtTE/sONZ0zSlgo7VRqqx/cfvbcHF1JPD62nfmo3FfkbQWgcTVysVbCdY\r\n" \
    "BP1NJCP70zxCLsPvFAvBU6l8QGbyQ3PAXtNGp+mxOVYBzyJZvDFVDZgReq+0lrBd\r\n" \
    "jKdCO8dPuDxSr3/QKfJEr/FcHw==\r\n" \
    "-----END CERTIFICATE-----";


const char  *client_key = \
    "-----BEGIN RSA PRIVATE KEY-----\r\n" \
    "MIIEpQIBAAKCAQEA5BNDxNTcB3E5P2E9BEqLAJuUscEiebRJkCpyz1PBKuHcoaDj\r\n" \
    "oSjKeKgEIWWt1rOobpEcpiGHGJIuB3Esfsf+YquX0EGOzNd+5vlYCFtvhzx6DkCN\r\n" \
    "F44CqLHVCiptV1HdD3U6Tf8ynq5Sjwhf3GnkNVKNlBw91G/NvywpbyIDyG6Uxypr\r\n" \
    "85s9XOPedDNu2oqJJX+D3DwphsbqfCkoGVKn7cZvpI/uqQf34+BjMCsDtJCc2LRn\r\n" \
    "8dyTXTkzcdYdoFAp88IykYcv4KyNJ3QRitDs7XZnJmQRKQI3+8k5ISwMXLSZ9n9m\r\n" \
    "aVnsg9Dw95kH4OgbH6QLNPIL7P/DeOqcndnp5QIDAQABAoIBAQCS61oHbHtEjAot\r\n" \
    "qszse7xpKs+xZdF4hsuwg7ermK9hmVdQAt0/jpBHAyyN+vrtyJOijfXfis+E432l\r\n" \
    "K6FXFivnkdPcwq5jhBnqXgr4bzY77phNoAWDe+Z4lv2+a7A9xK1bbM+fONp535+t\r\n" \
    "Jd7ilI/+TWyIUVz/APqPwLn8LfQz4thD/JW2tdic+bSVA3Zm2UTb/P0Obkyn3d5p\r\n" \
    "HkYTeMPni8yKg7O2MpbenFNNHPMDcuvHSqG81MJaqBO8nz5c163+f8v/ev9fjM0y\r\n" \
    "AVhppYDh1sCR4w/ODhMSiCRPAn5mvBAKRq860o1Ci/ouHxMmCs1LOz27jo3hzp3r\r\n" \
    "opsyofhBAoGBAP+ero9MlpTPx34Y98FAQLSoD7YWKPXn68NS0u8nrIJ//lzHseyB\r\n" \
    "/3nQfbjuTTWW2MkqgsTIM99r3EdfVzOiU85v4iW0wTZHHgW3YJLzvRI3vRm/y46a\r\n" \
    "rAEGvLtkdyPIFepGJBgEE4ucemf4q2ghukZpG1CMzY8pv8ZHMz06W6FRAoGBAORq\r\n" \
    "GKJKpK7XL4RK9RLTW5rpYA45doQLjfOhZeV+dcECNaj8KupjfQ7+782VPNHa6AV8\r\n" \
    "89V1xSjh/QThq1abu2VU3noHfM8Lyyz9XjSS+pbBcQe8FkJVGzlltXaCDskHVYVB\r\n" \
    "9wMfCboRF7iEn6u8A5NYCXdTC8SpBs7bvTBckTpVAoGBAL/MOsI+/0WNWexdo5iX\r\n" \
    "M3XaHF/N5NIOQLKANNFJfYE1tI+9Lt189Upbi9n0xG4PT34xrYklhvZ/IftTqESP\r\n" \
    "6j4daRxNsiOd3QMBAy+APz+qHvNxG7VWjPUALyCUU2i/uuHb2z1B4cCBmr7ksQgj\r\n" \
    "NneaaepYm+xtsP1/cwzf+tzhAoGARwFoWQMlQKjqP5DhsJF9t6F219UrL487+JGi\r\n" \
    "0H3Ic1CRh8+kM6rr1H6R43HSL1aCrDoUvrXWrYfbFOpmTg0pay2/32V2ehKrfXfi\r\n" \
    "EivitHy7+3CP5tz4Epa4m3eAyyN5hWxlMlYyuuHXYufvjv5qiiJjBvINBXyZ/jI3\r\n" \
    "FNKh3uUCgYEAvQf/XwtU55CijUCtvnex6M4qt+/b4SqkNyRJ8mnjiVZCco2gOd3a\r\n" \
    "+RpNAWabzdVV6+KrqVrMeI9AG9/AgzUgSj6ln5gwt1nGUTDSPIWaY4upz4pxB+Wi\r\n" \
    "N3xaPZ41YAt25b1yujSfTgvf0OcHY1q23Wux4B477LG6vk1XVksrUmg=\r\n" \
    "-----END RSA PRIVATE KEY-----\r\n";


const char  *rootca =
    "-----BEGIN CERTIFICATE-----\r\n" \
    "MIIE+jCCA+KgAwIBAgIUJSLofdNf8zajZlyPe4MXVnwNI50wDQYJKoZIhvcNAQEL\r\n" \
    "BQAwgZMxCzAJBgNVBAYTAkZSMQ8wDQYDVQQIDAZSYWRpdXMxEjAQBgNVBAcMCVNv\r\n" \
    "bWV3aGVyZTEVMBMGA1UECgwMRXhhbXBsZSBJbmMuMSAwHgYJKoZIhvcNAQkBFhFh\r\n" \
    "ZG1pbkBleGFtcGxlLm9yZzEmMCQGA1UEAwwdRXhhbXBsZSBDZXJ0aWZpY2F0ZSBB\r\n" \
    "dXRob3JpdHkwHhcNMTkxMjE4MDMyMTU4WhcNMjAwMjE2MDMyMTU4WjCBkzELMAkG\r\n" \
    "A1UEBhMCRlIxDzANBgNVBAgMBlJhZGl1czESMBAGA1UEBwwJU29tZXdoZXJlMRUw\r\n" \
    "EwYDVQQKDAxFeGFtcGxlIEluYy4xIDAeBgkqhkiG9w0BCQEWEWFkbWluQGV4YW1w\r\n" \
    "bGUub3JnMSYwJAYDVQQDDB1FeGFtcGxlIENlcnRpZmljYXRlIEF1dGhvcml0eTCC\r\n" \
    "ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL5JbCrruxdF96maHUjKt/fe\r\n" \
    "kjLfHgBnc1tWSNlMXMUnCyxJ4QKjbu/ivWBw0REgJzCfG8kCNHkaLQf4Bmn54c25\r\n" \
    "gHZZg6n9H6HAAn2Icd3nmnK2tvuaBUos2ZquMblBrPfijDX1dBAhABLmtYatyLX7\r\n" \
    "Y6uHGqY2THfayekrqrOWfv2HYIEJvIu8nxp8CzQtpk3VQEVKTXSzb13F+W6ECR0o\r\n" \
    "fiSPd+Psvm5jwFk13DriChjpeFJ8pip85QaDTdn3B2jL3Mi+ub1H7ce4wxZFNfmR\r\n" \
    "PxJofuuWtbi+BgQbLtkYtWZzTVh0BcNOYrdsbQCB1AVWkKKd8+jiet5H6wmdtH0C\r\n" \
    "AwEAAaOCAUIwggE+MB0GA1UdDgQWBBQ6LyUcCx2gNRhnPhv7D+yTo21jszCB0wYD\r\n" \
    "VR0jBIHLMIHIgBQ6LyUcCx2gNRhnPhv7D+yTo21js6GBmaSBljCBkzELMAkGA1UE\r\n" \
    "BhMCRlIxDzANBgNVBAgMBlJhZGl1czESMBAGA1UEBwwJU29tZXdoZXJlMRUwEwYD\r\n" \
    "VQQKDAxFeGFtcGxlIEluYy4xIDAeBgkqhkiG9w0BCQEWEWFkbWluQGV4YW1wbGUu\r\n" \
    "b3JnMSYwJAYDVQQDDB1FeGFtcGxlIENlcnRpZmljYXRlIEF1dGhvcml0eYIUJSLo\r\n" \
    "fdNf8zajZlyPe4MXVnwNI50wDwYDVR0TAQH/BAUwAwEB/zA2BgNVHR8ELzAtMCug\r\n" \
    "KaAnhiVodHRwOi8vd3d3LmV4YW1wbGUub3JnL2V4YW1wbGVfY2EuY3JsMA0GCSqG\r\n" \
    "SIb3DQEBCwUAA4IBAQA/nL14h5pFukIfQPmoqFd3IErd4EwwqNDr1YrT7bP+epS9\r\n" \
    "X7kmNhBWy2t4+5zBlyhk24ZhlGbYY7QSYFfpakFzHo3IZBCR+1RAZB9GElUq9jDN\r\n" \
    "qYoLvLMURRVsRFfp1I7rvXMh0ftJ2arFXGacu0NaGOLwoPSAuOyb2bdAPyiwVbEH\r\n" \
    "6JJNntWyhDQVhJ01SPNO5yKpDBd7YGqdmAPimW99GSgnj9JDg94OXPdtLalmbOfS\r\n" \
    "KTmEzB27sZmMWSLg+pB9Kajdfs6DwWTlg+FqKt3UbUTTj+m9SMwKm/nV9IzZ2+QC\r\n" \
    "8a8FUEOaE0I/ql27+DR8Du25sYZ8yhBSk2qQPlmp\r\n" \
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
    
AGAIN:
    /* demo default eap mode, EAP-PEAP*/
    mwifi_eap_connect("wifitest", "bob", "hello", NULL, NULL);
    sleep(20);
    attr.eap_type = EAP_TYPE_PEAP;
    attr.client_cert = client_cert;
    attr.client_key = client_key;
    attr.rootca = rootca;
    /* demo EAP-PEAP mode, check server certificate*/
    mwifi_eap_connect("wifitest", "bob", "hello", &attr, NULL);
    sleep(20);
    attr.eap_type = EAP_TYPE_TTLS;
    /* demo EAP-TTLS mode, check server certificate*/
    mwifi_eap_connect("wifitest", "bob", "hello", &attr, NULL);
    sleep(20);
    attr.eap_type = EAP_TYPE_TLS;
    /* demo EAP-TLS mode, check server certificate. EAP-TLS mode need check client certificate */
    mwifi_eap_connect("wifitest", "bob", "hello", &attr, NULL);

    sleep(20);
    /* passphrase mode connect */
    mwifi_connect(PASSPHRASESSID, PASSPHRASE, strlen(PASSPHRASE), NULL, NULL);
    sleep(20);
goto AGAIN;

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
