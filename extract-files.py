#!/usr/bin/env -S PYTHONPATH=../../../tools/extract-utils python3
#
# SPDX-FileCopyrightText: 2024 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

import extract_utils.tools


from extract_utils.fixups_blob import (
    blob_fixup,
    blob_fixups_user_type,
)
from extract_utils.fixups_lib import (
    lib_fixup_vendorcompat,
    lib_fixups_user_type,
    libs_proto_3_9_1,
)
from extract_utils.main import (
    ExtractUtils,
    ExtractUtilsModule,
)

namespace_imports = [
    'device/xiaomi/sm8550-common',
    'hardware/qcom-caf/sm8550',
    'hardware/xiaomi',
    'vendor/qcom/opensource/commonsys-intf/display',
    'vendor/xiaomi/sm8550-common',
]

def lib_fixup_odm_suffix(lib: str, partition: str, *args, **kwargs):
    return f'{lib}-{partition}' if partition == 'odm' else None

lib_fixups: lib_fixups_user_type = {
    libs_proto_3_9_1: lib_fixup_vendorcompat,
    (
        'sqlite3',
    ): lib_fixup_odm_suffix,
}

sensor_simbol = b'_ZN13DisplayConfig10ClientImpl4InitENSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEEPNS_14ConfigCallbackE\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'

blob_fixups: blob_fixups_user_type = {
    (
        'odm/etc/camera/enhance_motiontuning.xml',
        'odm/etc/camera/night_motiontuning.xml',
        'odm/etc/camera/motiontuning.xml'
    ): blob_fixup()
        .regex_replace('xml=version', 'xml version'),
    (
        'odm/lib64/libxmi_high_dynamic_range_cdsp.so',
        'odm/lib64/libailab_rawhdr.so'
    ): blob_fixup()
        .strip_debug_sections(),
    (
        'odm/lib64/libcamxcommonutils.so',
        'odm/lib64/hw/com.qti.chi.override.so',
        'odm/lib64/libchifeature2.so',
        'odm/lib64/libmialgoengine.so'
    ): blob_fixup()
        .add_needed('libprocessgroup_shim.so'),
    (
        'odm/lib64/libalLDC.so',
        'odm/lib64/libalAILDC.so'
    ): blob_fixup()
        .clear_symbol_version('AHardwareBuffer_allocate')
        .clear_symbol_version('AHardwareBuffer_describe')
        .clear_symbol_version('AHardwareBuffer_lock')
        .clear_symbol_version('AHardwareBuffer_release')
        .clear_symbol_version('AHardwareBuffer_unlock'),
    'odm/lib64/libTrueSight.so': blob_fixup()
        .clear_symbol_version('AHardwareBuffer_allocate')
        .clear_symbol_version('AHardwareBuffer_describe')
        .clear_symbol_version('AHardwareBuffer_isSupported')
        .clear_symbol_version('AHardwareBuffer_lock')
        .clear_symbol_version('AHardwareBuffer_lockPlanes')
        .clear_symbol_version('AHardwareBuffer_release')
        .clear_symbol_version('AHardwareBuffer_unlock'),
    'odm/lib64/libMiVideoFilter.so': blob_fixup()
        .clear_symbol_version('AHardwareBuffer_allocate')
        .clear_symbol_version('AHardwareBuffer_describe')
        .clear_symbol_version('AHardwareBuffer_lockPlanes')
        .clear_symbol_version('AHardwareBuffer_release')
        .clear_symbol_version('AHardwareBuffer_unlock'),
    'odm/lib64/libmorpho_ubwc.so': blob_fixup()
        .clear_symbol_version('AHardwareBuffer_describe'),
    'odm/lib64/libwrapper_dlengine.so' : blob_fixup()
        .add_needed('liblog.so'),
    'odm/lib64/hw/camera.xiaomi.so': blob_fixup()
        .add_needed('libprocessgroup_shim.so')
        .replace_needed('libui.so', 'libui-v34.so'),
    'odm/lib64/hw/vendor.xiaomi.sensor.citsensorservice@2.0-impl.so' : blob_fixup()
        .binary_regex_replace(b'_ZN13DisplayConfig10ClientImpl13ClientImplGetENSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEEPNS_14ConfigCallbackE', sensor_simbol)
}

module = ExtractUtilsModule(
    'vermeer',
    'xiaomi',
    blob_fixups=blob_fixups,
    lib_fixups=lib_fixups,
    namespace_imports=namespace_imports,
    add_firmware_proprietary_file=True,
)

if __name__ == '__main__':
    utils = ExtractUtils.device_with_common(
        module, 'sm8550-common', module.vendor
    )
    utils.run()
