package cn.kkmofang.ker;

import android.content.Context;
import android.util.AttributeSet;

import java.lang.ref.WeakReference;

/**
 * Created by zhanghailong on 2018/12/17.
 */

public class PageView extends KerView {

    public PageView(Context context) {
        super(context);
    }

    public PageView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        if(_pageListener != null) {
            PageListener v = _pageListener.get();
            if(v != null) {
                v.onPageLayout(this,r - l, b - t);
            }
        }
        super.onLayout(changed,l,t,r,b);
    }

    private WeakReference<PageListener> _pageListener;

    public void setPageListener(PageListener pageListener) {
        _pageListener = pageListener == null ? null : new WeakReference<>(pageListener);
    }

    public interface PageListener {
        void onPageLayout(PageView view,int width,int height);
    }
}
