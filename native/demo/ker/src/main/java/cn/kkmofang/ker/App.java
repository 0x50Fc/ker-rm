package cn.kkmofang.ker;

import android.app.Activity;
import android.app.IntentService;
import android.content.Intent;
import android.webkit.WebSettings;

import java.io.UnsupportedEncodingException;
import java.lang.ref.WeakReference;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

/**
 * Created by zhanghailong on 2018/12/12.
 */

public class App {

    static {
        System.loadLibrary("ker");
        openlib();
    }

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
    private final WeakReference<Activity> _activity;
    private final long _jsContext;
    private long _ptr;

    public long ptr() {
        return _ptr;
    }

    public App(Activity activity , String basePath, String appkey) {
        _ptr = alloc(this,basePath,appkey,WebSettings.getDefaultUserAgent(activity));
        _activity = new WeakReference<>(activity);
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

    public void run(Object query) {
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

    public Activity activity() {
        return _activity.get();
    }


    public static String stringValue(Object v,String defaultValue) {

        if(v != null) {
            if(v instanceof String ){
                return (String) v;
            }
            return v.toString();
        }
        return defaultValue;
    }

    public static String encodeQuery(Object query){
        StringBuffer sb = new StringBuffer();

        if(query != null) {
            String dot = "";
            if(query instanceof Map) {
                Map<?,?> m = (Map<?,?>) query;
                for(Object key : m.keySet()) {
                    sb.append(dot);
                    dot = "&";
                    sb.append(stringValue(key,""));
                    sb.append("=");
                    try {
                        sb.append(URLEncoder.encode(stringValue(m.get(key),""),"UTF-8"));
                    } catch (UnsupportedEncodingException e) {
                    }
                }
            }

        }

        return sb.toString();
    }

    public static Map<String,String> decodeQuery(String queryString) {
        Map<String,String> m = new TreeMap<>();
        if(queryString != null) {
            String[] vs = queryString.split("&");
            for(String v : vs) {
                String[] kv = v.split("=");
                if(kv.length > 1) {
                    try {
                        m.put(kv[0], URLDecoder.decode(kv[1],"UTF-8"));
                    } catch (UnsupportedEncodingException e) {
                    }
                }
            }
        }
        return m;
    }

    public static Class<?> AppActivityClass = AppActivity.class;

    public static void open(Activity activity,String basePath,String appkey,Object query) {

        Intent i = new Intent(activity,AppActivityClass);

        i.putExtra("basePath",basePath);
        i.putExtra("appkey",appkey);
        i.putExtra("query",encodeQuery(query));

        activity.startActivity(i);
    }

    private static native long alloc(App object,String basePath,String appkey,String userAgent);
    private static native void dealloc(long ptr);
    private static native long jsContext(long ptr);
    private static native void run(long ptr,Object query);
    private static native void openlib();
}
