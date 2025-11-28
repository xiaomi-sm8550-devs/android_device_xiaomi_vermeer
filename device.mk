#
# Copyright (C) 2025 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from xiaomi sm8550-common
$(call inherit-product, device/xiaomi/sm8550-common/common.mk)

# Inherit from the proprietary version
$(call inherit-product, vendor/xiaomi/vermeer/vermeer-vendor.mk)

# Audio
PRODUCT_PACKAGES += \
    firmware_aw_cali.bin_symlink

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/audio/mixer_paths_kalama_mtp.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio/sku_kalama/mixer_paths_kalama_mtp.xml \
    $(LOCAL_PATH)/configs/audio/resourcemanager_kalama_mtp.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio/sku_kalama/resourcemanager_kalama_mtp.xml

# IFAAService
PRODUCT_PACKAGES += \
    IFAAService

# Overlay
PRODUCT_PACKAGES += \
    ApertureResVermeer \
    FrameworkResOverlayVermeer \
    SettingsOverlayVermeer \
    SettingsOverlayVermeerPOCO \
    SettingsOverlayVermeerRedmi \
    SettingsProviderResVermeerPOCO \
    SettingsProviderResVermeerRedmi \
    SystemUIOverlayVermeer \
    WifiResVermeerPOCO \
    WifiResVermeerRedmi

# Soong namespaces
PRODUCT_SOONG_NAMESPACES += \
    $(LOCAL_PATH)

# System properties
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/properties/build_CN.prop:$(TARGET_COPY_OUT_ODM)/etc/build_CN.prop \
    $(LOCAL_PATH)/properties/build_GL.prop:$(TARGET_COPY_OUT_ODM)/etc/build_GL.prop

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/properties/build_CN.prop:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/odm/etc/build_CN.prop \
    $(LOCAL_PATH)/properties/build_GL.prop:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/odm/etc/build_GL.prop
