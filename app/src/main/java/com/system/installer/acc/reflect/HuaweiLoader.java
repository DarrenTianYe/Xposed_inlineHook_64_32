package com.system.installer.acc.reflect;

import android.app.Application;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.util.Log;

import com.system.installer.acc.system.MainLogic;

import java.io.File;

import dalvik.system.PathClassLoader;
import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.IXposedHookZygoteInit;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class HuaweiLoader implements IXposedHookLoadPackage, IXposedHookZygoteInit {

    public final static String TAG = "antiyDarren";
    private final static String modulePackageName = MainLogic.class.getPackage().getName();
    private final String handleHookClass = MainLogic.class.getName();
    private final String testMethod = "aaa";


    @Override
    public void handleLoadPackage(final XC_LoadPackage.LoadPackageParam loadPackageParam) {

        Log.e("darren_haha", "darren.handleLoadPackage.attach"+ loadPackageParam.packageName);
        XposedBridge.log(TAG + "start hook"+loadPackageParam.packageName);
        XposedHelpers.findAndHookMethod(Application.class, "attach", Context.class, new XC_MethodHook() {
            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                Context context = (Context) param.args[0];

                Log.e("darren_haha", "darren.handleLoadPackage.attach"+ loadPackageParam.packageName);

               XposedBridge.log(TAG + "检测到 " + this.getClass().getPackage().getName() + " 安装，正在免重启加载模块。 ");
              //  Context context = AndroidAppHelper.currentApplication().getApplicationContext();
                Class<?> cls = getApkClass(context, handleHookClass);
                Object instance = cls.newInstance();
                PackageManager packageManager = context.getPackageManager();
                PackageInfo packageInfo = packageManager.getPackageInfo("com.wizd.xposedinlinehook", 0);

                cls.getDeclaredMethod(testMethod, loadPackageParam.getClass(), int.class, Context.class)
                        .invoke(instance, loadPackageParam, packageInfo.versionCode, context);

                XposedBridge.log(TAG + "==== 免重启加载完成 =================");
            }
        });
    }


    /**
     * @param context
     * @param handleHookClass
     * @return HookTest.class
     * @throws Throwable
     */
    private Class<?> getApkClass(Context context, String handleHookClass) throws Throwable {
        File apkFile = null;
        if (context == null) {
            return null;
        }
        try {
            Context moudleContext = context.createPackageContext(HuaweiLoader.modulePackageName, Context.CONTEXT_INCLUDE_CODE | Context.CONTEXT_IGNORE_SECURITY);
            String apkPath = moudleContext.getPackageCodePath();
            apkFile = new File(apkPath);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }

        PathClassLoader pathClassLoader = new PathClassLoader(apkFile.getAbsolutePath(), XposedBridge.BOOTCLASSLOADER);
        return Class.forName(handleHookClass, true, pathClassLoader);
    }

    @Override
    public void initZygote(StartupParam startupParam) throws Throwable {

    }
}