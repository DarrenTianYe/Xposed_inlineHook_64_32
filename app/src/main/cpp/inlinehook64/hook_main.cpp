//
// Created by darren on 2022/11/3.
//


#include <jni.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <android/log.h>
#include <stdlib.h>
#include "../const.h"
#include "And64InlineHook.hpp"

#if defined(__aarch64__)

static int  fgets_flags = 1;
int (*oldfgets)(char* const __pass_object_size dest, int size, FILE* stream) = NULL;
char* myfgets(char* const dest, int size, FILE* stream){

    LOGE("inlineHook myfgets success start:%d",fgets_flags);
    char* reslut = reinterpret_cast<char *>(oldfgets(dest, size, stream));
    fgets_flags++;

    return reslut;
}

void hook_start(){

    A64HookFunction((void*) fgets, (void *)myfgets, reinterpret_cast<void **>(&oldfgets));
    LOGE("inlineHook myfgets success end :%d",fgets_flags);

    LOGE("hook_start success ");

}

#endif // defined(__aarch64__)