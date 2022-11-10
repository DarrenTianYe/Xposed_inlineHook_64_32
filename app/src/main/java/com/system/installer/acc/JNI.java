package com.system.installer.acc;

import android.content.Context;

public class JNI {

    private static JNI gJni = null;

    public static JNI getInstance(){
        if (gJni == null){
            gJni =new JNI();
        }
        return gJni;
    }

    public native String myTest32(Context context);
    public native String myTest64(Context context);
    public native String myTestComm(Context context);
}
