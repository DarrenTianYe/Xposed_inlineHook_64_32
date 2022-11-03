package com.system.installer.acc.system;

import android.content.Context;
import android.util.Log;

import com.system.installer.acc.impl.ServiceManagerWraper;
import com.system.installer.acc.location.GPSHook;
import com.system.installer.acc.utils.Utils;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class MainLogic {
    public final static String TAG = "MainLogicDarren";
    public static int checkVersionCode = 0;

    public static  volatile Context g_appContext = null;

    public void aaa(XC_LoadPackage.LoadPackageParam loadPackageParam, int checkNum, Context context) throws Throwable {
        checkVersionCode = checkNum;
        LoadPackage(loadPackageParam, context);
    }

    public static void LoadPackage(XC_LoadPackage.LoadPackageParam loadPackageParam, Context context) throws Throwable {

        XposedBridge.log(TAG + "on " + loadPackageParam.packageName + " handleLoadPackage. The current version: " + checkVersionCode);

        if (!loadPackageParam.packageName.equals("com.eg.android.AlipayGphone")){
            return;
        }
        GPSHook.handleLoadPackageGPs(loadPackageParam);

        /**
         * //must be todo
         */
        //cp /data/data/com.wizd.xposedinlinehook/lib/libnative-lib.so /data/data/com.eg.android.AlipayGphone/

       g_appContext = context;
        if (Utils.getProcessName(g_appContext).equals("com.eg.android.AlipayGphone")){

            Log.e("darren_haha", "darren.hookPMS.start"+ g_appContext.getClassLoader());
            ServiceManagerWraper.hookPMS(g_appContext);
            Log.e("darren_haha", "darren.hookPMS.end"+ g_appContext.getClassLoader());

        }
    }
}