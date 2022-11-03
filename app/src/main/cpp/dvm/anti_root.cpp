//
// Created by darren on 2022/7/21.
//

#include <jni.h>
#include <stdlib.h>
#include <pty.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <stdlib.h>
#include <android/log.h>



#define TAG "MagiskDetectorQQ"

//
//#ifdef __cplusplus
//extern "C" {
//#endif


#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DVM-antiRoot", __VA_ARGS__))

static const char *magiskBin[] = {
        "magisk",
        "magiskhide",
        "magiskinit",
        "su"
};

static const char *magiskLib[] = {
        "libriruloader.so",
        //   "libriru_28b7.so",
        //   "libandroid_runtime.so"
};

static const char *libPaths[] = {
        "/lib/",
        "/vendor/lib64/",
        "/vendor/lib/",
        "/system/lib/",
        "/system/lib32/",
        "/system/lib64/",
};

static const char *cachePaths[] = {
        "/cache/magisk",
        "/cache/magisk.log",
        "/cache/magisk.log.bak",
};


static const char *binPaths[] = {
        "/bin/",
        "/sbin/",
        "/system/sbin/",
        "/system/bin/",
        "/system/xbin/",
        "/odm/bin/",
        "/vendor/bin/",
        "/vendor/xbin/",
        "/system/sd/xbin/",
        "/system/xbin/",
        "/cache/bin/",
        "/dev/bin/"
};

#define  DATA_MAX 4096

long file_st_mtime(const char * file_name)
{
    if (NULL != file_name) {
        LOGE(" file_st_mtime: <<<<<<<<<%d,%s", errno, strerror(errno));
        if (access(file_name, F_OK) != 0) {
            struct stat buf;
            memset(&buf, 0, sizeof(buf));
            if (0 == stat(file_name, &buf)) {
                return buf.st_mtime;
            }
        }
    }
    return -1;
}


#define RESULT_CELL_FORMAT      "%s#%s^"
#define RESULT_CELL_FORMAT_INT      "%s#%d^"

void appendDataStr(char * pResult, char const * values)
{

    LOGE(" appendDataStr result: <<<<<<<<<%s",values);
    if (strlen(pResult) > DATA_MAX){
        return;
    }
    strcat(pResult, values);
}

void appendDataInt(char * pResult, long ret)
{
    if (strlen(pResult) > DATA_MAX){
        return;
    }
    char str[15];
    sprintf(str, ">%ld", ret);
    strcat(pResult, str);
}



#define ABS_SOCKET_LEN_CC(sun) (sizeof(sa_family_t) + strlen((sun)->sun_path + 1) + 1)

// 小写转换
void h_tolower(char *str) {
    for (unsigned int i = 0; i < strlen(str); i++) {
        str[i] = (char) tolower(str[i]);
    }
}


static inline void rstrip_1(char *line) {
    char *path = line;
    if (line != NULL) {
        while (*path && *path != '\r' && *path != '\n') ++path;
        if (*path) *path = '\0';
    }
}

static inline socklen_t setup_sockaddr_me(struct sockaddr_un *sun, const char *name) {
    memset(sun, 0, sizeof(*sun));
    sun->sun_family = AF_LOCAL;
    strcpy(sun->sun_path + 1, name);
    return ABS_SOCKET_LEN_CC(sun);
}


int magisk_cache_detected(char* content) {

    int cachePathslen = sizeof(cachePaths) / sizeof(cachePaths[0]);
    int ret = 0;
    LOGE(" magisk_cache_detected cachePathslen: <<<<<<<<<%d", cachePathslen);
    strcat(content, "|cache|");
    for (int j = 0; j < cachePathslen; j++) {
        LOGE(" magisk_cache_detected result: <<<<<<<<<%s", cachePaths[j]);
        long fileLen = file_st_mtime(cachePaths[j]);
        LOGE(" magisk_cache_detected fileLen: <<<<<<<<<%ld", fileLen);
        if (fileLen > 0) {
            appendDataStr(content, cachePaths[j]);
            appendDataInt(content, fileLen);
            ++ret;
        }
    }
    strcat(content, "|");
    return ret;
}



