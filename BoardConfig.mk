#
# Copyright (C) 2025 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from xiaomi sm8550-common
include device/xiaomi/sm8550-common/BoardConfigCommon.mk

# Inherit from the proprietary version
include vendor/xiaomi/vermeer/BoardConfigVendor.mk

DEVICE_PATH := device/xiaomi/vermeer

# Kernel
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
	aw882xx_dlkm.ko \
	goodix_fod.ko \
	goodix_core.ko \
        wl2868c.ko \
        wl2866d.ko \
        xiaomi_touch.ko
