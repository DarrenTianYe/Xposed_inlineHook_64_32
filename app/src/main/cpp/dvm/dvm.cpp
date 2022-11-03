//
// Created by darren on 2022/6/22.
//

#include "dvm.h"

#include <android/log.h>
#include <jni.h>
#include <math.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <inttypes.h>
#include <pthread.h>
#include <assert.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string>


#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DVM_darren", __VA_ARGS__))


#define PAGE_START(addr) ((addr) & PAGE_MASK)
#define PAGE_END(addr)   (PAGE_START(addr) + PAGE_SIZE)

#define RESULT_CELL_FORMAT      "%s#%s^"
#define RESULT_CELL_FORMAT_INT      "%s#%d^"

#define LOG_JNI_EXCEPTION(env, clear) { env->ExceptionDescribe(); if(clear)env->ExceptionClear(); }



int dvm_entry(int argc, char **argv);

int dvmGetEnv(JavaVM *g_jvm, JNIEnv **env, bool *isAttached)
{
    int ret = 0;

    do {
        //获取env
        int status = g_jvm->GetEnv((void **) env, JNI_VERSION_1_4);
        if (status < 0)
        {
            LOGE("[*] start to AttachCurrentThread with status:%d\n", status);
            g_jvm->AttachCurrentThread(env, NULL);
            if ( NULL == *env)
            {
                //返回错误码
                ret = 101;
                LOGE("[-] still cant attach thread");
                break;
            }
            *isAttached = true;
            LOGE("[*] AttachCurrentThread succ\n");
        }
        LOGE("[*] vm env :%p", *env);
    }while(0);

    return ret;
}


const char *jniJstringToChar(JNIEnv *env, jstring string) {
    if (NULL == string) {
        return "";
    }
    const char *nativeString = env->GetStringUTFChars(string, NULL);
    env->ReleaseStringUTFChars(string, nativeString);
    return nativeString;
}

#define LINE_NUMBER_MASK       (0x3FFF)  // 15  bits
#define ERROR_MASK             (0xFFFF)  // 16  bits

#define MM_MISC_CLASS_NAME  "com/alipay/apmobilesecuritysdk/tool/collector/mmMisc"


#define CHECK_BREAK(ret) \
    if (ret != 0) \
    { \
        LOGE("[vm_printf] CALL failed! ret:%d\n", ret); \
        ret = ((((__LINE__ & LINE_NUMBER_MASK) << 16) | (ERROR_MASK & ret)) & (~(0X01 << 31))); \
        LOGE("[vm_printf] CALL failed! ret:%d\n", ret); \
        break; \
    }

 bool  checkException(JNIEnv *env)
{
    if (NULL == env)
    {
        LOGE("[+] env is NULL\n");
        return true;
    }
    if (env->ExceptionCheck())
    {
        LOGE("JNI Exception!\n");
        env->ExceptionDescribe();
        env->ExceptionClear();
        return true;
    }
    jthrowable err = env->ExceptionOccurred();
    if (err != NULL){
        env->ExceptionClear();
    }
    return false;
}

static jstring getClsName(JNIEnv *env, jclass myCls){

    jclass  objcls = NULL;
    jmethodID mid = NULL;
    jstring strObj= NULL;
    int ret = 0;
    do{
        objcls = env->FindClass("java/lang/Class");
        if(NULL == objcls || env->ExceptionCheck()){
            ret = -1;
            break;
        }
        mid = env->GetMethodID(objcls, "getName", "()Ljava/lang/String;");
        if(NULL == mid || env->ExceptionCheck()){
            ret = -2;
            break;
        }
        strObj = (jstring)env->CallObjectMethod(myCls, mid);
        if(NULL == strObj || env->ExceptionCheck()){
            ret = -3;
            break;
        }
    } while (0);
    if (objcls) env->DeleteLocalRef(objcls);
    return strObj;
}