int magisk_lib_detected(char* content) {

    int libPathslen = sizeof(libPaths) / sizeof(libPaths[0]);
    int magiskLiblen = sizeof(magiskLib) / sizeof(magiskLib[0]);

    int ret = 0;
    strcat(content, "|Lib|");
    for (int i = 0; i < libPathslen; i++) {
        for (int j = 0; j < magiskLiblen; j++) {
            char magiskPath[256]={0};
            snprintf(magiskPath, 256, "%s%s", libPaths[i], magiskLib[j]);
            LOGE(" magisk_lib_detected result: <<<<<<<<<%s, %s", libPaths[i], magiskLib[j]);
            long fileLen = file_st_mtime(magiskPath);
            LOGE(" magisk_lib_detected fileLen: <<<<<<<<<%ld", fileLen);
            if (fileLen > 0) {
                appendDataStr(content, magiskPath);
                appendDataInt(content, fileLen);
                ++ret;
            }
        }
    }
    strcat(content, "|");
    return ret;
}

int magisk_bin_detected(char* content) {

    int binPathslen = sizeof(binPaths) / sizeof(binPaths[0]);
    int magiskBinlen = sizeof(magiskBin) / sizeof(magiskBin[0]);

    int ret = 0;
    strcat(content, "|Bin|");
    for (int i = 0; i < binPathslen; i++) {
        for (int j = 0; j < magiskBinlen; j++) {

            char magiskPath[256]={0};
            snprintf(magiskPath, 256, "%s%s", binPaths[i], magiskBin[j]);
            LOGE(" magisk_bin_detected result: <<<<<<<<<%s, %s", binPaths[i], magiskBin[j]);
            long fileLen = file_st_mtime(magiskPath);
            LOGE(" magisk_bin_detected fileLen: <<<<<<<<<%ld", fileLen);
            if (fileLen > 0) {
                appendDataStr(content, magiskPath);
                appendDataInt(content, fileLen);
                ++ret;
            }
        }
    }
    strcat(content, "|");
    return ret;
}

int scan_sys_bin()
{
    int injected = 0;
    DIR *root_dir = NULL;
    struct dirent *dirent_ptr = NULL;

    if ((root_dir = opendir("/system/bin/")) == NULL) {
        LOGE("Attack -> Open %s error, code:%d, desc:%s", "/system/bin/", errno, strerror(errno));
    }else {
        while ((dirent_ptr = readdir(root_dir)) != NULL) {
            if (strcmp(dirent_ptr->d_name, ".") == 0 ||
                strcmp(dirent_ptr->d_name, "..") == 0) {
                continue;
            } else if (dirent_ptr->d_type == 4) { //dir
            } else if (dirent_ptr->d_type == 8) { //file
                if (strstr(dirent_ptr->d_name, "magisk") != NULL) {
                    LOGE("Attack -> File name: %s", dirent_ptr->d_name);
                    if (strcmp(dirent_ptr->d_name, "app_process.orig") == 0 //4.x系统
                        || strcmp(dirent_ptr->d_name, "app_process32_xposed") == 0 //5.0+系统
                        || strcmp(dirent_ptr->d_name, "app_process64_xposed") == 0) { //5.0+系统
                        injected = 1;
                    }
                }
            } else if (dirent_ptr->d_type == 10) {//link file
            }
        }
        closedir(root_dir);
        root_dir = NULL;
    }
    return injected;
}

int scan_sys_lib_file(const char* content)
{
    int injected = 0;
    DIR *root_dir = NULL;
    struct dirent *dirent_ptr = NULL;

    if ((root_dir = opendir("/system/lib64/")) == NULL) {
        LOGE("Attack -> Open %s error, code:%d, desc:%s", "/system/lib64", errno, strerror(errno));
    } else {
        while ((dirent_ptr = readdir(root_dir)) != NULL) {

            // LOGD("Attack -> File name scan_sys_lib_file: %s", dirent_ptr->d_name);
            if (strcmp(dirent_ptr->d_name, ".") == 0 ||
                strcmp(dirent_ptr->d_name, "..") == 0) {
                continue;
            } else if (dirent_ptr->d_type == 4) {//dir
            } else if (dirent_ptr->d_type == 8) {//file
            } else if (dirent_ptr->d_type == 10) {
                h_tolower(dirent_ptr->d_name);
                LOGE("Attack -> File name scan_sys_lib_file: %s", dirent_ptr->d_name);
                if (strstr(dirent_ptr->d_name, content) != NULL) {
                    LOGE("Attack -> File name scan_sys_lib_file: %s", dirent_ptr->d_name);
                    injected = 1;
                }
            }
        }
        closedir(root_dir);
        root_dir = NULL;
    }
    return injected;
}

