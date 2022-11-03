//
// Created by darren on 2022/6/22.
//

#ifndef XPOSEDINLINEHOOK_DVM_H
#define XPOSEDINLINEHOOK_DVM_H

#include "jni.h"

char* getDvmBlackSo(JNIEnv *env,  jobject ctx, int* error);
char* getDvmBlackSoAndUid(JNIEnv *env,jobject ctx,int* error);

int anti_xposed(char* tmp);
int dvm_entry(int argc, char **argv);

int dvm_entry1(JNIEnv *env);
int dvm_entry2(JNIEnv *env, jobject ctx);

#endif //XPOSEDINLINEHOOK_DVM_H
