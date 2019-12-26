/**
 ******************************************************************************
 * @file    uart.c
 * @author  Snow Yang
 * @version V1.0.0
 * @date    17-May-2019
 * @brief   UART demo
 ******************************************************************************
 */

#include "mxos.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

int main(void)
{
	merr_t err = kNoErr;
	uint8_t data;
	uint32_t n;

	mhal_uart_config_t config = {
		.baudrate = MXOS_APP_UART_BAUDRATE,
		.data_width = DATA_WIDTH_8BIT,
		.parity = NO_PARITY,
		.stop_bits = STOP_BITS_1,
		.flow_control = FLOW_CONTROL_DISABLED,
		.buffersize = 1024,
	};

	mhal_uart_pinmux_t pinmux = {
		.tx = MXOS_APP_UART_TXD,
		.rx = MXOS_APP_UART_RXD,
		.rts = MXOS_APP_UART_RTS,
		.cts = MXOS_APP_UART_CTS,
	};

	/* Initialize uart driver */
	err = mhal_uart_open(MXOS_APP_UART, &config, &pinmux);
	require_noerr(err, exit);

	while (1)
	{
		n = 1;
		mhal_uart_read(MXOS_APP_UART, &data, &n, 0xFFFFFFFF);
		mhal_uart_write(MXOS_APP_UART, &data, 1);
	}

exit:
	return err;
}
