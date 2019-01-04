package cn.kkmofang.ker;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.text.Layout;
import android.text.SpannableString;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.text.StaticLayout;
import android.text.TextPaint;
import android.text.style.AbsoluteSizeSpan;
import android.text.style.DynamicDrawableSpan;
import android.text.style.ForegroundColorSpan;
import android.text.style.StyleSpan;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;
import android.webkit.MimeTypeMap;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

import cn.kkmofang.ker.http.SessionTask;
import cn.kkmofang.ker.image.ImageCache;

/**
 * Created by zhanghailong on 2019/1/3.
 */

public final class KerUI {

    private final static Map<Long,Page> _pages = new TreeMap<>();
    private final static Handler _handler;

    static {
        _handler = new Handler(Looper.getMainLooper());
    }

    private static String _userAgent;

    public static String userAgent() {
        return _userAgent;
    }

    public static void createPage(final long pageId,long ptr) {
        final Page page = new Page(pageId,ptr);
        _handler.post(new Runnable() {
            @Override
            public void run() {
                _pages.put(pageId,page);
            }
        });
    }

    public static Page getPage(long pageId) {

        if(_pages.containsKey(pageId)) {
            return _pages.get(pageId);
        }

        return null;
    }

    public static void removePage(long pageId) {

        if(_pages.containsKey(pageId)) {
            Page page = _pages.get(pageId);
            _pages.remove(pageId);
        }

    }


    public static void pageSetOptions(final long pageId, final Object options) {
        _handler.post(new Runnable() {
            @Override
            public void run() {
                Page page = getPage(pageId);

                if(page != null) {
                    page.setOptions(options);
                }
            }
        });
    }

    public static void pageClose(final long pageId, final boolean animated) {
        _handler.post(new Runnable() {
            @Override
            public void run() {
                Page page = getPage(pageId);

                if(page != null) {
                    page.close(animated);
                }
            }
        });
    }

    public static void openPageViewController(final long pageId,final boolean animated,final String type) {

        _handler.post(new Runnable() {
            @Override
            public void run() {

                Page page = getPage(pageId);

                if(page != null) {

                    Activity activity = getAppActivity();

                    if(activity instanceof IAppActivity) {
                        PageFragment fragment = new PageFragment();
                        Bundle b = new Bundle();
                        b.putLong("pageId",pageId);
                        b.putString("type",type);
                        fragment.setArguments(b);
                        if(PageFragment.TYPE_WINDOW.equals(type)) {
                            ((IAppActivity) activity).show(fragment,animated);
                        } else {
                            ((IAppActivity) activity).open(fragment,animated);
                        }
                    } else {
                        page.recycle();
                        return;
                    }
                }
            }
        });
    }

    public static void popPageViewController(final int delta,final boolean animated) {
        _handler.post(new Runnable() {
            @Override
            public void run() {

                Activity activity = getAppActivity();

                if(activity instanceof IAppActivity) {
                    ((IAppActivity) activity).back(delta,animated);
                }
            }
        });
    }


    private final static Map<String,Class<?>> _viewClass = new TreeMap<>();
    private final static Map<Long,View> _views = new TreeMap<>();

    public static void addViewClass(String name,Class<?> isa) {
        _viewClass.put(name,isa);
    }

    public static void createView(final String name, final WebViewConfiguration configuration, final long viewId, final long appid) {

        _handler.post(new Runnable() {
            @Override
            public void run() {

                Activity activity = getAppActivity();

                if(activity == null) {
                    return;
                }

                Class<?> isa = null;

                if(_viewClass.containsKey(name)) {
                    isa = _viewClass.get(name);
                }

                if(isa == null) {
                    isa = KerView.class;
                }

                try {
                    View view = (View) isa.getConstructor(Context.class).newInstance(activity.getApplicationContext());
                    if(view instanceof IKerView) {
                        ((IKerView) view).obtain(viewId,appid);
                        ((IKerView) view).setViewConfiguration(viewId,configuration,appid);
                    }
                    _views.put(viewId,view);
                }
                catch (Throwable ex) {
                    Log.e("ker",Log.getStackTraceString(ex));
                }

            }
        });
    }

