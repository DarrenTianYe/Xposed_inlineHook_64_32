package com.system.installer.acc.impl;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.util.Log;

public class ServiceManagerWraper {

    public final static String TAG = "SharkDarren";

    public static void hookPMS(Context context, String signed, String appPkgName, int hashCode) {
        try {
            // 获取全局的ActivityThread对象
            Class<?> activityThreadClass = Class.forName("android.app.ActivityThread");
            Method currentActivityThreadMethod =
                    activityThreadClass.getDeclaredMethod("currentActivityThread");
            Object currentActivityThread = currentActivityThreadMethod.invoke(null);
            // 获取ActivityThread里面原始的sPackageManager
            Field sPackageManagerField = activityThreadClass.getDeclaredField("sPackageManager");
            sPackageManagerField.setAccessible(true);
            Object sPackageManager = sPackageManagerField.get(currentActivityThread);
            // 准备好代理对象, 用来替换原始的对象
            Class<?> iPackageManagerInterface = Class.forName("android.content.pm.IPackageManager");
            Object proxy = Proxy.newProxyInstance(
                    iPackageManagerInterface.getClassLoader(),
                    new Class<?>[]{iPackageManagerInterface},
                    new PmsBinderHandler(sPackageManager, signed, appPkgName, 0));
            // 1. 替换掉ActivityThread里面的 sPackageManager 字段
            sPackageManagerField.set(currentActivityThread, proxy);
            // 2. 替换 ApplicationPackageManager里面的 mPM对象
            PackageManager pm = context.getPackageManager();
            Field mPmField = pm.getClass().getDeclaredField("mPM");
            mPmField.setAccessible(true);
            mPmField.set(pm, proxy);

           // getAppInfogetPackagesForUid(context);
        } catch (Exception e) {
            Log.d(TAG, "hook pms error:" + Log.getStackTraceString(e));
        }
    }

    public static void hookPMS(Context context) {
        String qqSign = "30820253308201bca00302010202044bbb0361300d06092a864886f70d0101050500306d310e300c060355040613054368696e61310f300d06035504080c06e58c97e4baac310f300d06035504070c06e58c97e4baac310f300d060355040a0c06e885bee8aeaf311b3019060355040b0c12e697a0e7babfe4b89ae58aa1e7b3bbe7bb9f310b30090603550403130251513020170d3130303430363039343831375a180f32323834303132303039343831375a306d310e300c060355040613054368696e61310f300d06035504080c06e58c97e4baac310f300d06035504070c06e58c97e4baac310f300d060355040a0c06e885bee8aeaf311b3019060355040b0c12e697a0e7babfe4b89ae58aa1e7b3bbe7bb9f310b300906035504031302515130819f300d06092a864886f70d010101050003818d0030818902818100a15e9756216f694c5915e0b529095254367c4e64faeff07ae13488d946615a58ddc31a415f717d019edc6d30b9603d3e2a7b3de0ab7e0cf52dfee39373bc472fa997027d798d59f81d525a69ecf156e885fd1e2790924386b2230cc90e3b7adc95603ddcf4c40bdc72f22db0f216a99c371d3bf89cba6578c60699e8a0d536950203010001300d06092a864886f70d01010505000381810094a9b80e80691645dd42d6611775a855f71bcd4d77cb60a8e29404035a5e00b21bcc5d4a562482126bd91b6b0e50709377ceb9ef8c2efd12cc8b16afd9a159f350bb270b14204ff065d843832720702e28b41491fbc3a205f5f2f42526d67f17614d8a974de6487b2c866efede3b4e49a0f916baa3c1336fd2ee1b1629652049";
        hookPMS(context, qqSign, "com.shark.hookpms", 0);
    }

    public static String getAppInfogetPackagesForUid(Context context){

        PackageManager packageManager = context.getPackageManager();
        ArrayList<String> appInfos = new ArrayList<>();

        for (int i = 0; i< 20000; i++){
            String[] installedPackages = packageManager.getPackagesForUid(i);
            if (installedPackages != null && installedPackages.length > 0){
                List<String> striList = Arrays.asList(installedPackages);
                Log.e(TAG, ":getAppInfogetPackagesForUid::+"+striList.toString()+"\n");
                appInfos.addAll(striList);
            }
        }

        ArrayList<String> appInfosLast = new ArrayList<>();
        for (int i = 0; i < appInfos.size();i++){
            try {
                ApplicationInfo appLicaton = packageManager.getApplicationInfo(appInfos.get(i), 0);
                if (appInfos != null){
                    CharSequence v7 = packageManager.getApplicationLabel(packageManager.getApplicationInfo(appLicaton.packageName, PackageManager.GET_META_DATA));
                    appInfosLast.add("<"+appLicaton.packageName+appLicaton.name+v7.toString()+">\n"+appLicaton.sourceDir+appLicaton.packageName+"\n");
                    Log.e(TAG, ":getApplicationInfo::+"+appLicaton.toString());
                }
            } catch (Throwable e) {
                e.printStackTrace();
            }
        }
        return appInfosLast.toString();
    }
}