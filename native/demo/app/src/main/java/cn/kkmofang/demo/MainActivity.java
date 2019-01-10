package cn.kkmofang.demo;

import android.os.Bundle;
import android.util.Log;

import cn.kkmofang.ker.AppActivity;
import cn.kkmofang.ker.KerUI;
import cn.kkmofang.ker.Native;

public class MainActivity extends AppActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        try {
            Class.forName("cn.kkmofang.wx.WXObject");
        } catch (ClassNotFoundException e) {
            Log.d("ker",Log.getStackTraceString(e));
        }
        KerUI.open("http://10.222.96.92:8080/1.0.ker",null,null);
    }
}
