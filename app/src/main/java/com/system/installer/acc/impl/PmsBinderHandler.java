//PmsHookBinderInvocationHandler.java
package com.system.installer.acc.impl;

import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.util.Log;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;

public class PmsBinderHandler implements InvocationHandler {
    private Object base;

    public final static String SHARK = "pmsDarren";

    //应用正确的签名信息
    private String SIGN;
    private String appPkgName = "";

    public PmsBinderHandler(Object base, String sign, String appPkgName, int hashCode) {
        try {
            this.base = base;
            this.SIGN = sign;
            this.appPkgName = appPkgName;
        } catch (Exception e) {
            Log.d(SHARK, "error:"+Log.getStackTraceString(e));
        }
    }

    @Override
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        Log.i(SHARK, "PmsHookBinderInvocationHandler ppp:"+method.getName());
        //查看是否是getPackageInfo方法
       // dumpParams.dumpMethodParams(args);

        //Dump

        if("getPackageInfo".equals(method.getName())){
            String pkgName = (String)args[0];
            Integer flag = (Integer)args[1];
            //是否是获取我们需要hook apk的签名
            if(flag == PackageManager.GET_SIGNATURES && appPkgName.equals(pkgName)){
                //将构造方法中传进来的新的签名覆盖掉原来的签名
                Signature sign = new Signature(SIGN);
                PackageInfo info = (PackageInfo) method.invoke(base, args);
                info.signatures[0] = sign;
                return info;
            }
        }else if ("getNamesForUids".equals(method.getName())){

            int[] pkgName = (int[])args[0];
            for (int i = 0; i< pkgName.length; i++){
                Log.i(SHARK, "getNamesForUids index:"+pkgName[i]);
            }
        }
        return method.invoke(base, args);
    }
}
