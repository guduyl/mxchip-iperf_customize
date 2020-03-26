/**
 ******************************************************************************
 * @file    tls_client.c
 * @author  WES
 * @version V1.0.0
 * @date    24-March-2020
 * @brief   MXOS tls client demo to connect AWS IoT
 ******************************************************************************
 *  The MIT License
 *  Copyright (c) 2016 MXCHIP Inc.
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

#define tls_client_log(M, ...) custom_log("TLS_C", M, ##__VA_ARGS__)

// #define USE_AWS_IOT

#ifdef USE_AWS_IOT
/* server hostname */
#define SERVER_HOSTNAME "animag0r6i3wv-ats.iot.ap-southeast-1.amazonaws.com"
/* server port */
#define SERVER_PORT (8883)
/* server cert pem */
#define SERVER_CERT_PEM "-----BEGIN CERTIFICATE-----\n\
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n\
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n\
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n\
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n\
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n\
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n\
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n\
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n\
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n\
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n\
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n\
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n\
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n\
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n\
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n\
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n\
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n\
rqXRfboQnoZsG4q5WTP468SQvvG5\n\
-----END CERTIFICATE-----\n"

/* client cert pem */
#define CLIENT_CERT_PEM "-----BEGIN CERTIFICATE-----\n\
MIIDWjCCAkKgAwIBAgIVANLMBwfF/mfBEGnIWVu8zXREb6KrMA0GCSqGSIb3DQEB\n\
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n\
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDAzMjQwODA0\n\
NDJaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n\
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC4yHLP4Llj2gIpfYtu\n\
TUeeys7qNTCuT3LP3e+0KNN5f/jT6F3v3YkkiyJl1r0RN9pJOvwhlfyGbWa1ucr6\n\
RYi4AHU5B7MvwKOsUmieppCIC7GRfTg2wUzr6/T3wH7kC9frsHZWMp0F6rOpYEtK\n\
hDYjONq93m9QDnY/3SKeEe2FXZWPUUS7dd9LScWf0KX/H6H8IpLugMIuvpxIh6S3\n\
AmuV8maSXjLsgFJOi6nHg0+b/Qewm6LyBKX3pGiUYnlv+sgK4uk/DTnIicFcJbFi\n\
b1spSvLHTzAQo+Kst+xW5m+8Cuuj1XCMfzYlxZf2ReGAv4eSaMtL7RHCyrp/Nmf7\n\
Mq4bAgMBAAGjYDBeMB8GA1UdIwQYMBaAFNeDdiatU904GzM+xZCEhmzsiirBMB0G\n\
A1UdDgQWBBSzdNf4UdlflOVrdGG72yyzn2RGsTAMBgNVHRMBAf8EAjAAMA4GA1Ud\n\
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAUCfm1EoXdW12QE7g6KCsm5y2\n\
+dvPQ2zTaw0epFYvmp1oX6xBITo8tMt08wQjyn0r3uNXU9klj+oyUm3HXK1qr3q/\n\
S+VHLiK1dbww9ytcI/XS5YID1ir6NSfPzEpksvFA0BdyT/oAh7AqYG0lCKPWs11Y\n\
6ycyNjjw0YrQLn5agV7WyEc8OW8/2FE+KXyluy9FJxhgSmfpDJji3ymkIrjCz15W\n\
uo5HRlfrCVDdHIZqSY89ZI9aYezPVfowpeQcZlI/9lPed9habhSewY6Eb7aq9P8G\n\
Tpvz7iqUZxVyKU8dSwyoypYU2QimCh8CPEFoTtLHSYo3bnPLrANjNAU5WT2z1Q==\n\
-----END CERTIFICATE-----\n"

