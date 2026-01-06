#  SPDX-License-Identifier: BSD-3-Clause
#  Copyright (C) 2017 Intel Corporation
#  All rights reserved.
#

SPDK_ROOT_DIR := $(abspath $(CURDIR)/spdk)
include $(SPDK_ROOT_DIR)/mk/spdk.common.mk
include $(SPDK_ROOT_DIR)/mk/spdk.modules.mk

APP = q_fs

C_SRCS := q_fs.c

SPDK_LIB_LIST = $(ALL_MODULES_LIST) event event_bdev json

include $(SPDK_ROOT_DIR)/mk/spdk.app.mk
