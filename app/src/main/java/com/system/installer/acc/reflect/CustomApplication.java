package com.system.installer.acc.reflect;
import android.app.Application;

public class CustomApplication extends Application
{

    @Override
    public void onCreate()
    {
        super.onCreate();


        System.loadLibrary("native-lib");

    }
}