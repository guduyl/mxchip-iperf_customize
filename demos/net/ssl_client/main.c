/**
 ******************************************************************************
 * @file    tcp_client.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   MiCO tcp client Demo
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#include "mxos.h"
#include "SocketUtils.h"
#include "StringUtils.h"


#define tcp_client_log(M, ...) custom_log("TCP", M, ##__VA_ARGS__)

#define TIME_SOURCE_HTTP_HOST   "www.gandi.net"
#define TIME_SOURCE_HTTP_PORT   443
const char tls_root_ca_cert[]=
"-----BEGIN CERTIFICATE-----\n"
"MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB"
"iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl"
"cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV"
"BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw"
"MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV"
"BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU"
"aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy"
"dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK"
"AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B"
"3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY"
"tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/"
"Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2"
"VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT"
"79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6"
"c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT"
"Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l"
"c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee"
"UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE"
"Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd"
"BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G"
"A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF"
"Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO"
"VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3"
"ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs"
"8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR"
"iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze"
"Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ"
"XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/"
"qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB"
"VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB"
"L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG"
"jjxDah2nGN59PRbxYvnKkKj9"
"-----END CERTIFICATE-----\n";



// ARM-MBED echo server
#define MX_TEST_TLS_ECHO_REMOTE_IP      ("52.215.34.155")  
#define MX_TEST_TLS_ECHO_REMOTE_PORT    (2007)

typedef struct {
    char *domain;
    int   port;
    char *ca;
    char *prvkey;
    char *prvcert;
} ssl_test_t;

static char ssl_server_addr[64] = "www.gandi.net";//"aws-file.topband-cloud.com"; /*remote ip address*/
static int ssl_server_port = 443; /*remote port*/

/*
1.fogv3
domain: device.fogcloud.io
prot: 443

2.飞燕国内环境 
domain: a1cchs3abdo.iot-as-mqtt.cn-shanghai.aliyuncs.com  port: 1883

3.飞燕海外环境 
domain: iot-auth.ap-southeast-1.aliyuncs.com    port: 443
domain: iot-auth.ap-northeast-1.aliyuncs.com   port: 443
domain: iot-auth.us-west-1.aliyuncs.com   port: 443
domain: iot-auth.eu-central-1.aliyuncs.com   port: 443
domain: public.itls.eu-central-1.aliyuncs.com  port: 1883
*/

static const char aws_cert[] = "-----BEGIN CERTIFICATE-----\n"
"MIIDWjCCAkKgAwIBAgIVAK/lcACulH0SiL2vRnd12Ott8Fm/MA0GCSqGSIb3DQEB"
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t"
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDAyMTMwNjA0"
"NTJaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh"
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDR2fo0EZYQYeLsHmLg"
"4I0qG+6WB9K9MZGCZrS+sgCL2VWqjh82Hddhswbp2O8kUYJY7PTwn49sFPkpSbHh"
"mcsBL8IILl564N9riwfmf/7X2sjestRwiFvkYvCjh0+jbc7H/sNUD9XhsnpSmqFJ"
"vAfQCc6Er4Z2CslHwh53h3W537uH8g2Nm2sMipUf2uUcGAInDWIXoHoJE1xhILYB"
"muzFI7OU/lF8yMm/48XG+kdB6XpTErRnXhPU3+NUzdSeh/aupSKZVG6/zFq3zdbr"
"VFto+KMoYXbTnanHvptRK+c6X/0iSmhduVgDc4sHU09lnOBuCNGhDVlFGBUt03Cq"
"GJiZAgMBAAGjYDBeMB8GA1UdIwQYMBaAFO1yDQW6GJX5Jd/hbrlo8YqMs3YlMB0G"
"A1UdDgQWBBTplsgx3MNdl4Fg3XsblGlyCABdcTAMBgNVHRMBAf8EAjAAMA4GA1Ud"
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAzGs73eQhImThTJ1aYtQZ424Z"
"vpaksndyi/iT2QXr7/DrF6is25jZJaxDhAiCworSgjM/CgMfvM/lPoiHVNmaw6T0"
"lNkrwieUz7/JgpLiEu1Aq/TRmEpU12G07wnqD3CfpncKo/JKasJZcoyAoBWXpPwz"
"fYrrlmDjOv4USQkwtG3NoZwQRIkI78j8/Y7e3ZGacFVrs8rcNMS+/a6V7ZcMMZoW"
"GMG67rCy/c0jlPq3gQ9epXiEP1ftyUeAKU9nFMBwNn5Z+5n6/uIM//gRXJ7xHG5V"
"7rKA1Fc/k+UnJRJtvf2uNGDicLaq/SGNS767ytW6PGY8YTzqULUSH/Bn/GuXrg=="
"-----END CERTIFICATE-----\n";