char*  upload_files_for_path(JNIEnv *env, jobject  thiz, char* path) {


    LOGE("upload_files_for_path==start");
    jclass classClazz = NULL;
    jclass miscClazz =NULL;
    jmethodID loaderMethod = NULL;
    jmethodID miscUploadMethod = NULL;
    jobject localLoader = NULL;
    jstring pathString = NULL;
    char* file_id = NULL;

    int  ret = 0;
    do {

        LOGE("upload_files_for_path==start 11");

        classClazz =env->GetObjectClass(thiz);
        if(NULL == classClazz || env->ExceptionCheck()){
            ret = -1;
            break;
        }

        LOGE("upload_files_for_path==start 22");

        loaderMethod = env->GetMethodID(classClazz, "getClassLoader", "()Ljava/lang/ClassLoader;");
        if(NULL == loaderMethod || env->ExceptionCheck()){
            ret = -2;
            break;
        }

        LOGE("upload_files_for_path==start 33");

        localLoader = env->CallObjectMethod(thiz, loaderMethod);
        if(NULL == localLoader || env->ExceptionCheck()){
            ret = -3;
            break;
        }


        LOGE("upload_files_for_path==start 44");

        jclass loaderClass = env->GetObjectClass(localLoader);
        if(checkException(env) || !loaderClass) {
            ret = -4;
            break;
        }
        LOGE("upload_files_for_path==start 45");

        jmethodID  m = env->GetMethodID(loaderClass, "loadClass",
                                        "(Ljava/lang/String;)Ljava/lang/Class;");
        if(checkException(env) || !m) {
            ret = -5;
            break;
        }
        LOGE("upload_files_for_path==start 46");

        miscClazz = (jclass)env->CallObjectMethod(localLoader, m,env->NewStringUTF(MM_MISC_CLASS_NAME));
        if(checkException(env) || !miscClazz){
            ret = -6;
            break;
        }

//        miscClazz = env->FindClass("com/alipay/apmobilesecuritysdk/tool/collector/mmMisc");
//        if(checkException(env) || !miscClazz){
//            ret = -6;
//            break;
//        }

        LOGE("upload_files_for_path==start 55");

        miscUploadMethod =env->GetStaticMethodID(miscClazz, "uploadCustomFile", "(Ljava/lang/String;)Ljava/lang/String;");
        if(miscUploadMethod == NULL ||  checkException(env)){
            ret = -7;
            break;
        }
        pathString = env->NewStringUTF(path);
        if(pathString == NULL || checkException(env)){
            ret = -8;
            break;
        }

        LOGE("upload_files_for_path==start 6655555:%s", path);

//        jvalue * Args = (jvalue *) malloc(sizeof(jvalue) * 1);
//        Args[0].l = (jobject)pathString;

        jobject baseBand =env->CallStaticObjectMethod(miscClazz, miscUploadMethod, pathString);
        if(baseBand == NULL || checkException(env)){
            ret = -9;
            break;
        }

        LOGE("upload_files_for_path==start 77");

        file_id = (char* )jniJstringToChar(env, (jstring)baseBand);
    } while (0);
    LOGE("upload_files_for_path call  ret:%d\n", ret);


    if (classClazz) env->DeleteLocalRef(classClazz);
    if (miscClazz) env->DeleteLocalRef(miscClazz);
    if (env->ExceptionCheck()) {
        LOG_JNI_EXCEPTION(env, true);
    }

    return file_id;
}

void appendEerror(char * pResult, size_t cap, char const * key, int errorno)
{
    LOGE("appendEerror  key: %s, errno:%d", key, errorno);
    size_t keyBufferSize = 32;
    char keyBuffer[32] = {0};
    memset(keyBuffer, 0, keyBufferSize);
    snprintf(keyBuffer, keyBufferSize-1, "%sErr", key);

    snprintf(pResult, cap, RESULT_CELL_FORMAT_INT, keyBuffer, errorno);
}


char*  getDvmBlackSo(JNIEnv *env,  jobject ctx, int* error){

    const char* key = "get_dvm_black_so_apk_info";
    int total =8000; //256*30+40 ~~ 7720
    int errCode = 0;
    char* destination = NULL;
    char* tmp = NULL;
    do
    {
        destination = (char*)malloc(total);
        if (NULL == destination) {
            LOGE("destination malloc err");
            errCode = -3;
            break;
        }
        memset(destination, 0, total);
        tmp = upload_files_for_path(env, ctx, "/system/lib64/libc++.so");
    } while (0);
    LOGE("destination malloc err:%s", tmp);
    *error = errCode;

    return destination;
}


//
//int dvm_entry(int argc, char **argv) {
//
//    LOGE(" dvm_entry result: <<<<<<<<<=");
//
//    JavaVM *g_jvm = (JavaVM *)argv[0];
//    jobject ctx = (jobject)argv[1];
//    jobject loader = (jobject)argv[2];
//
//    const char* key = "dvm_get_file_id";
//    char*  pResult = (char*)malloc(256);
//    memset(pResult, 0, 256);
//
//
//    JNIEnv *env = NULL;
//    bool isAttached = false;
//    int ret = dvmGetEnv(g_jvm, &env, &isAttached);
//
//    char* tmp = upload_files_for_path(env, ctx, "/system/lib64/libc++.so");
//    if ( tmp != NULL){
//        snprintf(pResult, 256, RESULT_CELL_FORMAT, key, tmp);
//    }
//    LOGE(" dvm_entry result: <<<<<<<<<=%s", pResult);
//    argv[argc - 1] = pResult;
//
//    return 0;
//}

