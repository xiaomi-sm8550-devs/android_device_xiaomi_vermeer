#
# Copyright (C) 2025 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from xiaomi sm8550-common
$(call inherit-product, device/xiaomi/sm8550-common/common.mk)

# Inherit from the proprietary version
$(call inherit-product, vendor/xiaomi/vermeer/vermeer-vendor.mk)

# Init
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/init/init.vermeer.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.vermeer.rc \

# Overlay
PRODUCT_PACKAGES += \
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
