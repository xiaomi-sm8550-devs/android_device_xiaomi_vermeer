/*
 * Copyright (C) 2026 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "vermeer_touch_patch"

#include <android/log.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <array>
#include <string>

namespace {

constexpr char kGoodixChipInfo[] =
        "/sys/devices/platform/goodix_ts.0/chip_info";
constexpr char kGoodixRegRw[] =
        "/sys/devices/platform/goodix_ts.0/reg_rw";
constexpr char kTsrNode[] =
        "/sys/devices/virtual/touch/touch_dev/bump_sample_rate";

constexpr size_t kRateConfigSize = 8;
constexpr unsigned char kRateCapable = 0x02;
constexpr unsigned int kRateHz = 500;

using RateConfig = std::array<unsigned char, kRateConfigSize>;

constexpr RateConfig kStockRateConfig = {
        0x14, 0xe0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
};

volatile sig_atomic_t stop_requested;

void Log(android_LogPriority priority, const char* message) {
    __android_log_write(priority, LOG_TAG, message);
}

void HandleSignal(int) {
    stop_requested = 1;
}

bool WriteText(const char* path, const std::string& value) {
    int fd = open(path, O_WRONLY | O_CLOEXEC);
    if (fd < 0)
        return false;

    const char* data = value.data();
    size_t remaining = value.size();
    while (remaining > 0) {
        ssize_t written = write(fd, data, remaining);
        if (written < 0 && errno == EINTR)
            continue;
        if (written <= 0) {
            close(fd);
            return false;
        }
        data += written;
        remaining -= written;
    }

    close(fd);
    return true;
}

bool ReadText(const char* path, std::string* value) {
    char buffer[512];
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return false;

    value->clear();
    for (;;) {
        ssize_t count = read(fd, buffer, sizeof(buffer));
        if (count < 0 && errno == EINTR)
            continue;
        if (count < 0) {
            close(fd);
            return false;
        }
        if (count == 0)
            break;
        value->append(buffer, count);
    }

    close(fd);
    return true;
}

bool ReadTsr(bool* enabled) {
    std::string value;
    char* end;

    if (!ReadText(kTsrNode, &value))
        return false;

    errno = 0;
    long parsed = strtol(value.c_str(), &end, 10);
    if (errno || end == value.c_str())
        return false;

    *enabled = parsed != 0;
    return true;
}

bool ParseRateConfig(const std::string& value, RateConfig* config) {
    const char* cursor = strchr(value.c_str(), '{');
    if (!cursor)
        return false;
    cursor++;

    for (size_t i = 0; i < config->size(); i++) {
        char* end;

        while (*cursor == ' ' || *cursor == '\t')
            cursor++;
        errno = 0;
        unsigned long byte = strtoul(cursor, &end, 16);
        if (errno || end == cursor || byte > 0xff)
            return false;
        (*config)[i] = static_cast<unsigned char>(byte);
        cursor = end;
    }

    return true;
}

bool ReadRateConfig(RateConfig* config) {
    std::string value;

    if (!WriteText(kGoodixRegRw, "r:15a40:8"))
        return false;
    usleep(2000);
    return ReadText(kGoodixRegRw, &value) &&
            ParseRateConfig(value, config);
}

bool WriteRateConfig(const RateConfig& config) {
    static constexpr char hex[] = "0123456789abcdef";
    std::string command = "w:15a40:8";

    for (unsigned char byte : config) {
        command.push_back(':');
        command.push_back(hex[byte >> 4]);
        command.push_back(hex[byte & 0x0f]);
    }

    return WriteText(kGoodixRegRw, command);
}

bool IsPatched(const RateConfig& config) {
    return (config[0] & kRateCapable) &&
            config[3] == (kRateHz & 0xff) &&
            config[4] == (kRateHz >> 8) &&
            config[5] == (kRateHz & 0xff) &&
            config[6] == (kRateHz >> 8);
}

RateConfig MakePatchedConfig(RateConfig config) {
    config[0] |= kRateCapable;
    config[3] = kRateHz & 0xff;
    config[4] = kRateHz >> 8;
    config[5] = kRateHz & 0xff;
    config[6] = kRateHz >> 8;
    return config;
}

bool WriteAndVerify(const RateConfig& config) {
    RateConfig verify;

    if (!WriteRateConfig(config))
        return false;
    usleep(2000);
    return ReadRateConfig(&verify) && verify == config;
}

bool IsSupportedFirmware() {
    std::string chip_info;

    if (!ReadText(kGoodixChipInfo, &chip_info))
        return false;

    return chip_info.find("patch_pid:9916R") != std::string::npos &&
            chip_info.find("patch_vid:05800289") != std::string::npos;
}

bool WaitForTouch() {
    while (!stop_requested) {
        if (access(kGoodixChipInfo, R_OK) == 0 &&
                access(kGoodixRegRw, R_OK | W_OK) == 0 &&
                access(kTsrNode, R_OK | W_OK) == 0 &&
                IsSupportedFirmware())
            return true;
        sleep(1);
    }

    return false;
}

}  // namespace

int main() {
    RateConfig restore_config = kStockRateConfig;
    bool patched = false;

    signal(SIGINT, HandleSignal);
    signal(SIGTERM, HandleSignal);

    if (!WaitForTouch())
        return EXIT_SUCCESS;

    Log(ANDROID_LOG_INFO, "supported Goodix firmware detected");
    RateConfig initial_config;
    if (ReadRateConfig(&initial_config) && IsPatched(initial_config)) {
        patched = true;
        Log(ANDROID_LOG_WARN,
                "existing Vermeer runtime patch detected; stock restore armed");
    }

    while (!stop_requested) {
        RateConfig current;
        bool tsr_enabled;

        if (!ReadTsr(&tsr_enabled)) {
            usleep(250000);
            continue;
        }
        if (!tsr_enabled && !patched) {
            usleep(100000);
            continue;
        }
        if (!ReadRateConfig(&current)) {
            usleep(250000);
            continue;
        }

        if (tsr_enabled && !IsPatched(current)) {
            if (!WriteText(kTsrNode, "0")) {
                Log(ANDROID_LOG_ERROR, "failed to reset TSR state");
                usleep(250000);
                continue;
            }
            usleep(10000);

            if (!ReadRateConfig(&restore_config)) {
                Log(ANDROID_LOG_ERROR, "failed to back up rate config");
                WriteText(kTsrNode, "1");
                usleep(250000);
                continue;
            }

            if (!WriteAndVerify(MakePatchedConfig(restore_config))) {
                Log(ANDROID_LOG_ERROR, "failed to patch rate config");
                WriteAndVerify(restore_config);
                WriteText(kTsrNode, "1");
                usleep(250000);
                continue;
            }
            usleep(1500);

            if (!WriteText(kTsrNode, "1")) {
                Log(ANDROID_LOG_ERROR, "failed to reactivate TSR");
                WriteAndVerify(restore_config);
                WriteText(kTsrNode, "0");
                usleep(250000);
                continue;
            }

            patched = true;
            Log(ANDROID_LOG_INFO, "Vermeer 500Hz runtime patch applied");
        } else if (tsr_enabled && IsPatched(current)) {
            patched = true;
        } else if (!tsr_enabled && patched) {
            if (!WriteAndVerify(restore_config)) {
                Log(ANDROID_LOG_ERROR, "failed to restore rate config");
                usleep(250000);
                continue;
            }
            WriteText(kTsrNode, "0");
            patched = false;
            restore_config = kStockRateConfig;
            Log(ANDROID_LOG_INFO, "Vermeer rate config restored");
        }

        usleep(tsr_enabled ? 1000000 : 100000);
    }

    if (patched) {
        WriteAndVerify(restore_config);
        WriteText(kTsrNode, "0");
    }
    return EXIT_SUCCESS;
}