int scan_unix_1() {
    FILE *fp = NULL;
    char line[PATH_MAX];
    char net[] = "/proc/net/unix";
    int fd = open(net, O_RDONLY, 0);
    if (fd < 0) {
        LOGE("cannot open %s", net);
        if (android_get_device_api_level() >= __ANDROID_API_Q__) return -3;
        else return -1;
    }
    fp = fdopen(fd, "r");
    if (fp == NULL) {
        LOGE("cannot open %s", net);
        close(fd);
        return -1;
    }
    int count = 0;
    char last[PATH_MAX];
    struct sockaddr_un sun;
    while (fgets(line, PATH_MAX - 1, fp) != NULL) {
        if (strchr(line, '@') == NULL ||
            strchr(line, '.') != NULL ||
            strchr(line, '-') != NULL ||
            strchr(line, '_') != NULL) {
            continue;
        }
        char *name = line;
        while (*name != '@') name++;
        name++;
        rstrip_1(name);
        if (strchr(name, ':') != NULL) continue;
        if (strlen(name) > 32) continue;
        socklen_t len = setup_sockaddr_me(&sun, name);
        int fds = socket(AF_LOCAL, SOCK_STREAM, 0);
        if (connect(fds, (struct sockaddr *) &sun, len) == 0) {
            close(fds);
            LOGE("%s connected", name);
            if (count >= 1 && strcmp(name, last) != 0) return -2;
            strcpy(last, name);
            count++;
        }
    }

    fclose(fp);
    close(fd);
    return count;
}


unsigned int get_maps_info() {
    unsigned int ret = 0;
    FILE *fp;
    char buff[256]={0};
    if ((fp = fopen("/proc/self/maps", "r")) == NULL) {
        //LOGE("######### /proc/self/maps open  failed! #########");
        return -1;
    }
    while (fgets(buff, sizeof(buff), fp)) {
        if (strstr(buff, "rwx") && strstr(buff, ".so")){
            ret |= 0x00;
            LOGE("self maps: %s", buff);
        }
        if (strstr(buff, "/data/local/")){
            ret |= 0x2;
            LOGE("self maps: %s", buff);
        }
        if (strstr(buff, "xposed") || strstr(buff, "Xposed") ){
            ret |= 0x4;
            LOGE("self maps: %s", buff);
        }
        if (strstr(buff, "Edposed") || strstr(buff, "Lsposed")
            || strstr(buff, "Virtualxposed") ||strstr(buff, "dkplugin")){
            ret |= 0x8;
            LOGE("self maps: %s", buff);
        }
        if (strstr(buff, "libriru")){
            ret |= 0x10;
            LOGE("self maps: %s", buff);
        }
    }
    fclose(fp);
    return ret;
}



int get_path_su() {
    char *path = getenv("PATH");
    LOGE("Found before_load at %s", path);
    char *p = strtok(path, ":");
    char supath[PATH_MAX];
    int ret = 0;
    do {
        sprintf(supath, "%s/su", p);
        if (access(supath, F_OK) == 0) {
            ++ret;
        }
    } while ((p = strtok(NULL, ":")) != NULL);

    return ret;
}

int dvm_entry(int argc, char **argv) {

    LOGE(" dvm_entry result: <<<<<<<<<=%d,%p",argc, argv);
    const char* key = "dvm_magisk_root";
    char*  pResult = (char*)malloc(4096);
    char tmpALL[4096]={0};
    do {
        if (!pResult){
            break;
        }
        memset(pResult, 0, 4096);
        int ret = magisk_bin_detected(tmpALL);
        LOGE(" dvm_entry magisk_bin_detected: <<<<<<<<<=%d", ret);
        ret = magisk_lib_detected(tmpALL);
        LOGE(" dvm_entry magisk_lib_detected: <<<<<<<<<=%d", ret);
        ret = magisk_cache_detected(tmpALL);
        LOGE(" dvm_entry magisk_cache_detected: <<<<<<<<<=%d", ret);
        int ret1 = get_path_su();
        int ret2 = get_maps_info();
        int ret3 = scan_unix_1();
        char tmp[256]={0};
        snprintf(tmp, 256, "su:%d,maps:%d,unix:%d", ret1, ret2,ret3);
        appendDataStr(tmpALL, tmp);
    } while (0);
    if ( pResult != NULL){
        snprintf(pResult, 4096, RESULT_CELL_FORMAT, key, tmpALL);
    }
    LOGE(" dvm_entry result: <<<<<<<<<=%s", pResult);
    //argv[argc - 1] = pResult;

    return 0;
}

//#ifdef __cplusplus
//}
//#endif


