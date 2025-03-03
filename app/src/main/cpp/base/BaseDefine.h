//
// Created by 周豪豪 on 2025/3/2.
//

#ifndef AVLEARNING_BASEDEFINE_H
#define AVLEARNING_BASEDEFINE_H

#define AVResult int64_t
#define AV_ERROR -1
#define AV_OK 0

#include <string>
#include <functional>
#include "android/log.h"
#include "assert.h"

#define LOG_TAG "AVLearning"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#endif //AVLEARNING_BASEDEFINE_H
