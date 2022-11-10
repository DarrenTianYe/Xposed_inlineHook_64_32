package com.system.installer.acc;

import android.app.Activity;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button bt1 =  findViewById(R.id.button1);
        bt1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

//                Intent intent = new Intent(getApplicationContext(), TelphoneCellInfoActivity.class);
//                startActivity(intent);
                Toast.makeText(getApplicationContext(), "TelphoneCellInfoActivity", Toast.LENGTH_LONG).show();
            }
        });

        TextView tvb_dtx = findViewById(R.id.sample_text);
        String nativeStr32 =  JNI.getInstance().myTest32(getApplication().getApplicationContext());

        String nativeStr64 =  JNI.getInstance().myTest64(getApplication());
        String nativeStrComm  = JNI.getInstance().myTestComm(getApplication());

        tvb_dtx.setText("nativeStr32:"+nativeStr32+"\n\n"+
                "nativeStr64:"+nativeStr64+"\n\n"+"nativeStrComm:"+nativeStrComm);


    }
}