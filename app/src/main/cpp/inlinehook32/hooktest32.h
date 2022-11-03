//
// Created by Administrator on 2019/7/8.
//

#ifndef INLINEHOOKTEST_HOOKTEST_H
#define INLINEHOOKTEST_HOOKTEST_H

#include <jni.h>

int testHook();
void hookRecvfrom();

int dvm_entry2(JNIEnv *env, jobject thiz);

#endif //INLINEHOOKTEST_HOOKTEST_H
