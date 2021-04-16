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
"JAYDVQQDDB1FeGFtcGxlIENlcnRpZmljYXRlIEF1dGhvcml0eTAeFw0yMTA0MTYw\r\n" \
"NjMzMzBaFw0yNDA0MTUwNjMzMzBaMHExCzAJBgNVBAYTAkZSMQ8wDQYDVQQIDAZS\r\n" \
"YWRpdXMxFTATBgNVBAoMDEV4YW1wbGUgSW5jLjEZMBcGA1UEAwwQdXNlckBleGFt\r\n" \
"cGxlLm9yZzEfMB0GCSqGSIb3DQEJARYQdXNlckBleGFtcGxlLm9yZzCCASIwDQYJ\r\n" \
"KoZIhvcNAQEBBQADggEPADCCAQoCggEBAMCDW9IKjRB8N2Gb/TMOgh9K0iUngkd+\r\n" \
"FRDUYahLpVZ2qKMQnDFfXSiewl76/0WbmdwgoTqkwlIgGsn9b36zKecu4pyd+/YS\r\n" \
"Z90hRu0l1qvFJ4Ede7k1rGBwBszS+TytDrsQ2muXLPISxQMtfsFrl7zrrKzQP5Nf\r\n" \
"i/mKhu1VrNqpstdQoFSxHakX2C4jhYbvQ+zvVTNq2tbQkP1vbweevMgB+vrr2TRX\r\n" \
"NYg5J2bwI7k7TH9UoeeQlwSUqrT8fEmWwwmiWZpfh5Q4tCZVeMD7bjQlL5q9rFgC\r\n" \
"Jyt/MNralthHZGvenSV71X8WrtcjP6UGCiz9pGol2tn9zpUIVVa/zYcCAwEAAaNP\r\n" \
"ME0wEwYDVR0lBAwwCgYIKwYBBQUHAwIwNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDov\r\n" \
"L3d3dy5leGFtcGxlLmNvbS9leGFtcGxlX2NhLmNybDANBgkqhkiG9w0BAQsFAAOC\r\n" \
"AQEAAGOQOyvpG0fpPnuPgr3czLSCPAaj3VFA9gjAfF9A3ue9LrVGKOtGAHQnvO5f\r\n" \
"khTU2tzWgQYip0+/udgI85WEQqINyP41b/xX6BHyBFz5Pp9X2RBOwaqyDlCbr7Wh\r\n" \
"xZib49Qe8RMOfGUSCugV+N2yNXNSx77kU17rPBVc8dXGVwkBpTPiA6UsHB3JYEF4\r\n" \
"vkFOQrp92vHTXKz1GJbY0ebUvjljKMxoPG+AgXolA41dAcm+NpA1uvX8C7WRUmNo\r\n" \
"4Oj4OP92prIqdR4lP+BnuGcrUmnq4DoczWevlj0wp4Pj9Fb+n6nevCQXx2EBoNJf\r\n" \
"dUMLS2AryMrMZAp7cjdRALkkEA==\r\n" \
"-----END CERTIFICATE-----\r\n";