/* client key pem */
#define CLIENT_PRV_KEY_PEM "-----BEGIN RSA PRIVATE KEY-----\n\
MIIEogIBAAKCAQEAuMhyz+C5Y9oCKX2Lbk1HnsrO6jUwrk9yz93vtCjTeX/40+hd\n\
792JJIsiZda9ETfaSTr8IZX8hm1mtbnK+kWIuAB1OQezL8CjrFJonqaQiAuxkX04\n\
NsFM6+v098B+5AvX67B2VjKdBeqzqWBLSoQ2Izjavd5vUA52P90inhHthV2Vj1FE\n\
u3XfS0nFn9Cl/x+h/CKS7oDCLr6cSIektwJrlfJmkl4y7IBSToupx4NPm/0HsJui\n\
8gSl96RolGJ5b/rICuLpPw05yInBXCWxYm9bKUryx08wEKPirLfsVuZvvArro9Vw\n\
jH82JcWX9kXhgL+HkmjLS+0Rwsq6fzZn+zKuGwIDAQABAoIBAD2uC0UAimGeP+KB\n\
ma0z9pg2smLTZnCI7JYeSwkkJ5MC+JgjlFeg50pfdxfvWiUJM8AGBGthmW11zzcR\n\
0YNA+qYQ4RAsVc+2yeuXhnyOtOf0n00lM0Jzu610kcyxYKnWmjh+KsITGv8CnP4I\n\
upkhEMEpFTsTqKxcZ3Rh6FBDQ7mkNXv6R0M/Ee6PL7a20u2ZxBnKp+fmFhgEfkKG\n\
3Fy9VI7nv+Ji3imPhUNwywrkIpaMzSK6C196RSWfHA+LPLYupCkspxLHIQdrS2wn\n\
M61hrp4ROl4yVExZH3nNMntgY3Wl3clP+f2nfDLxT6BS+FmbJ0kDbkcgoGuTrwYq\n\
1z9bIUECgYEA8IwgaR3ZlZFtbfPIc8fNMk/+KEzX7TmsR1ilIc5s2Ilb2P2G8F1R\n\
hHtDA6jSJXt5RfLJnjILmPRCwvInNPsLF/yxlt1UcPubR+owpL1WT7MVbvTCEb6R\n\
rjtLsOIVUN6U98jKIS6QySYVCVqp/Nk/aWhEssQPhRcR+b7MpevUYUkCgYEAxKdC\n\
m4QSd7hxf0AbXmYrhI32/d0Vf+1RW3R7f0MX+Et/BatG8MdJl7nXn0oYCLh2+0aP\n\
VmoO/0Cm27bV+Bge/AdxTqTGFJoLymKGuM16dHU6VMkuZOCusLv7ayX02YRGBsa1\n\
VFs+9n7+RvhyvWvfCWEcMX838YrcQDEPri/reEMCgYAkPskHbcMCYXZesfQLVhja\n\
Mul2AxYqHZ2S5o2DSYpnGlNnn9P8VeNZlgUkAQUhNriwTIQKvTm4I14H59r2hblH\n\
NQFBXTpDB/Z+4I/R0uG1gqOBI4o2iEOFQFQIZBiYJeHGcE/crhqM5JsY3LtuaQHA\n\
DUEEurwn7rD7s788ZQuMWQKBgBrWeNCghaSxF0w2g4Wui+HbPgoHUhfTW3cPvorv\n\
rSXFp+AuL9BMYk7CnhqvEfietB420vkIIGPVfvAdU7Y4FIJwwZXqBt5o55MHPzuZ\n\
T0z1crB/hEVgJHcaMIW++Ft3Iddq5vRMP5uEWU92OLKBNj8hcaePpSLhwI1CJNGw\n\
2IHhAoGAd5YSpxcs01RcL10H/BayymtFZ9LHEhHVz5K8FfB6B1eg+OlHexXxbUKV\n\
UTH5H4mz1aZFmvEpzvSf1ax54klmcew26872wMYQC/z/n+bSYe1Iaq97XEtU3PKP\n\
voQqbRjadRu1NT+ASCeSYVyhePF3a8g8p9V9aFiGfYQeOeNa6nc=\n\
-----END RSA PRIVATE KEY-----\n"

