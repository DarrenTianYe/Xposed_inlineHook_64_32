#include <stdio.h>
#include <android/legacy_stdlib_inlines.h>
#include <sys/socket.h>

#include "include/inlineHook.h"
#include "hooktest.h"

#include  <android/log.h>
#include <string.h>
#include <unistd.h>

#define TAG "darren"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

int (*old_rand)() = NULL;

int new_rand()
{
    return 100;
}

int hook()
{
    if (registerInlineHook((uint32_t) rand, (uint32_t) new_rand, (uint32_t **) &old_rand) != ELE7EN_OK) {
        return -1;
    }
    if (inlineHook((uint32_t) rand) != ELE7EN_OK) {
        return -1;
    }

    return 0;
}

int unHook()
{
    if (inlineUnHook((uint32_t) rand) != ELE7EN_OK) {
        return -1;
    }

    return 0;
}


// system lib
//#include <unistd.h>
//
//// user lib
//#include <Logger.h>

extern "C"{
// 如果功能需要被dlsym调用出来，那么传入的应该是函数的symbpls
// 因此 如果编译选项开启了-fvisibility=hidden
// 那么就需要用__attribute__((visibility ("default")))修饰被调用的功能 以确保功能可以被外部调用
// 同时用extern "C"保证以c风格编译(c风格编译，symbols是函数名本身)
// 而不是c++风格编译(c++风格编译后，symbaps为__ZXXX_XX格式)
// 或者自行ida查看symbols
__attribute__((visibility ("default"))) void hello(){
    LOGD("This fun comed from libHook.so::hello ~~");
}
}

// 先atmain函数执行的 最高执行权重函数
__unused __attribute__((constructor)) void constructor_main(){
    LOGD("Inject Ok");

    int pid = getpid();

    testHook();
    //hookRecvfrom();
    LOGD("Hello I'am from libHook.so ~~ from pid:%d", pid);

    LOGD("Inject Finished");
}


int testHook() {
    int n = rand();
    LOGI("rand return: %d.", n);

    n = hook();
    LOGI("hook return: %d.", n);

    n = rand();
    LOGI("rand return: %d.", n);

    n = unHook();
    LOGI("unHook return: %d.", n);

    n = rand();
    LOGI("rand return: %d.", n);

    return 0;
}


int (*old_recvfrom)() = NULL;

//ssize_t recvfrom(int __fd, void* __buf, size_t __n, int __flags, struct sockaddr* __src_addr, socklen_t* __src_addr_length)
ssize_t new_recvfrom(int __fd, void* __buf, size_t __n, int __flags, struct sockaddr* __src_addr, socklen_t* __src_addr_length)
{
    memset(__buf, 0, __n);
    return -1;
}

void hookRecvfrom(){
    if (registerInlineHook((uint32_t) recvfrom, (uint32_t) new_recvfrom, (uint32_t **) &old_recvfrom) != ELE7EN_OK) {
        LOGD("registerInlineHook failed.");
    }
    if (inlineHook((uint32_t) recvfrom) != ELE7EN_OK) {
        LOGD("inlineHook failed.");
    }

    LOGD("hook succeed.");
}
