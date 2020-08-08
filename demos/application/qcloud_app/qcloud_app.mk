#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2020 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := App_qcloud_light

# IO define for dc motor & rgb led on MiCOKit-EXT board
GLOBAL_DEFINES +=   DC_MOTOR_PIN=MXKIT_D9		\
                    P9813_PIN_CIN=MXKIT_SCL 	\
                    P9813_PIN_DIN=MXKIT_SDA 	\
                    EXT_KEY1=MODULE_PIN_14

GLOBAL_INCLUDES += .

$(NAME)_SOURCES :=  mxos_main.c

$(NAME)_COMPONENTS :=   lib_qcloud							\
                        libraries/drivers/motor/dc_motor	\
                        libraries/drivers/rgb_led/P9813