static const char aws_key[] = "-----BEGIN RSA PRIVATE KEY-----\n"
"MIIEpAIBAAKCAQEA0dn6NBGWEGHi7B5i4OCNKhvulgfSvTGRgma0vrIAi9lVqo4f"
"Nh3XYbMG6djvJFGCWOz08J+PbBT5KUmx4ZnLAS/CCC5eeuDfa4sH5n/+19rI3rLU"
"cIhb5GLwo4dPo23Ox/7DVA/V4bJ6UpqhSbwH0AnOhK+GdgrJR8Ied4d1ud+7h/IN"
"jZtrDIqVH9rlHBgCJw1iF6B6CRNcYSC2AZrsxSOzlP5RfMjJv+PFxvpHQel6UxK0"
"Z14T1N/jVM3Unof2rqUimVRuv8xat83W61RbaPijKGF2052px76bUSvnOl/9Ikpo"
"XblYA3OLB1NPZZzgbgjRoQ1ZRRgVLdNwqhiYmQIDAQABAoIBAGzrltIvzpPyOrYT"
"j37pDlws40k8BkXs1Hm6lVXlTiNISw6bddfA5WnwYkY+MuHG7MD7PKcoKmkO+cRU"
"W9+c06FqCGv2pVNiJJTNNOyOwteG13u/lkRSgl+oev5l3RAWjp1/viKDlZ/v4028"
"tLpwvh1c+bQk5sWeE5sLRoUD6jbu2KsqM+a3WA9hZ0y1IZ98uEZRD5DiXvzfrgYs"
"YsijNyXNLv3rfW7CqfRq6Hz6zXPmHEeifq5H+MZJ7hPakLPiPeKVgu5k1w+/KFDO"
"BHihJy4nCvfPtCMJ/Ss2tHXEKmkaFmR1YBSb4fapmkX+n+B9Mx5wR/WjejVxAi1a"
"2dfdJ4ECgYEA8qTr3PKZwamDb+EIqBuMFGKtJRL3DCAKuxwUUMMCUqNDqOCsRSCr"
"uj5Ag7GQs1JSWFqhAeFN2Sk+6hGAHS+1OqrQigWRcJyX1kdLk1w8IStTZEeBM+V3"
"jNv7T5NiI/J37JTd5Gucp/bXzw+BVfNgZRSzPfA7yArHKvc+8EJ50aMCgYEA3Wb5"
"6igMBscFSfxhLqNb00DQTKhKLtGu9cGZNl2eJIpcolekr+zZrwBloSa6LTuLc4H3"
"Ty0l2cAULUxj1qG8cc29MbbdM6/sFoB56mcTK+qxlGGEqC2ALleBEhbzLxJoq6gR"
"NR6yEmiEfMD3BIfuyaadS6N8rTeVIdtUJXjJaJMCgYByGb+YHLTU7sOR/r41Xw7W"
"HzjQcESf/QlOx3t+JtWWypNsfIpFz4ExiUiJJRyoVpPGXIfN/IbBEbY2F5ipIOAW"
"AHEkFk44KEr2RnxxhTP5XQhP5ZgKMWOwZ1mcJ/BEeyaNp8tYOIpoKZl+Ewp8wn+X"
"yM4o2Ckkfnz95VpBXOEwBQKBgQCxlt/vip/a79i2N9Jb7nzBPsPWOUcHtvl39wpG"
"KWGoPbOAJHzdAEa7sL3UPZQopBJ/XEhbIgOyXffRPez3RFQ7lKOlNODJjNr6UaN4"
"+aTPHTnDo1Uc8SJYCahmMjJuF34N8oma6jw7t8pNpxp8b+yBozuYaFKhPmxI8/zS"
"8cfcMQKBgQCm5suQZNOzI154tSUsCgy9k8Iw6mQycO6mmKtG2rz9D7KZn371N5KX"
"Bexr3/AWPTApPaFLR3n1Jn5h89x3nkJX63L/9RwHL6UlNFfEQWEKax9MLt77ArO3"
"JTvfAPnZotowWLfNwQzziNvzLKWr87L1m1vikKyVENNnY1GHGtJ4ug=="
"-----END RSA PRIVATE KEY-----\n";

