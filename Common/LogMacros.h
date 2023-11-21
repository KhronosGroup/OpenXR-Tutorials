// Copyright 2023, Collabora, Ltd.
//
// SPDX-License-Identifier: BSL-1.0

/**
 * @file
 * @brief Provides `LOG_INFO()` and `LOG_ERROR()` macros that can
 * contain a stream output expression: it will be routed to logcat
 * on android and stdout/stderr elsewhere.
 */

#pragma once

#ifdef __bionic__

#include <android/log.h>

#include <sstream>

#define LOG_TAG "openxr_tutorial"
#define LOG_INFO(...)                                                   \
    do {                                                                \
        std::ostringstream os;                                          \
        os << __VA_ARGS__;                                              \
        __android_log_write(ANDROID_LOG_INFO, LOG_TAG, os.str.c_str()); \
    } while (0)
#define LOG_ERROR(...)                                                   \
    do {                                                                 \
        std::ostringstream os;                                           \
        os << __VA_ARGS__;                                               \
        __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, os.str.c_str()); \
    } while (0)
#else
#include <iostream>

#define LOG_INFO(...) std::cout << __VA_ARGS__ << "\n"
#define LOG_ERROR(...) std::cerr << __VA_ARGS__ << "\n"
#endif