    public static void createView(final View view,final long viewId, final long appid) {

        _handler.post(new Runnable() {
            @Override
            public void run() {

                if(view instanceof IKerView) {
                    ((IKerView) view).obtain(viewId,appid);
                }

                _views.put(viewId,view);

            }
        });

    }

    public static void deleteView(final long viewId,final long appid) {
        _handler.post(new Runnable() {
            @Override
            public void run() {
                if(_views.containsKey(viewId)) {
                    View view = _views.get(viewId);
                    if(view instanceof IKerView) {
                        ((IKerView) view).recycle(viewId,appid);
                    }
                    _views.remove(viewId);
                }
            }
        });
    }

    public static void setAttribute(final long viewId,final String name,final String value,final long appid) {
        _handler.post(new Runnable() {
            @Override
            public void run() {
                if(_views.containsKey(viewId)) {
                    View view = _views.get(viewId);
                    if(view instanceof IKerView) {
                        ((IKerView) view).setAttributeValue(viewId,name,value,appid);
                    } else {
                        KerView.setAttributeValue(view,viewId,name,value,appid);
                    }
                }
            }
        });
    }

    public static void setFrame(final long viewId, int x, int y, int width, int height,final long appid) {
        final Rect frame = new Rect();
        frame.x = x;
        frame.y = y;
        frame.width = width;
        frame.height = height;
        _handler.post(new Runnable() {
            @Override
            public void run() {
                if(_views.containsKey(viewId)) {
                    View view = _views.get(viewId);
                    view.setTag(R.id.ker_frame,frame);
                    ViewParent p = view.getParent();
                    if(p != null) {
                        p.requestLayout();
                    }
                }
            }
        });
    }

    public static void setContentSize(final long viewId, final int width, final int height,final long appid) {
        _handler.post(new Runnable() {
            @Override
            public void run() {
                if(_views.containsKey(viewId)) {
                    View view = _views.get(viewId);
                    if(view instanceof IKerView) {
                        ((IKerView) view).setContentSize(viewId,width,height,appid);
                    }
                }
            }
        });
    }

    public static void setContentOffset(final long viewId, final int x, final int y,final boolean animated,final long appid) {
        _handler.post(new Runnable() {
            @Override
            public void run() {
                if(_views.containsKey(viewId)) {
                    View view = _views.get(viewId);
                    if(view instanceof IKerView) {
                        ((IKerView) view).setContentOffset(viewId,x,y,animated,appid);
                    }
                }
            }
        });
    }

    public static void setContent(final long viewId, final String content, final String contentType, final String basePath, final long appid) {
        _handler.post(new Runnable() {
            @Override
            public void run() {
                if(_views.containsKey(viewId)) {
                    View view = _views.get(viewId);
                    if(view instanceof IKerView) {
                        ((IKerView) view).setContent(viewId,content,contentType,basePath,appid);
                    }
                }
            }
        });
    }

    public static void setImage(final long viewId, final Object image, final long appid) {

        _handler.post(new Runnable() {
            @Override
            public void run() {
                if(_views.containsKey(viewId)) {
                    View view = _views.get(viewId);
                    if(view instanceof IKerView) {
                        ((IKerView) view).setImage(viewId,image,appid);
                    } else if(image instanceof Drawable) {
                        view.setBackground((Drawable) image);
                    } else if(image instanceof Image) {
                        view.setBackground(((Image) image).getDrawable());
                    }
                }
            }
        });
    }

