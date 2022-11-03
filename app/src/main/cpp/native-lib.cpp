#include <jni.h>
#include <string>

#include "inlinehook32/inlineHook.h"
#include "inlinehook32/hooktest32.h"
#include "dvm/dvm.h"
#include "antiXposed/xposed.h"
#include  <android/log.h>
#include "inlinehook64/hooktest64.h"


#define TAG "darren"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


//com.system.installer.acc.
extern "C" JNIEXPORT jstring JNICALL
Java_com_system_installer_acc_JNI_myTest32(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

#if defined(__arm__)
    testHook();
#endif  //#if defined(__arm__)

    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_system_installer_acc_JNI_myTest64(JNIEnv *env, jclass className, jobject ctx) {

#if defined(__aarch64__)
    hook_start();
#endif  //#if defined(__aarch64__)

    return NULL;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_system_installer_acc_JNI_myTestComm(JNIEnv *env, jclass className, jobject ctx) {

    LOGD("dvm_entry end >>>>>>>: Java_com_wizd_xposedinlinehook_imp_HookNativeImpl_testNative");
    dvm_entry2(env,ctx);
//  int xposed_stat = get_xposed_status(env, android_get_device_api_level());
//  char *tmp = getDvmBlackSoAndUid(env, ctx, &error);
    int ret = 0;//dvm_entry2(env, ctx);
    LOGD("dvm_entry end >>>>>>>: %d", ret);

    return NULL;
}