#else
/* server hostname */
#define SERVER_HOSTNAME "192.168.137.1"
/* server port */
#define SERVER_PORT (9000)
/* server cert pem */
#define SERVER_CERT_PEM "-----BEGIN CERTIFICATE-----\n\
MIID8zCCAtugAwIBAgIUD3rsTYxpmxrJ+LFVaIGzgHQXoKUwDQYJKoZIhvcNAQEL\n\
BQAwgYgxCzAJBgNVBAYTAlVTMQswCQYDVQQIDAJXQTEOMAwGA1UEBwwFUGxhY2Ux\n\
FDASBgNVBAoMC1lvdXJDb21wYW55MQswCQYDVQQLDAJJVDEWMBQGA1UEAwwNd3d3\n\
LnlvdXJzLmNvbTEhMB8GCSqGSIb3DQEJARYSeW91ckVtYWlsQHlvdXIuY29tMB4X\n\
DTIwMDMyNDAyNTY0NloXDTIxMDMyNDAyNTY0NlowgYgxCzAJBgNVBAYTAlVTMQsw\n\
CQYDVQQIDAJXQTEOMAwGA1UEBwwFUGxhY2UxFDASBgNVBAoMC1lvdXJDb21wYW55\n\
MQswCQYDVQQLDAJJVDEWMBQGA1UEAwwNd3d3LnlvdXJzLmNvbTEhMB8GCSqGSIb3\n\
DQEJARYSeW91ckVtYWlsQHlvdXIuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A\n\
MIIBCgKCAQEA17gTTYWMAOiPAkedkyaqvs/rDBdQjm7Q617FK7IRv3E6LcI5b1E0\n\
MqPzjnrt+rHId6E6htiFRokQo6fuwhngwyl7xUorFwcHlA3qkD6HzpyfeUFIzjmx\n\
tVH18RXlZLF9gY88Re2XvlpWZnBMcgGJkAdKnwpyK9uFjYnu73EiV8PdicJis5KD\n\
H8INSI0XidomtwMRxZztUbauoTmy8ruRwOyCLPtL0ryfLcmGsqobip0Vhhfsah6e\n\
hJyRfVeAv9YKXaL1qy/AL1A2DOlrsrfluN2/fq2MIeYwuUVpvFaOjzvTPBEZhEQ5\n\
oY/6bBBendaaTXFUGX5SfxaZbbIzj3Z73QIDAQABo1MwUTAdBgNVHQ4EFgQUMJff\n\
5U5e5Jg3qBkjzyWWqzQCCe8wHwYDVR0jBBgwFoAUMJff5U5e5Jg3qBkjzyWWqzQC\n\
Ce8wDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAXkrpcC5otzGI\n\
4XbOrL6m62ggvyne8vNPFnlie8DJcZ9ABaDkDhYVRE+3bs+eCCDDhMKfJ/K1BGl5\n\
cqMZnBQiEt6WWlBtEFBUFcHiX/Hw4WCFRCFbP87lc7eFetPLmAlNMCPpuKeuZWk3\n\
5nsHJYhjhbPu6SLTTmIucEBEcGJlpgjlvi4TwEVFn8piZp47xalK/SaOERAAZxy8\n\
i6iO0T/IQsj8yaYD2ENUgmudVVL8x5HBsnbyCJ5MK5L4vVD19yQPjZkUxi0TBrFD\n\
lC1ZrTwtvEGj8fW8fvJnE/asEaW5bHgG+zKcZxLhCA2lv9WT1Io+MVdbMWDljz8U\n\
naluEY7kwQ==\n\
-----END CERTIFICATE-----\n"

