#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME := App_Homekit_Example

HOMEKIT_APP_TARGET_DIR := Lightbulb

GLOBAL_INCLUDES += $(HOMEKIT_APP_TARGET_DIR)

# IO defines for MiCOKit-EXT board
GLOBAL_DEFINES +=   DC_MOTOR=MXKIT_D9       \
                    P9813_PIN_CIN=MXKIT_SCL \
                    P9813_PIN_DIN=MXKIT_SDA \
                    EXT_KEY1=MODULE_PIN_14

# for homekit adk
GLOBAL_DEFINES += HAP_LOG_LEVEL=3

$(NAME)_SOURCES :=  mxos_main.c                                         \
                    $(HOMEKIT_APP_TARGET_DIR)/Main.c                    \
                    $(HOMEKIT_APP_TARGET_DIR)/App.c                     \
                    $(HOMEKIT_APP_TARGET_DIR)/DB.c                      \
                    $(HOMEKIT_APP_TARGET_DIR)/ExampleAccessorySetup.c

$(NAME)_COMPONENTS :=   lib_homekit                             \
                        libraries/drivers/motor/dc_motor        \
                        libraries/drivers/rgb_led/P9813
