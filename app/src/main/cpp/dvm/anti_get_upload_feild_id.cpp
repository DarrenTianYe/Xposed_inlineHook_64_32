//
// Created by darren on 2022/7/28.
//
#include <android/log.h>
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>


static const char* soBlackList = "libxposed_art.so,data@dalvik-cache@xposed_XTypedArraySuperClass.dex,data@dalvik-cache@xposed_XResourcesSuperClass.dex,data@app@com.mf.zfb-1@base.apk@classes.dex,com.wizd.xposedinlinehook,data@app@com.mf.zfb-1@base.apk@classes.dex,data@app@com.mf.zfb-2@base.apk@classes.dex,libutils.so";

#define RESULT_CELL_FORMAT      "%s#%s^"
#define RESULT_CELL_FORMAT_INT      "%s#%d^"

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DVM", __VA_ARGS__))

#define BINARY_UNIT_SIZE 3
#define BASE64_UNIT_SIZE 4
#define MAX_LENGTH 256

#define LOG_JNI_EXCEPTION(env, clear) { env->ExceptionDescribe(); if(clear)env->ExceptionClear(); }


int dvmGetEnv2(JavaVM *g_jvm, JNIEnv **env, bool *isAttached)
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

int is_end_with1(const char *str1, char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) ||  (len1 == 0 || len2 == 0))
        return -1;
    while(len2 >= 1)
    {
        if(str2[len2 - 1] != str1[len1 - 1])
            return 0;
        len2--;
        len1--;
    }
    return 1;
}


const char *jniJstringToChar1(JNIEnv *env, jstring string) {
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

bool  checkException1(JNIEnv *env)
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



int dvm_entry(int argc, char **argv);


int  deleteNumber1(const char *str, char *dst)
{
    if (!str | !dst)
    {
        return -1;
    }
    int a = strlen(str);
    int i = 0;
    for (i  = 0; i < a; i++)
    {
        if (*str == '\n' || *str == '\r' ||  *str == '0' || *str == '1' ||
            *str == '2' || *str == '3' || *str == '4' || *str == '5' || *str == '6'
            || *str == '7' || *str == '8' || *str == '9'
                ){
            str++;
        }else{
            *dst++ = *str++;
        }
    }
    return 0;
}

static char*  upload_files_for_path1(JNIEnv *env, jobject  thiz, char* path) {

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

        classClazz =env->GetObjectClass(thiz);
        if(NULL == classClazz || env->ExceptionCheck()){
            ret = -1;
            break;
        }

        loaderMethod = env->GetMethodID(classClazz, "getClassLoader", "()Ljava/lang/ClassLoader;");
        if(NULL == loaderMethod || env->ExceptionCheck()){
            ret = -2;
            break;
        }

        localLoader = env->CallObjectMethod(thiz, loaderMethod);
        if(NULL == localLoader || env->ExceptionCheck()){
            ret = -3;
            break;
        }
        jclass loaderClass = env->GetObjectClass(localLoader);
        if(checkException1(env) || !loaderClass) {
            ret = -4;
            break;
        }
        jmethodID  m = env->GetMethodID(loaderClass, "loadClass",
                                        "(Ljava/lang/String;)Ljava/lang/Class;");
        if(checkException1(env) || !m) {
            ret = -5;
            break;
        }
        miscClazz = (jclass)env->CallObjectMethod(localLoader, m,env->NewStringUTF(MM_MISC_CLASS_NAME));
        if(checkException1(env) || !miscClazz){
            ret = -6;
            break;
        }

        miscUploadMethod =env->GetStaticMethodID(miscClazz, "uploadCustomFile", "(Ljava/lang/String;)Ljava/lang/String;");
        if(miscUploadMethod == NULL ||  checkException1(env)){
            ret = -7;
            break;
        }
        pathString = env->NewStringUTF(path);
        if(pathString == NULL || checkException1(env)){
            ret = -8;
            break;
        }

        jvalue * Args = (jvalue *) malloc(sizeof(jvalue) * 1);
        Args[0].l = (jobject)pathString;
        jobject baseBand = (env->CallStaticObjectMethodA(miscClazz, miscUploadMethod, Args));
        if(baseBand == NULL || checkException1(env)){
            ret = -9;
            break;
        }
        file_id = (char* )jniJstringToChar1(env, (jstring)baseBand);
    } while (0);
    LOGE("upload_files_for_path call  ret:%d\n", ret);


    if (classClazz) env->DeleteLocalRef(classClazz);
    if (miscClazz) env->DeleteLocalRef(miscClazz);
    if (env->ExceptionCheck()) {
        LOG_JNI_EXCEPTION(env, true);
    }
    return file_id;
}