/* client cert pem */
#define CLIENT_CERT_PEM "-----BEGIN CERTIFICATE-----\n\
MIIDmTCCAoECFG1KBtHPnBrEV06VM2vOLvVsjOwyMA0GCSqGSIb3DQEBCwUAMIGI\n\
MQswCQYDVQQGEwJVUzELMAkGA1UECAwCV0ExDjAMBgNVBAcMBVBsYWNlMRQwEgYD\n\
VQQKDAtZb3VyQ29tcGFueTELMAkGA1UECwwCSVQxFjAUBgNVBAMMDXd3dy55b3Vy\n\
cy5jb20xITAfBgkqhkiG9w0BCQEWEnlvdXJFbWFpbEB5b3VyLmNvbTAeFw0yMDAz\n\
MjQwMjU3MzlaFw0yMTAzMjQwMjU3MzlaMIGIMQswCQYDVQQGEwJVUzELMAkGA1UE\n\
CAwCV0ExDjAMBgNVBAcMBVBsYWNlMRQwEgYDVQQKDAtZb3VyQ29tcGFueTELMAkG\n\
A1UECwwCSVQxFjAUBgNVBAMMDXd3dy55b3Vycy5jb20xITAfBgkqhkiG9w0BCQEW\n\
EnlvdXJFbWFpbEB5b3VyLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC\n\
ggEBALV5REMtjqXd7RTucCzLt9FhWJYI1b6vjJf9/OJPSqFwAOOVkfLyj47wxKtn\n\
rMsZ9ZRoOXhTLYE++WKPoYLE5ompEab5luPy/8rzUK5e7rOm322byX+lIyVQzg/r\n\
biGPu2+zLOVtF1Ef6Fze8NWDAYv4ifYmUhlIA2/KIdiSeKZZ5WWURMxDxEEOWTk3\n\
JGxXkEf+TIAlBsZNV1zHW3EHKsUE2hjolbL3D42SzgGYH1uBha5NtCHiZrrKjtJZ\n\
Q7pskdaY3PBDNPYI6EEonGffuCgDXYMIm+hxCn9uMT6DAdM3jUM91hNrwXHVV0Jb\n\
lza3GkPqUpOy6Jx3mv4acqvEoEsCAwEAATANBgkqhkiG9w0BAQsFAAOCAQEAQeYx\n\
L4YbsqLVgX9+Bl/sAFjvjb4vWHhGrvBUPqaRwNxnfxI+PQaagFjJ5qtcPodDzZKc\n\
yQAY2QlOvcqp+dweKiNNdgj3vOJzNGfRoCOr+NfJavbSqAR7yBKoan/arJcyx3WS\n\
BJ/Q4bLda3o7Ojdh4eVR6v+g/phc5ShdFgt0RxECK+MXyXYAXxx+0DDvweGqX10J\n\
hRNIyUewDgn61WShOR/q6NgM2vfPysfC6fcbWbcPdiVaXbmh6T6XWySHZ6P34R0R\n\
wthj7amLKomCkxcDlwC2QFmjN8yWV2qRqBKYKQNvjaepsx6JAcoDvUWGaJnwsFft\n\
J9CEqpASo+TTxF8ZZA==\n\
-----END CERTIFICATE-----\n"