#define AWS_CA "-----BEGIN CERTIFICATE-----\n\
MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\n\
yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\n\
ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\n\
U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\n\
ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\n\
aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL\n\
MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\n\
ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln\n\
biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp\n\
U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y\n\
aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1\n\
nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex\n\
t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz\n\
SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG\n\
BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+\n\
rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/\n\
NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E\n\
BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH\n\
BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\n\
aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv\n\
MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE\n\
p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y\n\
5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK\n\
WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ\n\
4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N\n\
hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq\n\
-----END CERTIFICATE-----\n"

const char azure_certificates[] =
/*DigiCert Global Root CA*/
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\r\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\r\n"
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\r\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\r\n"
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\r\n"
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\r\n"
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\r\n"
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\r\n"
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\r\n"
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\r\n"
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\r\n"
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\r\n"
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\r\n"
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\r\n"
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\r\n"
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\r\n"
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\r\n"
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\r\n"
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\r\n"
"-----END CERTIFICATE-----\r\n"
/* DigiCert Baltimore Root */
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\r\n"
"RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\r\n"
"VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\r\n"
"DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\r\n"
"ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\r\n"
"VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\r\n"
"mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\r\n"
"IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\r\n"
"mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\r\n"
"XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\r\n"
"dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\r\n"
"jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\r\n"
"BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\r\n"
"DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\r\n"
"9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\r\n"
"jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\r\n"
"Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\r\n"
"ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\r\n"
"R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\r\n"
"-----END CERTIFICATE-----\r\n"
/*D-TRUST Root Class 3 CA 2 2009*/
"-----BEGIN CERTIFICATE-----\r\n"
"MIIEMzCCAxugAwIBAgIDCYPzMA0GCSqGSIb3DQEBCwUAME0xCzAJBgNVBAYTAkRF\r\n"
"MRUwEwYDVQQKDAxELVRydXN0IEdtYkgxJzAlBgNVBAMMHkQtVFJVU1QgUm9vdCBD\r\n"
"bGFzcyAzIENBIDIgMjAwOTAeFw0wOTExMDUwODM1NThaFw0yOTExMDUwODM1NTha\r\n"
"ME0xCzAJBgNVBAYTAkRFMRUwEwYDVQQKDAxELVRydXN0IEdtYkgxJzAlBgNVBAMM\r\n"
"HkQtVFJVU1QgUm9vdCBDbGFzcyAzIENBIDIgMjAwOTCCASIwDQYJKoZIhvcNAQEB\r\n"
"BQADggEPADCCAQoCggEBANOySs96R+91myP6Oi/WUEWJNTrGa9v+2wBoqOADER03\r\n"
"UAifTUpolDWzU9GUY6cgVq/eUXjsKj3zSEhQPgrfRlWLJ23DEE0NkVJD2IfgXU42\r\n"
"tSHKXzlABF9bfsyjxiupQB7ZNoTWSPOSHjRGICTBpFGOShrvUD9pXRl/RcPHAY9R\r\n"
"ySPocq60vFYJfxLLHLGvKZAKyVXMD9O0Gu1HNVpK7ZxzBCHQqr0ME7UAyiZsxGsM\r\n"
"lFqVlNpQmvH/pStmMaTJOKDfHR+4CS7zp+hnUquVH+BGPtikw8paxTGA6Eian5Rp\r\n"
"/hnd2HN8gcqW3o7tszIFZYQ05ub9VxC1X3a/L7AQDcUCAwEAAaOCARowggEWMA8G\r\n"
"A1UdEwEB/wQFMAMBAf8wHQYDVR0OBBYEFP3aFMSfMN4hvR5COfyrYyNJ4PGEMA4G\r\n"
"A1UdDwEB/wQEAwIBBjCB0wYDVR0fBIHLMIHIMIGAoH6gfIZ6bGRhcDovL2RpcmVj\r\n"
"dG9yeS5kLXRydXN0Lm5ldC9DTj1ELVRSVVNUJTIwUm9vdCUyMENsYXNzJTIwMyUy\r\n"
"MENBJTIwMiUyMDIwMDksTz1ELVRydXN0JTIwR21iSCxDPURFP2NlcnRpZmljYXRl\r\n"
"cmV2b2NhdGlvbmxpc3QwQ6BBoD+GPWh0dHA6Ly93d3cuZC10cnVzdC5uZXQvY3Js\r\n"
"L2QtdHJ1c3Rfcm9vdF9jbGFzc18zX2NhXzJfMjAwOS5jcmwwDQYJKoZIhvcNAQEL\r\n"
"BQADggEBAH+X2zDI36ScfSF6gHDOFBJpiBSVYEQBrLLpME+bUMJm2H6NMLVwMeni\r\n"
"acfzcNsgFYbQDfC+rAF1hM5+n02/t2A7nPPKHeJeaNijnZflQGDSNiH+0LS4F9p0\r\n"
"o3/U37CYAqxva2ssJSRyoWXuJVrl5jLn8t+rSfrzkGkj2wTZ51xY/GXUl77M/C4K\r\n"
"zCUqNQT4YJEVdT1B/yMfGchs64JTBKbkTCJNjYy6zltz7GRUUG3RnFX7acM2w4y8\r\n"
"PIWmawomDeCTmGCufsYkl4phX5GOZpIJhzbNi5stPvZR1FDUWSi9g/LMKHtThm3Y\r\n"
"Johw1+qRzT65ysCQblrGXnRl11z+o+I=\r\n"
"-----END CERTIFICATE-----\r\n"
/*WoSign*/
"-----BEGIN CERTIFICATE-----\r\n"
"MIIFdjCCA16gAwIBAgIQXmjWEXGUY1BWAGjzPsnFkTANBgkqhkiG9w0BAQUFADBV\r\n"
"MQswCQYDVQQGEwJDTjEaMBgGA1UEChMRV29TaWduIENBIExpbWl0ZWQxKjAoBgNV\r\n"
"BAMTIUNlcnRpZmljYXRpb24gQXV0aG9yaXR5IG9mIFdvU2lnbjAeFw0wOTA4MDgw\r\n"
"MTAwMDFaFw0zOTA4MDgwMTAwMDFaMFUxCzAJBgNVBAYTAkNOMRowGAYDVQQKExFX\r\n"
"b1NpZ24gQ0EgTGltaXRlZDEqMCgGA1UEAxMhQ2VydGlmaWNhdGlvbiBBdXRob3Jp\r\n"
"dHkgb2YgV29TaWduMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAvcqN\r\n"
"rLiRFVaXe2tcesLea9mhsMMQI/qnobLMMfo+2aYpbxY94Gv4uEBf2zmoAHqLoE1U\r\n"
"fcIiePyOCbiohdfMlZdLdNiefvAA5A6JrkkoRBoQmTIPJYhTpA2zDxIIFgsDcScc\r\n"
"f+Hb0v1naMQFXQoOXXDX2JegvFNBmpGN9J42Znp+VsGQX+axaCA2pIwkLCxHC1l2\r\n"
"ZjC1vt7tj/id07sBMOby8w7gLJKA84X5KIq0VC6a7fd2/BVoFutKbOsuEo/Uz/4M\r\n"
"x1wdC34FMr5esAkqQtXJTpCzWQ27en7N1QhatH/YHGkR+ScPewavVIMYe+HdVHpR\r\n"
"aG53/Ma/UkpmRqGyZxq7o093oL5d//xWC0Nyd5DKnvnyOfUNqfTq1+ezEC8wQjch\r\n"
"zDBwyYaYD8xYTYO7feUapTeNtqwylwA6Y3EkHp43xP901DfA4v6IRmAR3Qg/UDar\r\n"
"uHqklWJqbrDKaiFaafPz+x1wOZXzp26mgYmhiMU7ccqjUu6Du/2gd/Tkb+dC221K\r\n"
"mYo0SLwX3OSACCK28jHAPwQ+658geda4BmRkAjHXqc1S+4RFaQkAKtxVi8QGRkvA\r\n"
"Sh0JWzko/amrzgD5LkhLJuYwTKVYyrREgk/nkR4zw7CT/xH8gdLKH3Ep3XZPkiWv\r\n"
"HYG3Dy+MwwbMLyejSuQOmbp8HkUff6oZRZb9/D0CAwEAAaNCMEAwDgYDVR0PAQH/\r\n"
"BAQDAgEGMA8GA1UdEwEB/wQFMAMBAf8wHQYDVR0OBBYEFOFmzw7R8bNLtwYgFP6H\r\n"
"EtX2/vs+MA0GCSqGSIb3DQEBBQUAA4ICAQCoy3JAsnbBfnv8rWTjMnvMPLZdRtP1\r\n"
"LOJwXcgu2AZ9mNELIaCJWSQBnfmvCX0KI4I01fx8cpm5o9dU9OpScA7F9dY74ToJ\r\n"
"MuYhOZO9sxXqT2r09Ys/L3yNWC7F4TmgPsc9SnOeQHrAK2GpZ8nzJLmzbVUsWh2e\r\n"
"JXLOC62qx1ViC777Y7NhRCOjy+EaDveaBk3e1CNOIZZbOVtXHS9dCF4Jef98l7VN\r\n"
"g64N1uajeeAz0JmWAjCnPv/So0M/BVoG6kQC2nz4SNAzqfkHx5Xh9T71XXG68pWp\r\n"
"dIhhWeO/yloTunK0jF02h+mmxTwTv97QRCbut+wucPrXnbes5cVAWubXbHssw1ab\r\n"
"R80LzvobtCHXt2a49CUwi1wNuepnsvRtrtWhnk/Yn+knArAdBtaP4/tIEp9/EaEQ\r\n"
"PkxROpaw0RPxx9gmrjrKkcRpnd8BKWRRb2jaFOwIQZeQjdCygPLPwj2/kWjFgGce\r\n"
"xGATVdVhmVd8upUPYUk6ynW8yQqTP2cOEvIo4jEbwFcW3wh8GcF+Dx+FHgo2fFt+\r\n"
"J7x6v+Db9NpSvd4MVHAxkUOVyLzwPt0JfjBkUO1/AaQzZ01oT74V77D2AhGiGxMl\r\n"
"OtzCWfHjXEa7ZywCRuoeSKbmW9m1vFGikpbbqsY3Iqb+zCB0oy2pLmvLwIIRIbWT\r\n"
"ee5Ehr7XHuQe+w==\r\n"
"-----END CERTIFICATE-----\r\n"
;
static ssl_test_t ssl_test_tbl[] = {
    {"awsiot-device.galanz.com.cn", 443, NULL, NULL, NULL},
#if 1
    {"www.gandi.net", 443, tls_root_ca_cert, NULL, NULL},
    //{"52.215.34.155", 443, NULL, NULL, NULL},
    {"device.fogcloud.io", 443, NULL, NULL, NULL},
    {"a1cchs3abdo.iot-as-mqtt.cn-shanghai.aliyuncs.com", 1883, NULL, NULL, NULL},
    {"iot-auth.ap-southeast-1.aliyuncs.com", 443, NULL, NULL, NULL},
    {"iot-auth.ap-northeast-1.aliyuncs.com", 443, NULL, NULL, NULL},
    {"iot-auth.us-west-1.aliyuncs.com", 443, NULL, NULL, NULL},
    {"iot-auth.eu-central-1.aliyuncs.com", 443, NULL, NULL, NULL},
    {"public.itls.eu-central-1.aliyuncs.com", 1883, NULL, NULL, NULL},
#endif 
    {"global.azure-devices-provisioning.cn", 443, azure_certificates, NULL, NULL},
    {"a24yobszanh1nw.iot.cn-north-1.amazonaws.com.cn", 8883, AWS_CA, aws_key, aws_cert},
 
};