    public static void evaluateJavaScript(final long viewId, final String evaluateCode, final long appid) {

        _handler.post(new Runnable() {
            @Override
            public void run() {
                if(_views.containsKey(viewId)) {
                    View view = _views.get(viewId);
                    if(view instanceof IKerView) {
                        ((IKerView) view).evaluateJavaScript(viewId,evaluateCode,appid);
                    } else if(view instanceof WebView) {
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                            ((WebView) view).evaluateJavascript(evaluateCode,null);
                        } else {
                            ((WebView) view).loadUrl("javascript:(function(){" +evaluateCode+ "})();");
                        }
                    }
                }
            }
        });

    }

    public static void setAttributedText(final long viewId, final CharSequence string, final long appid) {
        _handler.post(new Runnable() {
            @Override
            public void run() {
                if(_views.containsKey(viewId)) {
                    View view = _views.get(viewId);
                    if(view instanceof IKerView) {
                        ((IKerView) view).setAttributedText(viewId,string,appid);
                    } else if(view instanceof TextView) {
                        ((TextView)view).setText(string);
                    }
                }
            }
        });
    }

    public static void addSubview(final long viewId, final long subviewId, final int position, final long appid) {
        _handler.post(new Runnable() {
            @Override
            public void run() {
                if(_views.containsKey(viewId) && _views.containsKey(subviewId)) {
                    View view = _views.get(viewId);
                    View subview = _views.get(subviewId);
                    if(view instanceof IKerView) {
                        view = ((IKerView) view).contentView();
                    }
                    if(view != null && view instanceof ViewGroup) {
                        if(position == 0) {
                            ((ViewGroup) view).addView(subview);
                        } else {
                            ((ViewGroup) view).addView(subview,0);
                        }
                    }
                }
            }
        });

    }

    public static void removeView(final long viewId, final long appid) {
        _handler.post(new Runnable() {
            @Override
            public void run() {
                if(_views.containsKey(viewId) ) {
                    View view = _views.get(viewId);
                    ViewParent p = view.getParent();
                    if(p instanceof ViewGroup){
                        ((ViewGroup) p).removeView(view);
                    }
                }
            }
        });
    }

    private static WeakReference<Activity> _appActivity;

    public static void setAppActivity(Activity appActivity) {
        if(appActivity == null) {
            _appActivity = null;
        } else {
            _appActivity = new WeakReference<>(appActivity);
            if(_userAgent == null) {
                openlib(appActivity);
            }
        }
    }

    public static Activity getAppActivity() {
        if(_appActivity != null) {
            Activity activity = _appActivity.get();
            if(activity != null) {
                if(activity.isFinishing()) {
                    _appActivity = null;
                } else {
                    return activity;
                }
            }
        }
        return null;
    }

    private static void setImage(long id,Object image) {
        int width = 0;
        int height = 0;
        if(image instanceof BitmapDrawable) {
            Bitmap b = ((BitmapDrawable) image).getBitmap();
            width = b.getWidth();
            height = b.getHeight();
        } else if(image instanceof Image) {
            Bitmap b = ((Image) image).getBitmap();
            width = b.getWidth();
            height = b.getHeight();
        }
        setImage(id,image,width,height);
    }

    public static void getImage(final long id,String URI) {

        final Drawable image = ImageCache.getDefaultImageCache().getImageWithCache(URI);

        if(image != null) {
            setImage(id,image);
        } else {

            Activity appActivity = getAppActivity();

            if(appActivity == null) {
                setImage(id,null);
            } else {

                ImageCache.getDefaultImageCache().getImage(appActivity.getApplicationContext(), URI, new ImageCache.Callback() {

                    @Override
                    public void onError(Throwable ex) {
                        Log.e("ker", Log.getStackTraceString(ex));
                        setImage(id,null);
                    }

                    @Override
                    public void onImage(Drawable image) {
                        setImage(id,image);
                    }

                });
            }
        }
    }

    public static byte[] copyPixels(Object image) {

        Bitmap b;

        if(image instanceof BitmapDrawable) {
            b = ((BitmapDrawable) image).getBitmap();
        } else if(image instanceof Image) {
            b = ((Image) image).getBitmap();
        } else {
            return null;
        }

        ByteBuffer data = ByteBuffer.allocate(b.getWidth() * b.getHeight() * 4);
        if(b.getConfig() == Bitmap.Config.ARGB_8888) {
            b.copyPixelsToBuffer(data);
        } else {
            Bitmap cp = b.copy(Bitmap.Config.ARGB_8888,false);
            cp.copyPixelsToBuffer(data);
            cp.recycle();
        }

        return data.array();
    }

    public static String getDirectory(String name) {

        Activity activity = getAppActivity();

        if(activity == null) {
            return null;
        }

        return activity.getDir("ker-" + name, Context.MODE_PRIVATE).getAbsolutePath();
    }

    public static String mimeType(String path,byte[] data,String defaultType) {
        String v = MimeTypeMap.getFileExtensionFromUrl(path);
        if(v == null) {
            if(data != null && data.length >0) {
                switch (data[0]){
                    case (byte) 0xFF:
                        v = "image/jpeg";
                        break;
                    case (byte) 0x89:
                        v = "image/png";
                        break;
                    case (byte) 0x47:
                        v = "image/gif";
                        break;
                    case (byte) 0x49:
                    case (byte) 0x4D:
                        v = "image/tiff";
                        break;
                }
            }
        }

        if(v == null) {
            return defaultType;
        }

        return v;
    }

    static  {
        _viewClass.put("UILabel",KerTextView.class);
        _viewClass.put("UIView",KerView.class);
        _viewClass.put("KerButton",KerButton.class);
        _viewClass.put("WKWebView",KerWebView.class);
        _viewClass.put("UICanvasView",KerCanvasView.class);
    }

    public interface Openlib {
        void open(long appid);
    }

    private static List<Openlib> _openlibs = new ArrayList<>();

    public static void addOpenlib(Openlib openlib){
        _openlibs.add(openlib);
    }

    public static void openlib(long appid) {
        for(Openlib v : _openlibs) {
            v.open(appid);
        }
    }

    public static String getPrototype(Class<?> isa) {

        if(isa == null) {
            return null;
        }

        if(isa != Object.class) {
            JSPrototype p = isa.getAnnotation(JSPrototype.class);
            if(p != null) {
                if("".equals(p.value())){
                    return isa.getName().replace(".","_");
                } else {
                    return p.value();
                }
            }
        }

        for(Class<?> i : isa.getInterfaces()) {
            JSPrototype p = i.getAnnotation(JSPrototype.class);
            if(p != null) {
                if("".equals(p.value())){
                    return i.getName().replace(".","_");
                } else {
                    return p.value();
                }
            }
        }

        return getPrototype(isa.getSuperclass());
    }

    protected static String typeString(Method method) {
        StringBuffer sb = new StringBuffer();
        sb.append("(");
        for(Class<?> t : method.getParameterTypes()) {
            sb.append(typeString(t));
        }
        sb.append(")");
        sb.append(typeString(method.getReturnType()));
        return sb.toString();
    }

    protected static String typeString(Class<?> isa) {
        if(Boolean.class == isa || boolean.class == isa) {
            return "Z";
        }
        if(Byte.class == isa || byte.class == isa) {
            return "B";
        }
        if(Short.class == isa || short.class == isa) {
            return "S";
        }
        if(Integer.class == isa || int.class == isa) {
            return "I";
        }
        if(Long.class == isa || long.class == isa) {
            return "J";
        }
        if(Float.class == isa || float.class == isa) {
            return "F";
        }
        if(Double.class == isa || double.class == isa) {
            return "D";
        }
        if(Void.class == isa || void.class == isa) {
            return "V";
        }
        if(isa.isArray()) {
            return "[" + typeString(isa.getComponentType());
        }
        return "L" + isa.getName().replace(".","/") + ";";
    }

    public static void addPrototype(long appid,Class<?> isa) {
        String base = getPrototype(isa.getSuperclass());
        String name = getPrototype(isa);

        if(name == null) {
            return;
        }

        List<Object> ms = new ArrayList<>();
        List<Object> fs = new ArrayList<>();

        Field fields[] = isa.getFields();

        for(Field fd : fields) {
            JSProperty p = fd.getAnnotation(JSProperty.class);
            if(p != null) {
                if("".equals(p.name())) {
                    fs.add(fd.getName());
                } else {
                    fs.add(p.name());
                }
                fs.add(typeString(fd.getType()));
                fs.add(p.readonly());
                fs.add(fd);
            }
        }

        Method methods[] = isa.getMethods();

        for(Method m : methods) {
            JSMethod p = m.getAnnotation(JSMethod.class);
            if(p != null) {
                if("".equals(p.name())) {
                    ms.add(m.getName());
                } else {
                    ms.add(p.name());
                }
                ms.add(typeString(m));
                ms.add(m);
            }
        }

        addPrototype(appid,name,base,ms.toArray(),fs.toArray());

    }

    private final static native void addPrototype(long appid,String name,String base,Object[] methods,Object[] fields);

    public final static native void addLibrary(long appid,String name,Object object);

    public interface OpenCallback {
        void onOpen(long appid);
        void onError(String errmsg);
    }

    public static JSObject allocJSObject(long ptr) {
        return new JSObject(ptr);
    }


    public static void appendText(SpannableStringBuilder string, String text, String family, float size, boolean bold, boolean italic, int color) {

        if(text == null){
            return;
        }

        SpannableString span = new SpannableString(text);

        int length = text.length();

        span.setSpan(new AbsoluteSizeSpan((int) Math.ceil( size)), 0, length, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        span.setSpan(new ForegroundColorSpan(color), 0, length, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);

        if(bold) {
            span.setSpan(new StyleSpan(Typeface.BOLD), 0, length, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        } else if(italic) {
            span.setSpan(new StyleSpan(Typeface.ITALIC), 0, length, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
        } else if(family != null && !"".equals(family)) {
            Typeface f = Typeface.create(family,Typeface.NORMAL);
            if(f != null) {
                span.setSpan(new StyleSpan(f.getStyle()), 0, length, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
            }
        }

        string.append(span);
    }

    public static void appendImage(SpannableStringBuilder string, final Object image, final int width, final int height, final int left, int top, int right, int bottom) {

        if(image == null) {
            return ;
        }

        if(image instanceof Drawable || image instanceof Image) {

            SpannableString span = new SpannableString("...");

            span.setSpan(new DynamicDrawableSpan() {
                @Override
                public Drawable getDrawable() {
                    Drawable drawable = null;
                    if(image instanceof Drawable) {
                        drawable = (Drawable) image;
                    } else if(image instanceof Image) {
                        drawable = ((Image) image).getDrawable();
                    }
                    android.graphics.Rect bounds = drawable.getBounds();
                    if(width != 0) {
                        bounds.right = width;
                    }
                    if(height != 0) {
                        bounds.bottom = height;
                    }
                    drawable.setBounds(bounds);
                    return drawable;
                }
            },0,3,Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);

            string.append(span);
        }

    }


    public static float[] getAttributedTextSize(CharSequence string,float maxWidth) {
        TextPaint paint = new TextPaint(Paint.ANTI_ALIAS_FLAG);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            paint.setLetterSpacing(0);
        }

        StaticLayout v = new StaticLayout(
                string,
                0,
                string.length(),
                paint,
                (int) Math.ceil(maxWidth),
                Layout.Alignment.ALIGN_NORMAL,
                1.0f,
                0f,
                false);

        float[] r = new float[]{0,v.getHeight()};

        for(int i=0;i<v.getLineCount();i++) {
            float vv = v.getLineWidth(i);
            if(vv > r[0]) {
                r[0] = vv;
            }
        }

        return r;
    }


    public static KerCanvas createCanvas(int width, int height) {
        return new KerCanvas(width,height);
    }

    public static void displayCanvas(KerCanvas canvas,final long viewId,final long appid) {

        final Drawable image = canvas.copyDrawable();

        _handler.post(new Runnable() {
            @Override
            public void run() {
                if(_views.containsKey(viewId)) {
                    View view = _views.get(viewId);
                    if(view instanceof IKerView) {
                        ((IKerView) view).setImage(viewId,image,appid);
                    } else {
                        view.setBackground(image);
                    }
                }
            }
        });

    }

    protected final static void openlib(Activity activity) {
        _userAgent = WebSettings.getDefaultUserAgent(activity);
        final DisplayMetrics metrics = activity.getResources().getDisplayMetrics();
        addOpenlib(new Openlib() {
            @Override
            public void open(long appid) {

                Map<String,Object> v = new TreeMap<>();

                v.put("width", Math.min(metrics.widthPixels,metrics.heightPixels));
                v.put("height",Math.min(metrics.widthPixels,metrics.heightPixels));
                v.put("density",metrics.density);
                v.put("scale",1.0f);

                KerUI.addLibrary(appid,"screen",v);
            }
        });

        System.loadLibrary("ker");
        openlib();
    }

    private final static native void openlib();

    public final static native void open(String URI,Object query,OpenCallback callback);

    public final static native String resolveURI(String path);

    public final static native String resolveURI(String path,String basePath);

    public final static native String resolveURI(String path,long appid);

    public final static native String resolvePath(String URI);

    public final static native String resolvePath(String URI,String basePath);

    public final static native String resolvePath(String URI,long appid);

    public final static native void dispatchSetContentOffset(long appid,int x,int y,long viewId);

    public final static native void emit(long appid,String name,long viewId,Object data);

    private final static native void setImage(long id,Object image,int width,int height);



}