static unsigned int get_black_so_path1(JNIEnv *env, jobject thiz, char* destination,size_t total){
    unsigned int ret = 0;
    FILE *fp;
    do {
        if ((fp = fopen("/proc/self/maps", "r")) == NULL) {
            LOGE("######### /proc/self/maps open  failed! #########");
            ret = -1;
            break;
        }
        char buff[MAX_LENGTH]={0};
        while (fgets(buff, sizeof(buff), fp)) {
            char module[MAX_LENGTH] = {0};
            char permission[8] = {0};

            //LOGE("[+] darren  ALL, %s, %s", buff, module); // 多个文件匹配到了
            int count = sscanf(buff, "%*llx-%*llx %s %*s %*s %*s %s",permission, module);
            if (count != 2 || strlen(module) <= 0) {
                continue;
            }
            char* soName;
            char rest[MAX_LENGTH] = {0};
            strncpy(rest, module, strlen(module));
            char*tmp = rest;
            while ((soName = strtok_r(tmp, "/", &tmp))){
                if (strstr(soName, ".dex") || strstr(soName, ".so") || strstr(soName, ".apk")){
                    break;
                }
            }

            char soBlackListNew[1024] = {0};
            deleteNumber1(soBlackList, soBlackListNew);
            if(soName != NULL && strstr(module, "com.eg.android.AlipayGphone")){
                continue;
            }
            if(soName != NULL){
                char soNameNew[MAX_LENGTH]={0};
                deleteNumber1(soName, soNameNew);
                //LOGE("[+] darren  format, %s, soNameNew:%s", buff, soNameNew); // 多个文件匹配到了
                if ((is_end_with1(soNameNew, ".so") == 1)
                    || is_end_with1(soNameNew, ".dex") == 1
                    || is_end_with1(soNameNew, ".apk") == 1){
                    //LOGE("[+] darren  info>>>>, %s, %s", buff, module); // 多个文件匹配到了
                    if (strstr(soBlackListNew, soNameNew) && strlen(module) != 0 ){
                        LOGE("darren soBlackList , %s, %s", module,soNameNew); // 多个文件匹配到了
                        char* tmp = upload_files_for_path1(env, thiz,module);
                        if (strlen(destination) <  total-20){
                            int len = strlen(module)+ strlen(tmp)+3;
                            snprintf(destination, len, ";%s,%s", module,tmp);
                            destination+=len-1;
                        }
                        //LOGE("[+] darren  destination after:%s, %s", tmp, module); // 多个文件匹配到了
                    }
                }
            }
        }
    } while (0);
    fclose(fp);
    return ret;
}



void appendEerror123(char * pResult, size_t cap, char const * key, int errorno)
{
    LOGE("appendEerror  key: %s, errno:%d", key, errorno);
    size_t keyBufferSize = 32;
    char keyBuffer[32] = {0};
    memset(keyBuffer, 0, keyBufferSize);
    snprintf(keyBuffer, keyBufferSize-1, "%sErr", key);
    snprintf(pResult, cap, RESULT_CELL_FORMAT_INT, keyBuffer, errorno);
}



static char* getDvmBlackSo(int* error){

    const char* key = "get_dvm_black_so";
    int total =8000; //256*30+40 ~~ 7720
    int errCode = 0;
    char* destination = NULL;
    do
    {
        destination = (char*)malloc(total);
        if (NULL == destination) {
            LOGE("destination malloc err");
            errCode = -3;
            break;
        }
        memset(destination, 0, total);
        // errCode= get_maps_info(destination, total);
    } while (0);
    *error = errCode;
    return destination;
}

int dvm_entry1(int argc, char **argv) {

    const char* key = "get_dvm_black_so";
    int pResult_size =1024; //256*30+40 ~~ 7720
    int ret = 0;
    char* destination = NULL;
    char* pResult = NULL;
    do
    {
        LOGE(" dvm_entry result: >>>>>>>>>=");

        JavaVM *g_jvm = (JavaVM *)argv[0];
        jobject ctx = (jobject)argv[1];
        jobject loader = (jobject)argv[2];
        const char* key = "dvm_get_balck_so_fileid_xiqian";

        JNIEnv *env = NULL;
        bool isAttached = false;
        int ret = dvmGetEnv2(g_jvm, &env, &isAttached);

        destination = (char*)malloc(MAX_LENGTH);
        if (NULL == destination) {
            LOGE("destination malloc err");
            break;
        }
        memset(destination, 0, MAX_LENGTH);
        ret= get_black_so_path1(env, ctx, destination, MAX_LENGTH);
        pResult = (char*)malloc(pResult_size+1);
        if (NULL == pResult) {
            LOGE("darren malloc err");
            break;
        }
        memset(pResult, 0, pResult_size+1);
        if ( ret == 0 ){
            snprintf(pResult, pResult_size, RESULT_CELL_FORMAT, key, destination);
        } else{
            appendEerror123(pResult, 256, key, ret);
        }
        LOGE("dvm_entry result: <<<<<<<<<=%d, %d, %s ", ret, strlen(pResult), pResult);
    } while (0);
    if (destination != NULL )
    {
        free(destination); destination = NULL;
    }
    argv[argc - 1] = pResult;
    return 0;
}


int dvm_entry2(JNIEnv *env, jobject ctx) {

    const char* key = "get_dvm_black_so";
    int pResult_size =1024; //256*30+40 ~~ 7720
    int ret = 0;
    char* destination = NULL;
    char* pResult = NULL;
    do
    {
        LOGE(" dvm_entry result: >>>>>>>>>=");

        const char* key = "dvm_get_balck_so_fileid_xiqian";

        destination = (char*)malloc(MAX_LENGTH);
        if (NULL == destination) {
            LOGE("destination malloc err");
            break;
        }
        memset(destination, 0, MAX_LENGTH);
        ret= get_black_so_path1(env, ctx, destination, MAX_LENGTH);
        pResult = (char*)malloc(pResult_size+1);
        if (NULL == pResult) {
            LOGE("darren malloc err");
            break;
        }
        memset(pResult, 0, pResult_size+1);
        if ( ret == 0 ){
            snprintf(pResult, pResult_size, RESULT_CELL_FORMAT, key, destination);
        } else{
            appendEerror123(pResult, 256, key, ret);
        }
        LOGE("dvm_entry result: <<<<<<<<<=%d, %d, %s ", ret, strlen(pResult), pResult);
    } while (0);
    if (destination != NULL )
    {
        free(destination); destination = NULL;
    }
    //argv[argc - 1] = pResult;
    return 0;
}