const char  *client_key = \
"-----BEGIN RSA PRIVATE KEY-----\r\n" \
"MIIEpAIBAAKCAQEAwINb0gqNEHw3YZv9Mw6CH0rSJSeCR34VENRhqEulVnaooxCc\r\n" \
"MV9dKJ7CXvr/RZuZ3CChOqTCUiAayf1vfrMp5y7inJ379hJn3SFG7SXWq8UngR17\r\n" \
"uTWsYHAGzNL5PK0OuxDaa5cs8hLFAy1+wWuXvOusrNA/k1+L+YqG7VWs2qmy11Cg\r\n" \
"VLEdqRfYLiOFhu9D7O9VM2ra1tCQ/W9vB568yAH6+uvZNFc1iDknZvAjuTtMf1Sh\r\n" \
"55CXBJSqtPx8SZbDCaJZml+HlDi0JlV4wPtuNCUvmr2sWAInK38w2tqW2Edka96d\r\n" \
"JXvVfxau1yM/pQYKLP2kaiXa2f3OlQhVVr/NhwIDAQABAoIBAQCk6CEgdG9P/sfF\r\n" \
"8kRblwL54TLQJt7fdO6Aox0gd+Qig7SDMnhJhsIbSJuBlwpSSy7qNH1H6diXVu77\r\n" \
"tf/rDXhIk44E0QQDTtLvxlCmVjJko5bqcCIKjiKh3XTFwbdENY4XyiD/5tjWaCOh\r\n" \
"TDh3YT3y1ALvadf72u3hjbt01F4kU5pdrEVpVHEBoMCI99hDQBPJBhZlOS+WnG/q\r\n" \
"HpZPwPBAcqj8G8loM16E56BlTimDvNqVyHV+b3jxVdWfWtGRlak5RyA5BekkBKd6\r\n" \
"/3uNnHOwqcrttk/4Q+VtNFOnWOMwStEAD//iiKHmMCGapRe/ygZ/qE0LpEMJnLCu\r\n" \
"1qh7HliBAoGBAOu1WQ7WY/rNVjuHbsGXGY8xQe4xAKSBpB23JZafLVxEkd5Bdv7I\r\n" \
"9PLrIQs3L+8Bvfyzzup9p94SCqtEPwH9MO7RhSBOtp9AfALXOiPH+Cy3KO31J7ri\r\n" \
"tB3OZ1/GLRXhOjmhO/+n7j74bu8Kccn4HXvZlJEmzGaB3MfIeA2xJIxBAoGBANEW\r\n" \
"DZthqhxXekzYkuDOdTIXDtdEz98inZa6FL7Wf/8nQEwnuuYQ6F/TD/ph+4B5bLrL\r\n" \
"ilWnkQQhfa+KtmfE1Ee/9nlqIT0sPi8L55OkUVnTM3/c1udF2lpBRd6y4REp/OLf\r\n" \
"5iVm7r05Ufqjw1aLSaqvxGuPjVFsaw3VOetHQQfHAoGAEtMO48J3haY0htiQHeHl\r\n" \
"Zqw2acF8HadE5iKkW90mkEHhrHBqAnsmJOhmYTDtIRdwhugNVGjoqTGjrH3s0knY\r\n" \
"SJpvGV9v8lQCE3AA8q6bwWZ3XFnB7IkCfYbodpurKh4sukGIOyi8PEc3BNw+rjSk\r\n" \
"limQVe9VT7J7YXE/NUi6dIECgYAgH6hFiDLPme0TY27llTLSWFKSZmMdoD7gCoTn\r\n" \
"cSNHO2uYh74iXNSm7FGXIXi3m3BGHsGJOW6BWPfrgX9W4BnRUD/5FYI32NVQ+x5A\r\n" \
"Q82v5FAURATMkaHjngWzGn6T2YSeAtJkSrVBY04iGQkJAet2ivy9ndeTe/nSrUDx\r\n" \
"0ge+vwKBgQDrOeuVRIg71ytx5hyi2NkAOIbOJX4DVTLWR84h/NF90R95J9Pa8Wtp\r\n" \
"4wvqtW5A1Q9vpjqHn5MCR2pQ2OPh082/xFuguGvhD5bkkBJvaetNpTQiB4xSyZpO\r\n" \
"j8xQR7UK6AdfJLvkINlLlWmmORWaxcZcIzDREtI8rUgSgjCwb0dQzA==\r\n" \
"-----END RSA PRIVATE KEY-----\r\n";

