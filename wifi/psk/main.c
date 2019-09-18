/**
 ******************************************************************************
 * @file    main.c
 * @author  Snow Yang
 * @version V1.0.0
 * @date    21-May-2019
 * @brief   Wi-Fi PSK demo.
 ******************************************************************************
 */

#include "mxos.h"

#define SSID "snowyang"
#define PASSPHRASE "mxchip123"

#define WC_SHA 1

int wc_PBKDF2(uint8_t *output, const uint8_t *passwd, int pLen,
			  const uint8_t *salt, int sLen, int iterations, int kLen,
			  int typeH);

int pbkdf2_sha1(const char *passphrase, const uint8_t *ssid, size_t ssid_len,
				int iterations, uint8_t *buf, size_t buflen)
{
	if (wc_PBKDF2(buf, (const uint8_t *)passphrase, strlen(passphrase), ssid,
				  ssid_len, iterations, buflen, WC_SHA) != 0)
		return -1;
	return 0;
}

int main(void)
{
	uint8_t psk[32];

	printf("SSID=\"%s\"\r\n", SSID);
	printf("Passphrase=\"%s\"\r\n", PASSPHRASE);
	uint32_t t = mos_time();
	pbkdf2_sha1(PASSPHRASE, (const uint8_t *)SSID, strlen(SSID), 4096, psk, 32);
	printf("PSK=");
	for (int i = 0; i < 32; i++)
		printf("%02x", psk[i]);
	printf("\r\n");
	printf("Cost %ld ms\r\n", mos_time() - t);

	return 0;
}
