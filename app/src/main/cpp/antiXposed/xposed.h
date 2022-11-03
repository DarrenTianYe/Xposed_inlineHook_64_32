#pragma once

#include <jni.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void doAntiXposed(C_JNIEnv *env, jobject object, intptr_t hash);

void checkCallStack(C_JNIEnv *env);


int get_xposed_status(JNIEnv *env, int sdk);

extern int xposed_status;

#ifdef __cplusplus
}
#endif
