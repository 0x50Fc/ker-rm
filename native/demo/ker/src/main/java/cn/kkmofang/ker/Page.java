package cn.kkmofang.ker;

import android.os.Handler;
import android.util.DisplayMetrics;
import android.view.View;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * Created by zhanghailong on 2018/12/12.
 */

public class Page implements PageView.PageListener{

    private int _layoutWidth = -1;
    private int _layoutHeight = -1;

    @Override
    public void onPageLayout(PageView view, int width, int height) {
        if(_layoutWidth != width || _layoutHeight != height) {
            _layoutWidth = width;
            _layoutHeight = height;
            if(_ptr != 0 && _app != null) {
                DisplayMetrics metrics = _app.activity().getResources().getDisplayMetrics();
                setSize(_ptr, (int) (_layoutWidth / metrics.density), (int) (_layoutHeight / metrics.density));
            }
        }
    }

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


    public void addLibrary(String name,Object object) {
        if(_ptr != 0) {
            addLibrary(_ptr, name, object);
        }
    }

    private long _ptr;
    private long _jsContext;
    private App _app;
    private PageView _view;
    private final Handler _handler;

    public long ptr() {
        return _ptr;
    }

    public Page(PageView view, App app) {
        _ptr = alloc(view,app.ptr());
        _jsContext = jsContext(_ptr);
        _app = app;
        _view = view;
        _view.setPageListener(this);
        _handler = new Handler();
        openlib(_jsContext,this);
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

    public void close(final boolean animated) {
        if(_listener != null) {
            final Listener v = _listener.get();
            if(v != null) {
                _handler.post(new Runnable() {
                    @Override
                    public void run() {
                        v.onClose(animated);
                    }
                });

            }
        }
    }

    public boolean onBackPressed() {
        if(_ptr != 0) {
            return onBackPressed(_ptr);
        }
        return true;
    }

    public View view() {
        return _view;
    }

    public App app() {
        return _app;
    }

    public void run(String path,Map<String,String> query) {
        int n = query == null ? 0 : query.size();
        String[] keys = new String[n];
        String[] values = new String[n];
        if(n >0) {
            int i = 0;
            for (Map.Entry<String, String> e : query.entrySet()) {
                keys[i] = e.getKey();
                values[i] = e.getValue();
                i++;
            }
        }
        run(_ptr,path,keys,values);
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

    private native long alloc(View view,long app);
    private static native void dealloc(long ptr);
    private static native long jsContext(long ptr);
    private static native void run(long ptr,String path,String[] keys,String[] values);
    private static native void setSize(long ptr,int width,int height);
    private static native void addLibrary(long ptr,String name,Object object);
    private static native boolean onBackPressed(long ptr);
}