static mos_semphr_id_t wifi_connect_sem = NULL;

static void micoNotify_WifiStatusHandler(WiFiEvent status, void *const inContext)
{
    UNUSED_PARAMETER(inContext);
    switch (status)
    {
        case NOTIFY_STATION_UP:
            mos_semphr_release(wifi_connect_sem);
            break;
        case NOTIFY_STATION_DOWN:
        case NOTIFY_AP_UP:
        case NOTIFY_AP_DOWN:
        default:
            break;
    }
}


void ssl_log(const int logLevel,const char *const logMessage)
{
    tcp_client_log("%s", logMessage);
}

/*when client connected wlan success,create socket*/
void tcp_client_thread( void* arg )
{
    UNUSED_PARAMETER( arg );
    struct hostent *server;
    merr_t err;
    struct sockaddr_in addr;
    struct timeval t;
    fd_set readfds;
    int tcp_fd = -1, len;
    char *buf = NULL;
    int ssl_err=0;
    void *ssl;
    int i, calen;
    
    buf = (char*) malloc( 1024 );
    require_action( buf, exit, err = kNoMemoryErr );

    for(i=0; i<sizeof(ssl_test_tbl)/sizeof(ssl_test_t); i++) {
        tcp_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
        require_action( IsValidSocket( tcp_fd ), exit, err = kNoResourcesErr );
        server = gethostbyname(ssl_test_tbl[i].domain); // Convert URL to IP.

        if (server == NULL) {
            tcp_client_log("no such host");
            continue;
        }
        
        memcpy((char *) &addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(ssl_test_tbl[i].port);

        tcp_client_log( "Connecting to server: %s  port=%d!", ssl_test_tbl[i].domain, ssl_test_tbl[i].port);
        err = connect( tcp_fd, (struct sockaddr *)&addr, sizeof(addr) );

        if (err != kNoErr) {
            tcp_client_log("TCP connect fail");
            close(tcp_fd);
            continue;
        }
        tcp_client_log( "TCP Connect success!" );
        if (ssl_test_tbl[i].ca == NULL)
            calen = 0;
        else
            calen = strlen(ssl_test_tbl[i].ca);
        tcp_client_log("Before SSL connect, free memory %d",mos_mallinfo()->free);
        if (ssl_test_tbl[i].prvcert && ssl_test_tbl[i].prvkey) {
            mtls_set_client_cert(ssl_test_tbl[i].prvcert, ssl_test_tbl[i].prvkey);
        }
        //ssl_set_max_fragment(1);
        ssl = mtls_connect(tcp_fd, calen, ssl_test_tbl[i].ca, &ssl_err);
        if (ssl == NULL) {
            tcp_client_log("ssl connect NULL, ssl_err %d", ssl_err);
            tcp_client_log("Free memory %d",mos_mallinfo()->free);
            close(tcp_fd);
        } else {
            tcp_client_log("SSL connected");
            tcp_client_log("Free memory %d",mos_mallinfo()->free);
            mos_msleep(100);
            mtls_close(ssl);
        }


        tcp_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
        tcp_client_log( "Connecting to server: %s  port=%d!", ssl_test_tbl[i].domain, ssl_test_tbl[i].port);
        err = connect( tcp_fd, (struct sockaddr *)&addr, sizeof(addr) );

        if (err != kNoErr) {
            tcp_client_log("TCP connect fail");
            close(tcp_fd);
            continue;
        }
        tcp_client_log( "TCP Connect success!" );
        if (ssl_test_tbl[i].ca == NULL)
            calen = 0;
        else
            calen = strlen(ssl_test_tbl[i].ca);
        tcp_client_log("Before SSL connect sni, free memory %d",mos_mallinfo()->free);
        if (ssl_test_tbl[i].prvcert && ssl_test_tbl[i].prvkey) {
            mtls_set_client_cert(ssl_test_tbl[i].prvcert, ssl_test_tbl[i].prvkey);
        }

        ssl = mtls_connect_sni(tcp_fd, calen, ssl_test_tbl[i].ca, ssl_test_tbl[i].domain, &ssl_err);
        if (ssl == NULL) {
            tcp_client_log("ssl connect NULL, ssl_err %d", ssl_err);
            tcp_client_log("Free memory %d",mos_mallinfo()->free);
            close(tcp_fd);
        } else {
            tcp_client_log("SSL connected");
            tcp_client_log("Free memory %d",mos_mallinfo()->free);
            mos_msleep(100);
            mtls_close(ssl);
        }

        
        tcp_client_log("close SSL");
        mos_msleep(1000);
    }

    mos_msleep(5000);

    //goto AGAIN;

exit:    
    if ( buf != NULL ) free( buf );



    mos_thread_delete( NULL );
}

int main( void )
{
    merr_t err = kNoErr;

    wifi_connect_sem = mos_semphr_new(1);

    /*Register user function for MiCO nitification: WiFi status changed */
    err = mxos_system_notify_register(mxos_notify_WIFI_STATUS_CHANGED,
                                      (void *)micoNotify_WifiStatusHandler, NULL);
    require_noerr(err, exit);

    /* Start MiCO system functions according to mxos_config.h */
    err = mxos_system_init();
    require_noerr(err, exit);

    /* Wait for wlan connection*/
    mos_semphr_acquire(wifi_connect_sem, MOS_WAIT_FOREVER);
    tcp_client_log("wifi connected successful");

    /* connect to tls server */
    mos_thread_new(MOS_APPLICATION_PRIORITY, "TLS_client", tcp_client_thread, 0x1000, 0 );

    exit:
    if ( wifi_connect_sem != NULL )
        mos_semphr_delete( wifi_connect_sem );

    return err;
}

