#include <jni.h>
#include <string>

#include "include/inlineHook.h"
#include "hooktest.h"
#include  <android/log.h>


#define TAG "darren"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)



extern "C" JNIEXPORT jstring JNICALL
Java_com_wizd_inlinehooktest_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    testHook();
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_wizd_xposedinlinehook_imp_HookNativeImpl_testNative(JNIEnv *env, jclass type) {

    testHook();
    LOGD("Java_com_wizd_xposedinlinehook_imp_HookNativeImpl_testNative end ");
   // hookRecvfrom();

}