#pragma once

// C Headers
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>

// Debugbreak
#if defined(_MSC_VER)
#define DEBUG_BREAK __debugbreak()
#else
#include <signal.h>
#define DEBUG_BREAK raise(SIGTRAP)
#endif

// XR_DOCS_TAG_BEGIN_Helper_Functions1
inline bool IsStringInVector(std::vector<const char *> list, const char *name) {
    bool found = false;
    for (auto &item : list) {
        if (strcmp(name, item) == 0) {
            found = true;
            break;
        }
    }
    return found;
}

template <typename T>
inline bool BitwiseCheck(const T &value, const T &checkValue) {
    return ((value & checkValue) == checkValue);
}
// XR_DOCS_TAG_END_Helper_Functions1

#define XR_DOCS_CHAPTER_2_1 0x21
#define XR_DOCS_CHAPTER_2_2 0x22
#define XR_DOCS_CHAPTER_2_3 0x23

#define XR_DOCS_CHAPTER_3_1 0x31
#define XR_DOCS_CHAPTER_3_2 0x32
#define XR_DOCS_CHAPTER_3_3 0x33

#define XR_DOCS_CHAPTER_4_1 0x41
#define XR_DOCS_CHAPTER_4_2 0x42
#define XR_DOCS_CHAPTER_4_3 0x43
#define XR_DOCS_CHAPTER_4_4 0x44
#define XR_DOCS_CHAPTER_4_5 0x45
#define XR_DOCS_CHAPTER_4_6 0x46

#define XR_DOCS_CHAPTER_5_1 0x51
#define XR_DOCS_CHAPTER_5_2 0x52
#define XR_DOCS_CHAPTER_5_3 0x53
#define XR_DOCS_CHAPTER_5_4 0x54
#define XR_DOCS_CHAPTER_5_5 0x55
#define XR_DOCS_CHAPTER_5_6 0x56