/* client key pem */
#define CLIENT_PRV_KEY_PEM "-----BEGIN RSA PRIVATE KEY-----\n\
MIIEpAIBAAKCAQEAtXlEQy2Opd3tFO5wLMu30WFYlgjVvq+Ml/384k9KoXAA45WR\n\
8vKPjvDEq2esyxn1lGg5eFMtgT75Yo+hgsTmiakRpvmW4/L/yvNQrl7us6bfbZvJ\n\
f6UjJVDOD+tuIY+7b7Ms5W0XUR/oXN7w1YMBi/iJ9iZSGUgDb8oh2JJ4plnlZZRE\n\
zEPEQQ5ZOTckbFeQR/5MgCUGxk1XXMdbcQcqxQTaGOiVsvcPjZLOAZgfW4GFrk20\n\
IeJmusqO0llDumyR1pjc8EM09gjoQSicZ9+4KANdgwib6HEKf24xPoMB0zeNQz3W\n\
E2vBcdVXQluXNrcaQ+pSk7LonHea/hpyq8SgSwIDAQABAoIBAFbMQ7wl5B6tgSIL\n\
0Zd7cNYyy14np6Rm7sYb84KDDl0c5y/zcDVVkxh5THf3RIx4RB/RoftNPWyeJEXi\n\
EQwtBV8+gHTu2B77MSFvM13ThjRjUmZopv3ojGDiv0yZ/LkMuEf62TR68zYYHtmc\n\
wHA/EtRR8hrLxWTWLt6fPYUKua5GbuPsLfI5SnZvswYCpfD2J7Ffoc7WXyk4rAwE\n\
Y0B1N1ErjdTxvv2ZboQPBM4EZgyyO7V95yTBWaVUdgF5Mv20DN7yD1QzlW6Ypmw1\n\
abUx1eoEhh8qmSO6Y5zjBgtE2rwsKOsG/zX4bRU8UK6m2lOzzzY8qSAOKdqbeLNS\n\
tQD8cuECgYEA2JPDUmNAMQjKFpe5hViF/Ko1NDzkdmGMjADc+yRgjAMt2dmT7Szz\n\
xxnFN46E9UXgaKtBAJlhMjbaiS7Ap2yxLQaAusticosq5viExm87x8CjzaVUe6qJ\n\
DH2sWT3an0jzajwPPNanv8ag8bw4rec/zPQOnr4KeA2dXHLZe9Qdo2MCgYEA1oG5\n\
BNkJnOyflkuHSpohgk5szI011S25zzl68+rnyz9TU2pGmGx2mU8WpE73oAF4miXH\n\
Sy/4qt1dSM3kEavOawL3bkpKkqwWZbrQkyk0XrlATENLSwBIVaCdYQp7vpsAQsmj\n\
CluGc7sJfQENW+uZlEgy3W5WBRubSHfrWyJzB/kCgYBM7P8djl9Ii2SsbavZrz24\n\
JwTKzy4e1jtDpzLWiW85jA+HJ38haSwVdhPUzexv6ifxRZLLEa+yVEV/IHoRfcvY\n\
wEyNDJylLsVIUNFoYi4Ln2MPMkA1xLb1FtZWxF+Y4CKFg1uuMlhXB8g21LIVIuRI\n\
eg+PVtUFG2Dfa3PQReFZHQKBgQDTPX6RypaP5eom3QxHqE+VflvgeLNp0xTngIiX\n\
il5oJl2NiNwqoK2Ovnbiqi1rcedtjgScA4rT4RlLaA7QZCvr8EjwYNLdbADGf9mF\n\
Ao/jNHWgZKjSHF4fjQNPHD7ObuAr5VvdCFL4x1lJs+6O9g4RG6STsWOethgOnqG0\n\
5tD3KQKBgQCbNrX46sAVrZX1kPDb+f63FRoT1jBOAXOBe/2HpArbl/sgIYgdVjjR\n\
vUwfAHXq57dcdqCGoJkGcVtq2soIkZDAsxR+pWV7KNRl1Sn0wDaujrIjbQhgxB5h\n\
XnSUUkLqDXeLTMI5A97CVtfTzJKyaROJk/kG5cHm/JiS4stEsYTSiw==\n\
-----END RSA PRIVATE KEY-----\n"
#endif /* USE_AWS_IOT */

/* data echo test */
#define ECHO_LOOP   (10)
#define TXRX_SIZE   (1024)
char tx_buf[TXRX_SIZE] = {"Message from MXCHIP wifi module."};
char rx_buf[TXRX_SIZE];

static mos_semphr_id_t wifi_connect_sem = NULL;

void tls_client_thread( void *arg );

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

int main(void)
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
    tls_client_log("wifi connected successful");

    /* connect to tls server */
    mos_thread_new(MOS_APPLICATION_PRIORITY, "TLS_client", tls_client_thread, 0x1000, 0 );

exit:
    mos_thread_delete(NULL);
    return err;
}

