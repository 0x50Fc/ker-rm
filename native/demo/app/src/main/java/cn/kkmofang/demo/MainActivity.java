package cn.kkmofang.demo;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import cn.kkmofang.ker.App;
import cn.kkmofang.ker.Package;

public class MainActivity extends Activity {

    static {
        try {
            Class.forName("cn.kkmofang.ker.App");
            Class.forName("cn.kkmofang.wx.WXObject");
        } catch (ClassNotFoundException e) {
            Log.d("ker",Log.getStackTraceString(e));
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        App.AppActivityClass = AppActivity.class;

        cn.kkmofang.ker.Package.getPackage(this, "http://10.222.96.92:8080/1.0.ker", new Package.Callback() {
            @Override
            public void onError(Throwable ex) {
                Log.e("ker",Log.getStackTraceString(ex));
            }

            @Override
            public void onLoad(Package pkg) {
                App.open(MainActivity.this,pkg.basePath,pkg.appkey,null);
                MainActivity.this.finish();
            }

            @Override
            public void onProgress(long bytes, long total) {

            }
        });

    }
}
