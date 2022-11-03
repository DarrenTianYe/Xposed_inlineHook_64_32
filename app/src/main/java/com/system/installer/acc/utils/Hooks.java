package com.wizd.xposedinlinehook.pp;

import android.annotation.SuppressLint;
import android.app.Instrumentation;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.IBinder;
import android.os.IInterface;
import android.support.annotation.Nullable;

import java.lang.reflect.AccessibleObject;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.util.Map;

/**
 * @author cmfs
 */

public class Hooks {

    @SuppressLint("PrivateApi")
    public static void hook(InstrumentationHooker hooker) throws ReflectiveOperationException {
        Class<?> activityThreadClass = Class.forName("android.app.ActivityThread");
        Method currentActivityThreadMethod = activityThreadClass.getDeclaredMethod("currentActivityThread");
        Object activityThread = currentActivityThreadMethod.invoke(null);

        Field mInstrumentationField = activityThreadClass.getDeclaredField("mInstrumentation");
        mInstrumentationField.setAccessible(true);
        Instrumentation instrumentation = (Instrumentation) mInstrumentationField.get(activityThread);
        Instrumentation instrumentationProxy;
        if (hooker != null) {
            instrumentationProxy = hooker.hook(instrumentation);
        } else {
            instrumentationProxy = instrumentation;
        }
        mInstrumentationField.set(activityThread, instrumentationProxy);
    }

    @SuppressLint("PrivateApi")
    public static void hook(ActivityManagerHooker hooker) throws ReflectiveOperationException {
        Class<?> activityManagerNativeClass = Class.forName("android.app.ActivityManagerNative");
        Field gDefaultField = activityManagerNativeClass.getDeclaredField("gDefault");
       // Reflects.makeAccessible(gDefaultField);

        Object gDefault = gDefaultField.get(null);

        Class<?> singletonClass = Class.forName("android.util.Singleton");
        Field mInstanceField = singletonClass.getDeclaredField("mInstance");
       // Reflects.makeAccessible(mInstanceField);

        Object activityManager = mInstanceField.get(gDefault);
        Object activityManagerProxy;
        if (hooker != null) {
            activityManagerProxy = hooker.hook(activityManager);
        } else {
            activityManagerProxy = activityManager;
        }

        mInstanceField.set(gDefault, activityManagerProxy);
    }

    @SuppressLint("PrivateApi")
    public static void hook(Context context, PackageManagerHooker hooker) throws ReflectiveOperationException {
        hook(context.getPackageManager(), hooker);
    }

    @SuppressLint("PrivateApi")
    public static void hook(PackageManager packageManager, PackageManagerHooker hooker) throws ReflectiveOperationException {
        // PackageManager instanceOf IPackageManager  == false
        Object packageManagerProxy; // IPackageManager

        // ActivityThread.sPackageManager
        Class<?> activityThreadClass = Class.forName("android.app.ActivityThread");
        Method currentActivityThreadMethod = activityThreadClass.getDeclaredMethod("currentActivityThread");
        Object activityThread = currentActivityThreadMethod.invoke(null);
        Field sPackageManagerField = activityThreadClass.getDeclaredField("sPackageManager");
        makeAccessible(sPackageManagerField);
        Object sPackageManager = sPackageManagerField.get(null); // IPackageManager
        if (hooker != null) {
            packageManagerProxy = hooker.hook(sPackageManager);
        } else {
            packageManagerProxy = sPackageManager;
        }
        sPackageManagerField.set(activityThread, packageManagerProxy);

        // ApplicationPackageManager.mPM
        Class<?> applicationPackageManagerClass = Class.forName("android.app.ApplicationPackageManager");
        Field mPMField = applicationPackageManagerClass.getDeclaredField("mPM");
        makeAccessible(mPMField);
        Object mPM = mPMField.get(packageManager); // IPackageManager
        if (hooker != null) {
            packageManagerProxy = hooker.hook(mPM);
        } else {
            packageManagerProxy = mPM;
        }
        mPMField.set(packageManager, packageManagerProxy);
    }

    @SuppressLint("PrivateApi")
    public static void hook(ServiceManagerHooker hooker) throws ReflectiveOperationException {
        Class<?> serviceManagerClass = Class.forName("android.os.ServiceManager");
        Method getIServiceManagerMethod = serviceManagerClass.getDeclaredMethod("getIServiceManager");
        makeAccessible(getIServiceManagerMethod);

        // 确保sServiceManager不为null
        getIServiceManagerMethod.invoke(null);

        Field sServiceManagerField = serviceManagerClass.getDeclaredField("sServiceManager");
        makeAccessible(sServiceManagerField);
        Object sServiceManager = sServiceManagerField.get(null);

        Object serviceManagerProxy;
        if (hooker != null) {
            serviceManagerProxy = hooker.hook(sServiceManager);
        } else {
            serviceManagerProxy = sServiceManager;
        }
        sServiceManagerField.set(null, serviceManagerProxy);
    }