void tls_client_thread( void *arg )
{
    merr_t err;
    int client_fd = -1;
    int ssl_errno = 0;
    mtls_t client_ssl = NULL;
    fd_set readfds;
    char ipstr[16];
    struct sockaddr_in addr;
    struct hostent *hostent_content = NULL;
    char **pptr = NULL;
    struct in_addr in_addr;
    struct timeval timeout_ms;
    int result_txrx;

    /* DNS lookup */
    hostent_content = gethostbyname(SERVER_HOSTNAME);
    require_action_quiet(hostent_content != NULL, exit, err = kNotFoundErr);
    pptr = hostent_content->h_addr_list;
    in_addr.s_addr = *(uint32_t *)(*pptr);
    strcpy(ipstr, inet_ntoa(in_addr));
    tls_client_log("tls server address: host:%s, ip: %s", SERVER_HOSTNAME, ipstr);

    /* tcp connect */
    client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    addr.sin_family = AF_INET;
    addr.sin_addr = in_addr;
    addr.sin_port = htons(SERVER_PORT);
    err = connect(client_fd, (struct sockaddr *)&addr, sizeof(addr));
    require_noerr_string(err, exit, "connect tls server failed");

    /* tls connect */
    tls_client_log("tls version %s", "TLS_V1_2_MODE");
    mtls_set_ver(TLS_V1_2_MODE);
    tls_client_log("client cert len %d, cert:\r\n%s", strlen(CLIENT_CERT_PEM), CLIENT_CERT_PEM);
    tls_client_log("client private key len %d, cert:\r\n%s", strlen(CLIENT_PRV_KEY_PEM), CLIENT_PRV_KEY_PEM);
    mtls_set_client_cert(CLIENT_CERT_PEM, CLIENT_PRV_KEY_PEM);
    tls_client_log("server CA len %d, CA:\r\n%s", strlen(SERVER_CERT_PEM), SERVER_CERT_PEM);
    client_ssl = mtls_connect(client_fd, strlen(SERVER_CERT_PEM), SERVER_CERT_PEM, &ssl_errno);
    if(client_ssl == NULL)
    {
        tls_client_log("*** tls connect error %d", ssl_errno);
        err = kConnectionErr;
        goto exit;
    }
    else
    {
        tls_client_log("============= tls connect success! ============");
    }
    
    #if 1
    for(int i = 0; i < ECHO_LOOP; i++)
    {
        /* tls Send */
        result_txrx = mtls_send(client_ssl, tx_buf, sizeof(tx_buf));
        if(result_txrx == sizeof(tx_buf))
        {
            tls_client_log("tls send len %d.", result_txrx);
        }
        else
        {
            tls_client_log("*** tls send error %d", result_txrx);
            err = kNotWritableErr;
            goto exit;
        }

        /* tls Recv */
        //tls_client_log("tls recv ...");
        FD_ZERO(&readfds);
        FD_SET(client_fd, &readfds);

        timeout_ms.tv_sec = 10;
        timeout_ms.tv_usec = 1;
        select(client_fd + 1, &readfds, NULL, NULL, &timeout_ms);
        if (FD_ISSET(client_fd, &readfds))
        {
            memset(rx_buf, 0, sizeof(rx_buf));
            result_txrx = mtls_recv(client_ssl, rx_buf, sizeof(rx_buf));
            if(result_txrx > 0)
            {
                tls_client_log("tls recv len %d : %s", result_txrx, rx_buf);
            }
            else
            {
                tls_client_log("*** tls recv error %d", result_txrx);
                err = result_txrx;
            }
        }
        else
        {
            tls_client_log("*** socket recv timeout！");
        }
    }
    #endif // 0

exit:
    tls_client_log("Exit: Client exit with err = %d, fd: %d", err, client_fd);
    if (client_ssl)
        mtls_close(client_ssl);
    SocketClose(&client_fd);

    mos_thread_delete(NULL);
}