//
// Created by darren on 2022/8/8.
//
#include <jni.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <android/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>



#define RESULT_CELL_FORMAT      "%s$%s^"
#define RESULT_CELL_FORMAT_INT      "%s$%d^"


#define MAX_LENGTH 256
#define P_RESULT_LENGTH 91024
#define P_INPUT_LENGTH  5000
#define P_MAPS_LINE_MAX 500


#define LOG_JNI_EXCEPTION(env, clear) { env->ExceptionDescribe(); if(clear)env->ExceptionClear(); }
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DVM_env", __VA_ARGS__))



char *g_input_package_black_list[MAX_LENGTH]={0};
static int g_input_black_packagelist_size = 0;


char *g_maps_mathed[P_MAPS_LINE_MAX] ={0};
static int g_maps_mathed_size = 0;


char *g_maps_package[P_MAPS_LINE_MAX] = {0};
static int g_maps_package_size = 0;


//
int dvmGetEnv1(JavaVM *g_jvm, JNIEnv **env, bool *isAttached)
{
    int ret = 0;

    do {
        //获取envg_maps_package
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

char * __cdecl mystrstr(const char *str1, const char *str2)
{

    char *cp = (char *)str1;
    char *s1, *s2;
    if (!*str2)
        return((char *)str1);
    while (*cp)
    {
        s1 = cp;
        s2 = (char *)str2;
        while (*s2 && !(*s1 - *s2))
            s1++, s2++;
        if (!*s2)
            return(cp);
        cp++;
    }
    return NULL;
}

long file_st_mtime(const char * file_name)
{
    if (NULL != file_name) {
        if (access(file_name, F_OK) == 0) {
            struct stat buf;
            memset(&buf, 0, sizeof(buf));
            if (0 == stat(file_name, &buf)) {
                return buf.st_mtime;
            }
        }
    }
    return -1;
}

bool isDuplicate(const char* module){

    if (module != NULL){
        for (int i = 0; g_maps_package[i]; i++){
            if (mystrstr(g_maps_package[i], module)){
                return true;
            }
        }
    }
    return false;
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

void appendData(char* destination, char* tmp, size_t destination_len){

    if (tmp && destination && strlen(destination) <  destination_len-20){
        int len = strlen(destination);
        if (len <  destination_len-100){
            strcat(destination,(char*)tmp);
            strcat(destination,";");
        }
    }
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

int  deleteNull(const char *str, char *dst)
{
    if (!str | !dst)
    {
        return -1;
    }
    int a = strlen(str);
    int i = 0;
    for (i  = 0; i < a; i++)
    {
        if (*str == '\n' || *str == '\r' || *str == ' '){
            str++;
        }else{
            *dst++ = *str++;
        }
    }
    return 0;
}

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


int  deleteNumber2(const char *str, char *dst)
{
    if (!str | !dst)
    {
        return -1;
    }
    int a = strlen(str);
    int i = 0;
    for (i  = 0; i < a; i++)
    {
        if (*str == '\n' || *str == '\r' ||  *str == '0'
            || *str == '1' || *str == ' ' || *str == '-'
            || *str == '2' || *str == '3' || *str == '4' || *str == '5' || *str == '6'
            || *str == '7' || *str == '8' || *str == '9'
                ){
            str++;
        }else{
            *dst++ = *str++;
        }
    }
    return 0;
}

void split_m(char str[],const char* splitSign)
{

    char* soName;
    char rest[P_INPUT_LENGTH] = {0};
    strncpy(rest, str, strlen(str));
    char*tmp = rest;
    while ((soName = strtok_r(tmp, splitSign, &tmp))){
        int s_len =strlen(soName);
        if (soName && s_len > 0 && s_len < MAX_LENGTH){
            g_input_package_black_list[g_input_black_packagelist_size] =(char*)malloc(MAX_LENGTH);
            if (g_input_package_black_list[g_input_black_packagelist_size] != NULL){
                memset(g_input_package_black_list[g_input_black_packagelist_size], 0, MAX_LENGTH);
                deleteNumber2(soName, g_input_package_black_list[g_input_black_packagelist_size]);
                LOGE("destination g_input_package_black_list 22222,%s",g_input_package_black_list[g_input_black_packagelist_size]);
                ++g_input_black_packagelist_size;
            }
        }
    }
}

int maps_match_all_info(char* buff, char* permission){

    for (int i = 0; g_input_package_black_list[i]; i++){
        char info[MAX_LENGTH]={0};
        deleteNull(buff,info);
        if (info != NULL && g_input_package_black_list[i] != NULL && mystrstr(permission, "r--p") && mystrstr(info, g_input_package_black_list[i]))
        {
            int s_len =strlen(info);
            if (s_len > 0 && s_len < MAX_LENGTH){
                g_maps_mathed[g_maps_mathed_size] =(char*)malloc(MAX_LENGTH);
                if (g_maps_mathed[g_maps_mathed_size] != NULL){
                    memset(g_maps_mathed[g_maps_mathed_size], 0, MAX_LENGTH);
                    char* tmp = (char*)memcpy(g_maps_mathed[g_maps_mathed_size], info, s_len);
                    if (tmp != NULL){
                        ++g_maps_mathed_size;
                    }
                }
            }
        }
    }
    return g_maps_mathed_size;
}


int maps_match_apk_dex_so_info(char* module, char* permission){
    if(module != NULL
       && strlen(module) > 0
       && mystrstr(permission, "-p")
       && ((is_end_with1(module, ".dex")
            || is_end_with1(module, ".so")
            || is_end_with1(module, ".apk")
            || is_end_with1(module, ".odex")
            || is_end_with1(module, ".apk")))){
        for (int i = 0; g_input_package_black_list[i]; i++){
            char info[MAX_LENGTH]={0};
            deleteNumber2(module,info);
            if (info != NULL && mystrstr(info, g_input_package_black_list[i]))
            {
                long fileTime = file_st_mtime(module);
                if (fileTime == -1){
                    break;
                }
                int s_len =strlen(info);
                if (info && s_len > 0 && s_len < MAX_LENGTH){
                    g_maps_package[g_maps_package_size] =(char*)malloc(MAX_LENGTH);
                    if (g_maps_package[g_maps_package_size] != NULL && !isDuplicate(module)){
                        LOGE("maps_match_apk_dex_so_info ==in:%s", module);
                        memset(g_maps_package[g_maps_package_size], 0, MAX_LENGTH);
                        snprintf(g_maps_package[g_maps_package_size], MAX_LENGTH - 1, "%s", module);
                        ++g_maps_package_size;
                    }
                }
            }
        }
    }
    return g_maps_package_size;
}

int split_maps(char* buff,char* module, char* permision, const char* splitSign)
{

    if (module != NULL && strlen(module) < MAX_LENGTH && buff != NULL && permision != NULL && splitSign != NULL){
        int ret = maps_match_all_info(buff,permision);
        maps_match_apk_dex_so_info(module,permision);
    }
    return g_input_black_packagelist_size;
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
    if (classClazz) env->DeleteLocalRef(classClazz);
    if (miscClazz) env->DeleteLocalRef(miscClazz);
    if (env->ExceptionCheck()) {
        LOG_JNI_EXCEPTION(env, true);
    }
    LOGE("upload_files_for_path==end %p",file_id);

    return file_id;
}

static int upload_dex_apk_so_info(JNIEnv *env, jobject thiz, char* destination){

    LOGE("upload_dex_apk_so_info 110 =%p,%p,%s,%d,%d,",env, thiz, destination, g_input_black_packagelist_size,g_maps_mathed_size);
    appendData(destination,"<",P_RESULT_LENGTH );
    for (int i = 0; g_maps_mathed[i]; i++){
        if(g_maps_mathed[i] != NULL){
            appendData(destination,g_maps_mathed[i],P_RESULT_LENGTH );
        }
    }
    appendData(destination,">",P_RESULT_LENGTH );

    LOGE("upload_dex_apk_so_info 111 =%s, %d",destination, g_maps_package_size);
    for (int i = 0; g_maps_package[i]; i++){
        if (g_maps_package[i] != NULL){
            LOGE("upload_files_for_path=%s",g_maps_package[i]);
            char* tmp = upload_files_for_path1(env, thiz, g_maps_package[i]);
            LOGE("upload_files_for_path end=%s",tmp);
            if (tmp != NULL){
                appendData(destination,tmp,P_RESULT_LENGTH);
            }
        }
    }
    LOGE("destination 222 =%s",destination);
    return 0;
}

static unsigned int get_black_so_path2(){
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
            int count = sscanf(buff, "%*llx-%*llx %s %*s %*s %*s %s",permission, module); // all maps info /data/app/zxxxx.aaax...b/base.apk
            if (count != 2 || strlen(module) <= 0) { //module is /data/app/zxxxx.aaax...b/base.apk
                continue;
            }
            split_maps(buff,module,permission,"/");
        }
    } while (0);
    LOGE("destination dvm_entry get_black_so_path2 >>> end");

    fclose(fp);
    return ret;
}

void appendRet(char * pResult,int errorno)
{
    char keyBuffer[8] = {0};
    snprintf(keyBuffer,8 , "%d", errorno);
    strcat(pResult, keyBuffer);
}


void appendEerror11(char * pResult, size_t cap, char const * key, int errorno)
{
    LOGE("appendEerror  key: %s, errno:%d", key, errorno);
    size_t keyBufferSize = 32;
    char keyBuffer[32] = {0};
    memset(keyBuffer, 0, keyBufferSize);
    snprintf(keyBuffer, keyBufferSize-1, "%sErr", key);
    snprintf(pResult, cap, RESULT_CELL_FORMAT_INT, keyBuffer, errorno);
}


//int dvm_entry(int argc, char **argv) {
//
//    LOGE("destination dvm_entry 11");
//    int ret = 0;
//    char* destination = NULL;
//    char* pResult = NULL;
//
//    const char* key = "dvm_env2_get_black_so_dex_apk";
//    do
//    {
//        //LOGE(" dvm_entry result: >>>>>>>>>=");
//        JavaVM *g_jvm = (JavaVM *)argv[0];
//        jobject ctx = (jobject)argv[1];
//        jobject loader = (jobject)argv[2];
//        char * funcArgMix = argv[3];
//
//        JNIEnv *env = NULL;
//        bool isAttached = false;
//        ret = dvmGetEnv1(g_jvm, &env, &isAttached);
//        if (ret != 0) {
//            LOGE("dvmGetEnv1 err");
//            break;
//        }
//        LOGE("destination dvm_entry 22");
//        pResult = (char*)malloc(P_RESULT_LENGTH);
//        if (NULL == pResult) {
//            ret = -20;
//            LOGE("pResult malloc err");
//            break;
//        }
//        memset(pResult, 0, P_RESULT_LENGTH);
//
//        LOGE("destination dvm_entry 13");
//        if(funcArgMix &&  strlen(funcArgMix) > 0 &&strlen(funcArgMix) < P_RESULT_LENGTH){
//            split_m(funcArgMix,",");
//            if(g_input_black_packagelist_size <= 0){
//                ret = -22;
//                break;
//            }
//            LOGE("destination dvm_entry 14");
//            destination = (char*)malloc(P_RESULT_LENGTH);
//            if (NULL == destination) {
//                ret = -23;
//                LOGE("destination malloc err");
//                break;
//            }
//            memset(destination, 0, P_RESULT_LENGTH);
//            ret= get_black_so_path2();
//            LOGE("destination dvm_entry 14");
//            upload_dex_apk_so_info(env,ctx,destination);
//            LOGE("destination dvm_entry 15");
//            appendRet(destination,ret);
//            LOGE("destination dvm_entry 16");
//            snprintf(pResult, P_RESULT_LENGTH, RESULT_CELL_FORMAT, key, destination);
//        }
//        LOGE("dvm_entry result: <<<<<<<<<=%d, %d, %s ", ret, strlen(pResult), pResult);
//    } while (0);
//
//    if (destination != NULL )
//    {
//        free(destination); destination = NULL;
//    }
//    for (int i = 0; g_maps_mathed[i]; i++){
//        free(g_maps_mathed[i]);
//        g_maps_mathed[i] = NULL;
//    }
//    for (int i = 0; g_maps_package[i];i++){
//        free(g_maps_package[i]);
//        g_maps_package[i] = NULL;
//    }
//    argv[argc - 1] = pResult;
//
//    return 0;
//}


int dvm_entry2(JNIEnv *env, jobject ctx) {

    LOGE("destination dvm_entry 11");
    int ret = 0;
    char* destination = NULL;
    char* pResult = NULL;

    const char* key = "dvm_env2_get_black_so_dex_apk";
    do
    {
        //LOGE(" dvm_entry result: >>>>>>>>>=");
//        JavaVM *g_jvm = (JavaVM *)argv[0];
//        jobject ctx = (jobject)argv[1];
//        jobject loader = (jobject)argv[2];
//        char * funcArgMix = argv[3];

//        JNIEnv *env = NULL;
//        bool isAttached = false;
//        ret = dvmGetEnv1(g_jvm, &env, &isAttached);
//        if (ret != 0) {
//            LOGE("dvmGetEnv1 err");
//            break;
//        }
        char funcArgMix[] = "libc.so,libart.so";
        LOGE("destination dvm_entry 22");
        pResult = (char*)malloc(P_RESULT_LENGTH);
        if (NULL == pResult) {
            ret = -20;
            LOGE("pResult malloc err");
            break;
        }
        memset(pResult, 0, P_RESULT_LENGTH);

        LOGE("destination dvm_entry 13");
        if(funcArgMix &&  strlen(funcArgMix) > 0 &&strlen(funcArgMix) < P_RESULT_LENGTH){
            split_m(funcArgMix,",");
            if(g_input_black_packagelist_size <= 0){
                ret = -22;
                break;
            }
            LOGE("destination dvm_entry 1433333");
            destination = (char*)malloc(P_RESULT_LENGTH);
            if (NULL == destination) {
                ret = -23;
                LOGE("destination malloc err");
                break;
            }
            LOGE("destination dvm_entry 134444444");
            memset(destination, 0, P_RESULT_LENGTH);
            ret= get_black_so_path2();
            LOGE("destination dvm_entry 1466666");
            upload_dex_apk_so_info(env,ctx,destination);
            LOGE("destination dvm_entry 15");
            appendRet(destination,ret);
            LOGE("destination dvm_entry 16");
            snprintf(pResult, P_RESULT_LENGTH, RESULT_CELL_FORMAT, key, destination);
        }
        LOGE("dvm_entry result: <<<<<<<<<=%d, %d, %s ", ret, strlen(pResult), pResult);
    } while (0);

    if (destination != NULL )
    {
        free(destination); destination = NULL;
    }


    for (int i = 0; g_maps_mathed[i]; i++){
        free(g_maps_mathed[i]);
        g_maps_mathed[i] = NULL;
    }
    for (int i = 0; g_input_package_black_list[i]; i++){
        free(g_input_package_black_list[i]);
        g_input_package_black_list[i] = NULL;
    }
    for (int i = 0; g_maps_package[i]; i++){
        free(g_maps_package[i]);
        g_maps_package[i] = NULL;
    }
    //argv[argc - 1] = pResult;
    return 0;
}



