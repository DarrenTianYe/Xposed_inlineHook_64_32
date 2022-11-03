package com.system.installer.acc.utils;

import android.util.Log;

public class DumpParams {

    public static final String TAG = "dumpParamsDarren";

    public static void  dumpMethodParams(Object[] args){

        if (args == null || args.length == 0){
            return;
        }
        for (int i = 0; i< args.length ; i++){
            try {
                Log.e(TAG, "args:"+args[i]);
            }catch (Throwable e){
                e.printStackTrace();
            }
        }
    }
}