const char  *rootca =
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIE+jCCA+KgAwIBAgIUGXMJoWwxSXkDHojoYgm9Nyqy66IwDQYJKoZIhvcNAQEL\r\n" \
"BQAwgZMxCzAJBgNVBAYTAkZSMQ8wDQYDVQQIDAZSYWRpdXMxEjAQBgNVBAcMCVNv\r\n" \
"bWV3aGVyZTEVMBMGA1UECgwMRXhhbXBsZSBJbmMuMSAwHgYJKoZIhvcNAQkBFhFh\r\n" \
"ZG1pbkBleGFtcGxlLm9yZzEmMCQGA1UEAwwdRXhhbXBsZSBDZXJ0aWZpY2F0ZSBB\r\n" \
"dXRob3JpdHkwHhcNMjEwNDE2MDYzMzI5WhcNMjQwNDE1MDYzMzI5WjCBkzELMAkG\r\n" \
"A1UEBhMCRlIxDzANBgNVBAgMBlJhZGl1czESMBAGA1UEBwwJU29tZXdoZXJlMRUw\r\n" \
"EwYDVQQKDAxFeGFtcGxlIEluYy4xIDAeBgkqhkiG9w0BCQEWEWFkbWluQGV4YW1w\r\n" \
"bGUub3JnMSYwJAYDVQQDDB1FeGFtcGxlIENlcnRpZmljYXRlIEF1dGhvcml0eTCC\r\n" \
"ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKkLlsyzI9HLeET4qio1Zsoy\r\n" \
"cDoWjnWmlMCbCxjApTVdZoIygTWgwzrl2IVRZLSYTZP9Fu7Nfxky1BS0Kwx5pZpV\r\n" \
"Iub8hgWmtjci5CY7NRr2dlznOuz9wCoQm4EAcqcXjGP+lnz707kU0aoC5v8WI06Y\r\n" \
"PN3TOjJx/X/gWhXcDXebm5a3TsgPiM2j5Xtt9caPtf3WxIGN8LCB3tbPG+QMYFcE\r\n" \
"h9uoVM/bpVpYLU5WlphyTMB7HYG1adR55yOdMgCpZgDTzF2HPrJvM/ZC5OC2gxrF\r\n" \
"GcYworCsD2DowpL804KTxt2KPu/olAeJl281Ki7YyU7cbrGMuhymY05w5/uvOEsC\r\n" \
"AwEAAaOCAUIwggE+MB0GA1UdDgQWBBS/soQcH/JvTZ4we/RuvqwI1ghRbTCB0wYD\r\n" \
"VR0jBIHLMIHIgBS/soQcH/JvTZ4we/RuvqwI1ghRbaGBmaSBljCBkzELMAkGA1UE\r\n" \
"BhMCRlIxDzANBgNVBAgMBlJhZGl1czESMBAGA1UEBwwJU29tZXdoZXJlMRUwEwYD\r\n" \
"VQQKDAxFeGFtcGxlIEluYy4xIDAeBgkqhkiG9w0BCQEWEWFkbWluQGV4YW1wbGUu\r\n" \
"b3JnMSYwJAYDVQQDDB1FeGFtcGxlIENlcnRpZmljYXRlIEF1dGhvcml0eYIUGXMJ\r\n" \
"oWwxSXkDHojoYgm9Nyqy66IwDwYDVR0TAQH/BAUwAwEB/zA2BgNVHR8ELzAtMCug\r\n" \
"KaAnhiVodHRwOi8vd3d3LmV4YW1wbGUub3JnL2V4YW1wbGVfY2EuY3JsMA0GCSqG\r\n" \
"SIb3DQEBCwUAA4IBAQBjALoStT7KV5yiORzHfr85xZURErN+S8PiSUnhD7NjVI/8\r\n" \
"RQkNwj7pl7smOgzXcphbfIhAcK1FF4p9kQIxVyaHLnbJsmPOh/ad/gK+GVLauDH/\r\n" \
"9y0YVuIxgB28ozj4VweBzk4yiY0NwjNGrXQCHvhGZ+YFU77eJQHrhkW7boxLBJpw\r\n" \
"LDkKIsfghe7JbelkxM//MlHqCpASn+BFmTo0snbrFn3qm+l6rt59ksFhfM+CTron\r\n" \
"9hqept8PVnUYXaSdvXAsA4mTQa28YWJv3maOSJIzIrLI3EzIsr6VJF83xdk7aQw1\r\n" \
"peMSB1SewD1kChqEVq+zW/WqXgW/3F0oDfLI0KoW\r\n" \
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
