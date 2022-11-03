package com.system.installer.acc;

import android.app.Activity;
import android.os.Bundle;
import android.support.annotation.Nullable;

public class MainActivity extends Activity {


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        System.loadLibrary("native-lib");
        JNI.getInstance().myTest32();
        JNI.getInstance().myTest64(getApplication());
        JNI.getInstance().myTestComm(getApplication());
    }
}