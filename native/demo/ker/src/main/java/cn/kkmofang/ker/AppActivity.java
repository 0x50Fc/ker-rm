package cn.kkmofang.ker;

import android.app.Activity;
import android.os.Bundle;

import java.util.Map;

/**
 * Created by hailong11 on 2018/12/13.
 */

public class AppActivity extends Activity {

    protected App _app;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        String basePath = null;
        String appkey = null;
        Map<String,String> query = null;

        if(basePath == null && savedInstanceState != null) {
            basePath = savedInstanceState.getString("basePath");
        }

        if(appkey == null && savedInstanceState != null) {
            appkey = savedInstanceState.getString("appkey");
        }

        if(query == null && savedInstanceState != null) {
            String v = savedInstanceState.getString("query");
            if(v != null) {
                query = App.decodeQuery(v);
            }
        }

        if(basePath == null) {
            basePath = getIntent().getStringExtra("basePath");
        }

        if(appkey == null) {
            appkey = getIntent().getStringExtra("appkey");
        }

        if(query == null) {
            String v = getIntent().getStringExtra("query");
            if(v != null) {
                query = App.decodeQuery(v);
            }
        }

        if(_app != null) {
            _app.recycle();
        }

        if(basePath != null && appkey != null ) {
            _app = new App(this,basePath,appkey);
            _app.run(query);
        }

    }
}
