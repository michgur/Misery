//
// Created by micha on 8/14/2020.
//

#ifndef MISERY_LOGGING_H
#define MISERY_LOGGING_H

#include <android/log.h>

#define ASSERT(condition, ...) if (!(condition)) __android_log_assert(#condition, "misery-native", __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "misery-native", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "misery-native", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "misery-native", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "misery-native", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "misery-native", __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, "misery-native", __VA_ARGS__)
#define JNI_CATCH_EXCEPTION(...) if (env->ExceptionCheck()) { \
    LOGF(__VA_ARGS__); \
    env->ExceptionDescribe(); \
    env->ExceptionClear(); \
    std::abort(); \
}

#endif //MISERY_LOGGING_H
