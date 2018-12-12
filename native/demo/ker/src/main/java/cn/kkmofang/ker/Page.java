package cn.kkmofang.ker;

import android.app.Activity;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

/**
 * Created by zhanghailong on 2018/12/12.
 */

public class Page {


    public interface Openlib {
        void open(long jsContext,Page page);
    }

    private static List<Openlib> _openlibs = new ArrayList<>();

    public static void addOpenlib(Openlib openlib){
        _openlibs.add(openlib);
    }

    public static void openlib(long jsContext,Page page) {
        for(Openlib v : _openlibs) {
            v.open(jsContext,page);
        }
    }

    private final Map<String,Object> _librarys = new TreeMap<>();

    public void addLibrary(String name,Object object) {
        _librarys.put(name,object);
    }

    private final WeakReference<Activity> _activity;

    public Page(Activity activity) {
        _activity = new WeakReference<>(activity);
    }

    public Activity getActivity(){
        return _activity.get();
    }
}
