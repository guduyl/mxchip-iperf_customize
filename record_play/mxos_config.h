/**
******************************************************************************
* @file    mxos_config.h
* @author  William Xu
* @version V1.0.0
* @date    08-Aug-2018
* @brief   This file provide application options diff to default.
******************************************************************************
*/

#ifndef __MXOS_CONFIG_H__
#define __MXOS_CONFIG_H__

#define FIRMWARE_REVISION   "MXPLAYER"
#define MANUFACTURER        "MXCHIP Inc."
#define SERIAL_NUMBER       "0000.0000.AD01"
#define PROTOCOL            "com.mxchip.spp"

#define MXOS_QUALITY_CONTROL_ENABLE 1

#define SPI_CS_PIN                                  MODULE_PIN_26
#define SPI_USE_SPI                                 SPI_1

/* enable buffer for mxplayer */
#define MXPLAYER_ENABLE_BUFFER 1
#define MXPLAYER_BUFFER_SIZE   2000
#endif
