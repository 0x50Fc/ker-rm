package cn.kkmofang.ker;

import android.os.Handler;
import android.os.Looper;
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
            if(_ptr != 0) {
                setSize(_ptr, _layoutWidth, _layoutHeight);
            }
        }
    }

    public interface Openlib {
        void open(Page page);
    }

    private static List<Openlib> _openlibs = new ArrayList<>();

    public static void addOpenlib(Openlib openlib){
        _openlibs.add(openlib);
    }


    public void addLibrary(String name,Object object) {
        if(_ptr != 0) {
            addLibrary(_ptr, name, object);
        }
    }

    public final long pageId;
    private long _ptr;
    private PageView _view;
    private final Handler _handler;
    private Object _options;
    protected boolean _backPressed = false;

    public long ptr() {
        return _ptr;
    }

    Page(long pageId,long ptr) {
        this.pageId = pageId;
        _ptr = ptr;
        _handler = new Handler(Looper.getMainLooper());
        for(Openlib v : _openlibs) {
            v.open(this);
        }
    }

    public void recycle() {
        if(_ptr != 0) {
            dealloc(_ptr);
            _ptr = 0;
        }
        if(_view != null) {
            _view.setPageListener(null);
            _view = null;
        }
        KerUI.removePage(pageId);
    }

    public void setOptions(final Object options) {

        if(options != null) {
            Object v = Ker.get(options,"backPressed");
            if(v != null) {
                _backPressed = Ker.booleanValue(v,false);
            }
        }

        _options = options;
        if(_listener != null) {
            Listener v = _listener.get();
            if(v != null) {
                v.onOptions(options);
            }
        }

    }

    public void close(final boolean animated) {
        if(_listener != null) {
            Listener v = _listener.get();
            if(v != null) {
                v.onClose(animated);
            }
        }
    }

    public boolean isBackPressed() {
        return _backPressed;
    }

    public void emit(String name,Object data) {
        if(_ptr != 0) {
            emit(_ptr,name,data);
        }
    }


    public View view() {
        return _view;
    }

    public void open(PageView view) {
        if(_view != view) {
            if(_view != null) {
                _view.setPageListener(null);
            }
            _view = view;
            if(_view != null) {
                _view.setPageListener(this);
            }
        }

        if(_listener != null && _options != null) {
            Listener v = _listener.get();
            if(v != null) {
                v.onOptions(_options);
            }
        }

        if(_ptr != 0 && _view != null) {
            open(_ptr,_view,_view.getWidth(),_view.getHeight());
        }

    }

    protected void finalize() throws Throwable {
        recycle();
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

    private static native void dealloc(long ptr);
    private static native void open(long ptr,View view,int width,int height);
    private static native void setSize(long ptr,int width,int height);
    private static native void addLibrary(long ptr,String name,Object object);
    private static native void emit(long ptr,String name,Object data);
}
