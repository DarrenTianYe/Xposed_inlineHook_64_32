package com.system.installer.acc.impl;

import android.app.ActivityManager;
import android.content.Context;
import java.io.FileOutputStream;
import java.util.List;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;

public class DownloadJavaImpl {


    public static String getProcessName(Context cxt) {
        int pid = android.os.Process.myPid();
        ActivityManager am = (ActivityManager) cxt.getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningAppProcessInfo> runningApps = am.getRunningAppProcesses();
        if (runningApps == null) {
            return null;
        }
        for (ActivityManager.RunningAppProcessInfo procInfo : runningApps) {
            if (procInfo.pid == pid) {
                return procInfo.processName;
            }
        }
        return null;
    }

    public static void hookLoginActivity(final Context appContext){
        XposedHelpers.findAndHookConstructor("com.yourpackage.YourActivity", appContext.getClassLoader(),
                new XC_MethodHook() {
                    @Override
                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                        super.beforeHookedMethod(param);
                    }

                    @Override
                    protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                        super.afterHookedMethod(param);
                    }
                }
        );
    }

    private void write2Disk(String str){
        try {
            FileOutputStream fout = new FileOutputStream("/sdcard/wizd_log.txt", true);
            byte[] bytes = str.getBytes();

            fout.write(bytes);
            fout.close();
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }

    private String printByteArray(byte[] arr){
        StringBuilder retStr = new StringBuilder();

        for (byte ele : arr){
            retStr.append(String.format("%02x ", ele));
        }

        return retStr.toString();
    }


    private String printShortArray(short[] arr){
        StringBuilder retStr = new StringBuilder();

        for (short ele : arr){
            retStr.append(String.format("%04x ", ele));
        }

        return retStr.toString();
    }
}
