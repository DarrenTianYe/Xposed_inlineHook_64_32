//
// Created by darren on 2022/1/7.
//


#ifndef FRIDADETECTOR_CONST_H
#define FRIDADETECTOR_CONST_H

#include <jni.h>
#include <android/log.h>


// Android log function wrappers
static const char* kTAG = "DarrenSys";

#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))


#endif //FRIDADETECTOR_CONST_H
