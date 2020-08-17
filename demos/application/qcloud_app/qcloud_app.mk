#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2020 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := App_qcloud_light

# IO define for rgb led on MiCOKit-EXT board
GLOBAL_DEFINES +=   P9813_PIN_CIN=MXKIT_SCL		\
                    P9813_PIN_DIN=MXKIT_SDA

GLOBAL_INCLUDES += .

$(NAME)_SOURCES :=  mxos_main.c

$(NAME)_COMPONENTS :=   libraries/protocols/SNTP		\
						lib_qcloud						\
                        libraries/drivers/rgb_led/P9813

# add qcloud sample code
# QCLOUD_SAMPLE_NAME := data_template
QCLOUD_SAMPLE_NAME := scenarized

QCLOUD_SAMPLE_DIR := samples/$(QCLOUD_SAMPLE_NAME)
include demos/application/qcloud_app/$(QCLOUD_SAMPLE_DIR)/$(QCLOUD_SAMPLE_NAME).mk
$(NAME)_INCLUDES += $(QCLOUD_SAMPLE_INCLUDES)
$(NAME)_SOURCES += $(QCLOUD_SAMPLE_SOURCES)