    @SuppressLint("PrivateApi")
    public static void hook(String key, BinderHooker hooker) throws ReflectiveOperationException {
        Class<?> serviceManagerClass = Class.forName("android.os.ServiceManager");

        Method getServiceMethod = serviceManagerClass.getDeclaredMethod("getService", String.class);
        makeAccessible(getServiceMethod);
        IBinder clipboardRawBinder = (IBinder) getServiceMethod.invoke(null, Context.CLIPBOARD_SERVICE);

        Field sCacheField = serviceManagerClass.getDeclaredField("sCache");
        makeAccessible(sCacheField);
        //noinspection unchecked
        Map<String, IBinder> sCache = (Map<String, IBinder>) sCacheField.get(null);
        IBinder binderProxy;
        if (hooker != null) {
            binderProxy = hooker.hook(clipboardRawBinder);
            if (binderProxy == null) {
                binderProxy = clipboardRawBinder;
            }
        } else {
            binderProxy = clipboardRawBinder;
        }
        sCache.put(key, binderProxy);
    }

    public static IBinder getBinderProxy(IBinder iBinder,
                                         String stubClass,
                                         String iInterfaceClass,
                                         Class<? extends IInterfaceBaseProxyHandler> handlerClass) {
        try {
            return getBinderProxy(iBinder, Class.forName(stubClass), Class.forName(iInterfaceClass), handlerClass);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
            return iBinder;
        }
    }

    public static IBinder getBinderProxy(IBinder iBinder,
                                         Class<?> stubClass,
                                         Class<?> iInterfaceClass,
                                         Class<? extends IInterfaceBaseProxyHandler> handlerClass) {
        return (IBinder) Proxy.newProxyInstance(stubClass.getClassLoader(), new Class[]{IBinder.class},
                new Hooks.BinderProxyHandler(iBinder, stubClass, iInterfaceClass, handlerClass));
    }

    private static void makeAccessible(AccessibleObject accessibleObject) {
        if (!accessibleObject.isAccessible()) {
            accessibleObject.setAccessible(true);
        }
    }

    public interface InstrumentationHooker {
        Instrumentation hook(Instrumentation instrumentation);
    }

    public interface ActivityManagerHooker {
        Object hook(Object iActivityManager);
    }

    public interface PackageManagerHooker {
        Object hook(Object iPackageManager);
    }

    public interface ServiceManagerHooker {
        Object hook(Object iServiceManager);
    }

    public interface BinderHooker {
        IBinder hook(@Nullable IBinder iBinder);
    }

    public static class BinderProxyHandler implements InvocationHandler {

        private IBinder base;
        private Class<?> stubClass;
        private Class<?> iInterfaceClass;
        private Class<? extends IInterfaceBaseProxyHandler> iInterfaceProxyHandlerClass;

        public BinderProxyHandler(IBinder base,
                                  Class<?> stubClass,
                                  Class<?> iInterfaceClass,
                                  Class<? extends IInterfaceBaseProxyHandler> iInterfaceProxyHandlerClass) {
            this.base = base;
            this.stubClass = stubClass;
            this.iInterfaceClass = iInterfaceClass;
            this.iInterfaceProxyHandlerClass = iInterfaceProxyHandlerClass;
        }

        @Override
        public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
            if ("queryLocalInterface".equals(method.getName())) {
                return Proxy.newProxyInstance(proxy.getClass().getClassLoader(),
                        new Class[]{IBinder.class, IInterface.class, this.iInterfaceClass},
                        iInterfaceProxyHandlerClass.getDeclaredConstructor(IBinder.class, Class.class).newInstance(base, stubClass));
            }
            return method.invoke(base, args);
        }
    }

    public static abstract class IInterfaceBaseProxyHandler implements InvocationHandler {
        // 原始的Service对象 (IInterface)
        protected Object base;

        public IInterfaceBaseProxyHandler(IBinder base, Class<?> stubClass) {
            try {
                Method asInterfaceMethod = stubClass.getDeclaredMethod("asInterface", IBinder.class);
                this.base = asInterfaceMethod.invoke(null, base);
            } catch (Exception e) {
                throw new RuntimeException("hooked failed!");
            }
            if (base == null) {
                throw new RuntimeException("hooked failed! base is null");
            }
        }

    }

}