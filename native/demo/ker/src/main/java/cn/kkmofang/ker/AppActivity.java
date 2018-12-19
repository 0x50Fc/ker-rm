package cn.kkmofang.ker;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.os.Bundle;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * Created by zhanghailong on 2018/12/13.
 */

public class AppActivity extends Activity implements IAppActivity {

    protected App _app;

    protected void onCreateContentView() {
        setContentView(R.layout.app);
    }

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        onCreateContentView();

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

    @Override
    public App app() {
        return _app;
    }

    private List<Fragment> _fragments = new ArrayList<>();

    @SuppressLint("ResourceType")
    @Override
    public void open(Fragment fragment, boolean animated) {
        FragmentManager fm = getFragmentManager();
        int n = _fragments.size();
        FragmentTransaction tr = fm.beginTransaction();

        if(animated && n > 0) {
            tr.setCustomAnimations(R.anim.slide_right_in, R.anim.slide_left_out,R.anim.slide_right_in, R.anim.slide_left_out);
        }

        if(n > 0) {
            tr.hide(_fragments.get(n - 1));
        }
        tr.add(R.id.ker_contentView, fragment);
        _fragments.add(fragment);
        tr.commit();
    }

    @SuppressLint("ResourceType")
    @Override
    public void back(int delta, boolean animated) {
        FragmentManager fm = getFragmentManager();

        FragmentTransaction tr = fm.beginTransaction();

        if(animated) {
            tr.setCustomAnimations(R.anim.slide_left_in, R.anim.slide_right_out,R.anim.slide_left_in, R.anim.slide_right_out);
        }

        int n = _fragments.size();

        if(n > 1 && delta > 0) {
            int v = Math.min(delta,n - 1);
            int b = n - 1 - v;
            int e = n - 1;
            for(int i=b;i<=e;i++) {
                Fragment f = _fragments.get(i);
                if(i == b) {
                    tr.show(f);
                } else {
                    tr.remove(f);
                }
            }
            while(v >0){
                _fragments.remove(b);
                v --;
            }
        }

        tr.commit();

    }

    private List<Fragment> _showFragments = new ArrayList<>();

    @SuppressLint("ResourceType")
    @Override
    public void show(Fragment fragment,boolean animated) {
        FragmentManager fm = getFragmentManager();
        FragmentTransaction tr = fm.beginTransaction().add(R.id.ker_contentView, fragment);
        if(animated) {
            tr.setCustomAnimations(R.anim.show, 0);
        }
        tr.commit();
        _showFragments.add(fragment);
    }

    @SuppressLint("ResourceType")
    @Override
    public void close(Fragment fragment,boolean animated) {
        FragmentManager fm = getFragmentManager();
        FragmentTransaction tr = fm.beginTransaction().remove(fragment);
        if(animated) {
            tr.setCustomAnimations(R.anim.hide, 0);
        }
        tr.commit();
        _showFragments.remove(fragment);
    }

    public void onBackPressed() {

        if(_showFragments.size() > 0) {

            int i = _showFragments.size() - 1;

            while(i >= 0) {
                Fragment fragment = _showFragments.get(i);
                if(fragment instanceof PageFragment) {
                    if(((PageFragment) fragment).onBackPressed()) {
                        close(fragment,true);
                        return;
                    }
                }
                i --;
            }

        }

        if(_fragments.size() > 1) {
            Fragment fragment = _fragments.get(_fragments.size() - 1);
            if(fragment instanceof PageFragment) {
                if(!((PageFragment) fragment).onBackPressed()) {
                    return;
                }
            }
            back(1,true);
            return;
        }


        super.onBackPressed();

    }
}
