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

# Display
TARGET_SCREEN_DENSITY := 530

# VINTF
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE += \
    $(DEVICE_PATH)/configs/vintf/framework_compatibility_matrix.xml

# Kernel
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    aw882xx_dlkm.ko \
    goodix_fod.ko \
    goodix_core.ko \
    wl2868c.ko \
    wl2866d.ko \
    xiaomi_touch.ko

BOARD_VENDOR_RAMDISK_RECOVERY_KERNEL_MODULES_LOAD += \
    goodix_core.ko

BOOT_KERNEL_MODULES += \
    goodix_core.ko

# OTA assert
TARGET_OTA_ASSERT_DEVICE := vermeer

# Properties
TARGET_ODM_PROP += $(DEVICE_PATH)/properties/odm.prop
TARGET_SYSTEM_PROP += $(DEVICE_PATH)/properties/system.prop
TARGET_VENDOR_PROP += $(DEVICE_PATH)/properties/vendor.prop

# Sepolicy
BOARD_VENDOR_SEPOLICY_DIRS += $(DEVICE_PATH)/sepolicy/vendor
