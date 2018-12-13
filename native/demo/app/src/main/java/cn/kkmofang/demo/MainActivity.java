package cn.kkmofang.demo;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import cn.kkmofang.ker.App;
import cn.kkmofang.ker.Package;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        App.AppActivityClass = AppActivity.class;

        cn.kkmofang.ker.Package.getPackage(this, "https://storage.sc.weibo.com/ker/demo/1.0.ker", new Package.Callback() {
            @Override
            public void onError(Throwable ex) {
                Log.e("ker",Log.getStackTraceString(ex));
            }

            @Override
            public void onLoad(Package pkg) {
                App.open(MainActivity.this,pkg.basePath,pkg.appkey,null);
            }

            @Override
            public void onProgress(long bytes, long total) {

            }
        });

    }
}
