/**
 ******************************************************************************
 * @file    main.c
 * @author  Eshen Wang
 * @version V1.0.0
 * @date    19-Oct-2020
 * @brief   Main entrance of MXOS applicatio.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2019 MXCHIP Inc.
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
 *
 ******************************************************************************
 */

#include "mxos.h"
#include "mhal/flash.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

#define TEST_PARTITION	(MODULE_PARTITION_USER)
#define WRITE_BUF_SIZE	(4*1024)
#define READ_BUF_SIZE	(WRITE_BUF_SIZE)

uint8_t write_buf[WRITE_BUF_SIZE] = {0};
uint8_t read_buf[READ_BUF_SIZE] = {0};

int main(void)
{
	merr_t err = kGeneralErr;
	uint32_t write_offset = 0;
	uint32_t read_offset = 0;

	/* Start mxos system functions according to mxos_config.h*/
	err = mxos_system_init();
	require_noerr(err, exit);

	/* Output on debug serial port 115200,8/N/1 */
	app_log("Flash RW Demo!");

	for(int i = 0; i < sizeof(write_buf); i++){
		write_buf[i] = i % 256;
	}
	memset(read_buf, 0, sizeof(read_buf));

	/* erase whole user partition */
	app_log("user partition erase...");
	write_offset = 0; // offset from start address of user partition
	err = mhal_flash_erase(TEST_PARTITION, write_offset, mhal_flash_get_info(MODULE_PARTITION_USER)->partition_length);
	require_noerr_string(err, exit, "erase error!!");
	app_log("user partition erase success.");

	/* write data to user partition */
	app_log("user partition write & read test...");
	write_offset = 0; // offset from the start address of user partition, and return the next offset to write
	read_offset = 0; // offset from the start address of user partition, and return the next offset to read
	while (write_offset < mhal_flash_get_info(TEST_PARTITION)->partition_length) {
		/* write data to user partition */
		app_log("offset: 0x%lx write", write_offset);
		err = mhal_flash_write(TEST_PARTITION, &write_offset, write_buf, sizeof(write_buf));
		require_noerr_action(err, exit, app_log("write offset: 0x%lx error!!", write_offset));

		mos_msleep(1); // NOTE: release for WDG

		/* read data from user partition */
		memset(read_buf, 0, sizeof(read_buf));
		app_log("offset: 0x%lx read", read_offset);
		err = mhal_flash_read(TEST_PARTITION, &read_offset, read_buf, sizeof(read_buf));
		require_noerr_action(err, exit, app_log("write offset: 0x%lx error!!", read_offset));

		/* data check */
		if (0 == memcmp(write_buf, read_buf, sizeof(write_buf))) {
			app_log("offet: 0x%lx RW success.", read_offset);
		} else {
			app_log("offet: 0x%lx RW failed!!", read_offset);
		}
	}
	app_log("user partition write & read test finish.");

exit:
	return 0;
}
