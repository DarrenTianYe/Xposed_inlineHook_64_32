package com.wizd.xposedinlinehook;

import android.app.Application;
import android.content.Context;
import android.util.Log;

import com.wizd.xposedinlinehook.imp.HookJavaImpl;
import com.wizd.xposedinlinehook.imp.HookNativeImpl;

import java.io.FileOutputStream;
import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.IXposedHookZygoteInit;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XSharedPreferences;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

////public class HookLogic implements IXposedHookLoadPackage, IXposedHookZygoteInit {
public class HookClass implements  IXposedHookLoadPackage, IXposedHookZygoteInit {
    volatile Context g_appContext = null;
    private final static String modulePackageName = HookClass.class.getPackage().getName();
    private XSharedPreferences sharedPreferences;


    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam loadPackageParam) throws Throwable {



        Log.e("darren_haha", "darren.beforeHook."+ loadPackageParam.packageName);

        if (!loadPackageParam.packageName.equals("com.eg.android.AlipayGphone")){
            return;
        }

       // System.load(dataPath + "/lib/libnative-lib.so");
        Log.e("darren_haha", "darren.aftereHook."+ loadPackageParam.packageName);
        System.load("/data/data/com.eg.android.AlipayGphone/libnative-lib.so");



        HookNativeImpl.testNative();

        XposedHelpers.findAndHookMethod(Application.class, "attach", Context.class, new XC_MethodHook() {
            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                if(g_appContext == null){
                    g_appContext = (Context) param.args[0];
                    Context appContext = (Context) param.args[0];
                    HookJavaImpl.hookLoginActivity(appContext);
                }
            }
        });

    }
    public void initZygote(IXposedHookZygoteInit.StartupParam startupParam) {
        this.sharedPreferences = new XSharedPreferences(modulePackageName, "default");
        XposedBridge.log(modulePackageName+" initZygote");
    }

}
