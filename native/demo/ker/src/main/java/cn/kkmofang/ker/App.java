package cn.kkmofang.ker;

import android.app.Activity;
import android.webkit.WebSettings;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

/**
 * Created by zhanghailong on 2018/12/12.
 */

public class App {

    public interface Openlib {
        void open(long jsContext,App app);
    }

    private static List<Openlib> _openlibs = new ArrayList<>();

    public static void addOpenlib(Openlib openlib){
        _openlibs.add(openlib);
    }

    public static void openlib(long jsContext,App app) {
        for(Openlib v : _openlibs) {
            v.open(jsContext,app);
        }
    }

    private final String _basePath;
    private final String _appkey;
    private final List<WeakReference<Activity>> _activitys = new LinkedList<>();
    private final long _jsContext;
    private long _ptr;

    public App(Activity activity , String basePath, String appkey) {
        _ptr = alloc(this,basePath,appkey,WebSettings.getDefaultUserAgent(activity));
        _activitys.add(new WeakReference<>(activity));
        _basePath = basePath;
        _appkey = appkey;
        _jsContext = jsContext(_ptr);
        openlib(_jsContext,this);
    }

    protected void finalize() throws Throwable {
        if(_ptr != 0) {
            dealloc(_ptr);
        }
        super.finalize();
    }

    public void run(Map<String,String> query) {
        if(_ptr != 0) {
            run(_ptr,query);
        }
    }

    public void open(String uri,boolean animated) {

    }

    public void back(int delta,boolean animated) {

    }

    public void recycle() {
        if(_ptr != 0) {
            dealloc(_ptr);
            _ptr = 0;
        }
    }

    public Activity topActivity() {

        Activity topActivity = null;
        Iterator<WeakReference<Activity>> i = _activitys.iterator();

        while(i.hasNext()) {
            Activity v = i.next().get();
            if(v == null || v.isFinishing()) {
                i.remove();
            } else if(v instanceof PageActivity){
                if(((PageActivity) v).isFrontground()) {
                    return v;
                } else if(topActivity == null){
                    topActivity = v;
                }
            } else {
                return v;
            }
        }

        return topActivity;
    }

    private static native long alloc(App object,String basePath,String appkey,String userAgent);
    private static native void dealloc(long ptr);
    private static native long jsContext(long ptr);
    private static native long run(long ptr,Map<String,String> query);

}
