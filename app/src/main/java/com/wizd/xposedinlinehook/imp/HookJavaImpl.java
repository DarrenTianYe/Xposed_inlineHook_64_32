package com.wizd.xposedinlinehook.imp;

import android.content.Context;
import java.io.FileOutputStream;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;

public class HookJavaImpl{

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
