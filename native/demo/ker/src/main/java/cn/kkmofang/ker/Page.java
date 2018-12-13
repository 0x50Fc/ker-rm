package cn.kkmofang.ker;

import android.app.Activity;
import android.view.View;

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

    private long _ptr;
    private long _jsContext;
    private App _app;
    private View _view;

    public long ptr() {
        return _ptr;
    }

    public Page(View view, App app) {
        _ptr = alloc(view,app.ptr());
        _jsContext = jsContext(_ptr);
        _app = app;
        _view = view;
    }

    public void recycle() {
        if(_ptr != 0) {
            dealloc(_ptr);
            _ptr = 0;
        }
        _app = null;
        _view = null;
    }

    public void setOptions(Object options) {
        if(_listener != null) {
            Listener v = _listener.get();
            if(v != null) {
                v.onOptions(options);
            }
        }
    }

    public void close(boolean animated) {
        if(_listener != null) {
            Listener v = _listener.get();
            if(v != null) {
                v.onClose(animated);
            }
        }
    }

    public View view() {
        return _view;
    }

    public App app() {
        return _app;
    }

    protected void finalize() throws Throwable {
        if(_ptr != 0) {
            dealloc(_ptr);
        }
        super.finalize();
    }

    private WeakReference<Listener> _listener;

    public void setListener(Listener listener) {
        if(listener == null) {
            _listener =  null;
        } else {
            _listener = new WeakReference<>(listener);
        }
    }

    public interface Listener {
        void onOptions(Object options);
        void onClose(boolean animated);
    }

    private static native long alloc(View view,long app);
    private static native void dealloc(long ptr);
    private static native long jsContext(long ptr);
}
