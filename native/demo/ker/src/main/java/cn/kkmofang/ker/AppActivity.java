package cn.kkmofang.ker;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * Created by zhanghailong on 2018/12/13.
 */

public class AppActivity extends Activity implements IAppActivity {


    protected void onCreateContentView() {
        setContentView(R.layout.app);
    }

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        onCreateContentView();
        KerUI.setAppActivity(this);
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

    protected void onSaveInstanceState(Bundle outState) {

    }

    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }

    private List<Fragment> _showFragments = new ArrayList<>();

    @SuppressLint("ResourceType")
    @Override
    public void show(Fragment fragment,boolean animated) {
        try {
            FragmentManager fm = getFragmentManager();
            FragmentTransaction tr = fm.beginTransaction().add(R.id.ker_contentView, fragment);
            if (animated) {
                tr.setCustomAnimations(R.anim.show, 0);
            }
            tr.commit();
            _showFragments.add(fragment);
        }
        catch (Throwable e) {
            Log.e("ker",Log.getStackTraceString(e));
        }
    }

    @SuppressLint("ResourceType")
    @Override
    public void close(Fragment fragment,boolean animated) {
        try {
            FragmentManager fm = getFragmentManager();
            FragmentTransaction tr = fm.beginTransaction().remove(fragment);
            if(animated) {
                //tr.setCustomAnimations(R.anim.hide, 0);
            }
            tr.commit();
            _showFragments.remove(fragment);
        }
        catch (Throwable e) {
            Log.e("ker",Log.getStackTraceString(e));
        }
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
