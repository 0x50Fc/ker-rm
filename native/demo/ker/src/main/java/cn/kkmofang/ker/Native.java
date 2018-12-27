package cn.kkmofang.ker;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.os.MessageQueue;
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
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;
import android.widget.TextView;

import java.lang.*;
import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.nio.ByteBuffer;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

import cn.kkmofang.ker.image.ImageCache;

/**
 * Created by zhanghailong on 2018/12/11.
 */

public final class Native {

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

    public static String getPrototype(Object object) {
        if(object == null) {
            return null;
        }
        return getPrototype(object.getClass());
    }

    public static JSObject allocJSObject(long kerObject) {
        return new JSObject(kerObject);
    }

    public static void pushObject(long jsContext,Object object) {

        if(object == null) {
            JSContext.PushUndefined(jsContext);
        } else if(object instanceof Integer || object instanceof Short){
            JSContext.PushInt(jsContext,((Number) object).intValue());
        } else if(object instanceof Double || object instanceof Float){
            JSContext.PushNumber(jsContext,((Number) object).doubleValue());
        } else if(object instanceof Long){
            if(((Number) object).intValue() == ((Number) object).longValue()) {
                JSContext.PushInt(jsContext,((Number) object).intValue());
            } else {
                JSContext.PushString(jsContext,object.toString());
            }
        } else if(object instanceof String) {
            JSContext.PushString(jsContext, (String) object);
        } else if(object instanceof Boolean) {
            JSContext.PushBoolean(jsContext, (boolean) object);
        } else if(object instanceof byte[]) {
            JSContext.PushBytes(jsContext, (byte[]) object);
        } else if(object instanceof Iterable) {
            JSContext.PushArray(jsContext);
            int i = 0;
            for (Object v : (Iterable) object) {
                JSContext.PushInt(jsContext, i);
                pushObject(jsContext,v);
                JSContext.PutProp(jsContext, -3);
                i++;
            }
        } else if(object.getClass().isArray()) {
            JSContext.PushArray(jsContext);
            int n = Array.getLength(object);
            for (int i = 0; i < n; i++) {
                Object v = Array.get(object, i);
                JSContext.PushInt(jsContext, i);
                pushObject(jsContext,v);
                JSContext.PutProp(jsContext, -3);
            }
        } else if(object instanceof Map) {
            JSContext.PushObject(jsContext);
            Map m = (Map) object;
            for (Object key : m.keySet()) {
                JSContext.PushString(jsContext, JSContext.stringValue(key, ""));
                pushObject(jsContext, m.get(key));
                JSContext.PutProp(jsContext, -3);
            }
        } else if(object instanceof JSONString) {
            JSContext.PushJSONString(jsContext, ((JSONString) object).string);
        } else if(object instanceof JSObject) {
            JSContext.PushJSObject(jsContext,(JSObject) object);
        } else {
            String name = Native.getPrototype(object);
            if(name == null) {
                JSContext.PushObject(jsContext);
                for(Field fd : object.getClass().getFields()) {
                    try {
                        Object v = fd.get(object);
                        if(v != null) {
                            JSContext.PushString(jsContext, fd.getName());
                            Native.pushObject(jsContext, v);
                            JSContext.PutProp(jsContext,-3);
                        }
                    } catch (IllegalAccessException e) {
                        Log.e("ker",Log.getStackTraceString(e));
                    }
                }
            } else {
                JSContext.PushObject(jsContext, object,name);
            }
        }


    }

    public static Object toObject(long jsContext,int idx) {

        if(idx >= 0) {
            return null;
        }

        switch (JSContext.GetType(jsContext,idx)) {
            case JSContext.TYPE_BOOLEAN:
                return JSContext.ToBoolean(jsContext,idx);
            case JSContext.TYPE_NUMBER:
                return JSContext.ToNumber(jsContext,idx);
            case JSContext.TYPE_STRING:
                return JSContext.ToString(jsContext,idx);
            case JSContext.TYPE_BUFFER:
                return JSContext.ToBytes(jsContext,idx);
            case JSContext.TYPE_OBJECT:
                if(JSContext.IsArray(jsContext,idx)) {
                    List<Object> m = new LinkedList<>();
                    JSContext.EnumArray(jsContext,idx);
                    while(JSContext.Next(jsContext,-1,true)) {
                        Object value = toObject(jsContext,-1);
                        if(value != null) {
                            m.add(value);
                        }
                        JSContext.Pop(jsContext,2);
                    }
                    JSContext.Pop(jsContext);
                    return m;
                } else {
                    Object v = JSContext.ToObject(jsContext,idx);
                    if(v == null) {
                        Map<String,Object> m = new TreeMap<>();
                        JSContext.EnumObject(jsContext,idx);
                        while(JSContext.Next(jsContext,-1,true)) {
                            String key = JSContext.ToString(jsContext,-2);
                            Object value = toObject(jsContext,-1);
                            if(key != null && value != null) {
                                m.put(key,value);
                            }
                            JSContext.Pop(jsContext,2);
                        }
                        JSContext.Pop(jsContext);
                        return m;
                    } else {
                        return v;
                    }
                }
            case JSContext.TYPE_LIGHTFUNC:
                return JSContext.ToJSObject(jsContext,idx);
        }

        return null;
    }

    public static int getImageWidth(Object object) {
        if(object instanceof BitmapDrawable) {
            return ((BitmapDrawable) object).getBitmap().getWidth();
        }
        if(object instanceof Image) {
            return ((Image) object).getBitmap().getWidth();
        }
        return 0;
    }

    public static int getImageHeight(Object object) {
        if(object instanceof BitmapDrawable) {
            return ((BitmapDrawable) object).getBitmap().getHeight();
        }
        if(object instanceof Image) {
            return ((Image) object).getBitmap().getHeight();
        }
        return 0;
    }

    public static byte[] getImageData(Object object) {
        Bitmap bitmap = null;
        if(object instanceof BitmapDrawable) {
            bitmap = ((BitmapDrawable) object).getBitmap();
        }
        if(object instanceof Image) {
            bitmap = ((Image) object).getBitmap();
        }
        if(bitmap != null) {
            if(bitmap.getConfig() == Bitmap.Config.ARGB_8888) {
                ByteBuffer data = ByteBuffer.allocate(bitmap.getWidth() * bitmap.getHeight() * 4);
                bitmap.copyPixelsToBuffer(data);
                return data.array();
            } else {
                Bitmap b = bitmap.copy(Bitmap.Config.ARGB_8888,false);
                ByteBuffer data = ByteBuffer.allocate(bitmap.getWidth() * bitmap.getHeight() * 4);
                b.copyPixelsToBuffer(data);
                b.recycle();
                return data.array();
            }
        }
        return null;
    }

    public static Object getImageWithCache(String URI,String basePath) {
        return null;
    }

    public static void getImage(App app, long imageObject,String URI,String basePath,long queuePtr) {

        final KerObject object = new KerObject(imageObject);
        final KerQueue queue = new KerQueue(queuePtr);
        final Context context = app.activity().getApplicationContext();

        if(!URI.contains("://")) {
            URI = basePath + "/" + URI;
        }

        final Drawable image = ImageCache.getDefaultImageCache().getImageWithCache(URI);

        if(image != null) {
            queue.async(new Runnable() {
                @Override
                public void run() {
                    Native.setImage(object.ptr(),image);
                    queue.recycle();
                    object.recycle();
                }
            });
        } else {

            ImageCache.getDefaultImageCache().getImage(context, URI, new ImageCache.Callback() {

                @Override
                public void onError(Throwable ex) {

                    Log.e("ker",Log.getStackTraceString(ex));

                    queue.async(new Runnable() {
                        @Override
                        public void run() {
                            Native.setImage(object.ptr(),null);
                            queue.recycle();
                            object.recycle();
                        }
                    });
                }

                @Override
                public void onImage(final Drawable image) {

                    queue.async(new Runnable() {
                        @Override
                        public void run() {
                            Native.setImage(object.ptr(),image);
                            queue.recycle();
                            object.recycle();
                        }
                    });

                }

            });
        }
    }

    public static void viewObtain(Object view,long viewObject) {

        if(view instanceof IKerView) {
            ((IKerView) view).obtain(viewObject);
        }

    }

    public static void viewRecycle(Object view,long viewObject) {

        if(view instanceof IKerView) {
            ((IKerView) view).recycle(viewObject);
        }

    }

    public static void viewSetAttribute(Object view,long viewObject,String key,String value) {

        if(view instanceof IKerView) {
            ((IKerView) view).setAttributeValue(viewObject,key,value);
        }
    }

    public static void viewSetFrame(Object view,long viewObject,float x,float y,float width,float height) {

        if(view instanceof View) {
            Rect frame = new Rect();
            frame.x = (int) (x);
            frame.y = (int) (y);
            frame.width = (int) Math.ceil(width);
            frame.height = (int) Math.ceil(height);
            ((View) view).setTag(R.id.ker_frame,frame);
            ViewParent p = ((View) view).getParent();
            if(p != null) {
                p.requestLayout();
            }
        }
    }

    public static void viewSetContentSize(Object view,long viewObject,float width,float height) {

    }

    public static void viewSetContentOffset(Object view,long viewObject,float x,float y,boolean animated) {

        if(view instanceof View) {
            Context context = ((View) view).getContext();
            DisplayMetrics metrics = context.getResources().getDisplayMetrics();
            ((View) view).scrollTo((int) (x * metrics.density),(int) (y * metrics.density));
        }

    }

    public static float viewGetContentOffsetX(Object view,long viewObject) {

        if(view instanceof View) {
            Context context = ((View) view).getContext();
            DisplayMetrics metrics = context.getResources().getDisplayMetrics();
            return ((View) view).getScrollX() / metrics.density;
        }
        return 0;
    }

    public static float viewGetContentOffsetY(Object view,long viewObject) {

        if(view instanceof View) {
            Context context = ((View) view).getContext();
            DisplayMetrics metrics = context.getResources().getDisplayMetrics();
            return ((View) view).getScrollY() / metrics.density;
        }

        return 0;
    }

    public static void viewAddSubview(Object view,long viewObject,Object subview,int position) {

        ViewGroup contentView = null;

        if(view instanceof IKerView) {
            contentView = ((IKerView) view).contentView();
        } else if(view instanceof View) {
            contentView = ((View) view).findViewById(R.id.ker_contentView);
        }

        if(contentView == null && view instanceof ViewGroup) {
            contentView = (ViewGroup) view;
        }

        if(subview instanceof View) {
            ViewParent p = ((View) subview).getParent();
            if(p != null) {
                if(p == view) {
                    return ;
                }
                if(p instanceof ViewGroup) {
                    ((ViewGroup) p).removeView((View) subview);
                }
            }
            if(contentView != null) {
                if(position ==1) {
                    contentView.addView((View) subview,0);
                } else  {
                    contentView.addView((View) subview);
                }
            }
        }

    }

    public static void viewRemoveView(Object view,long viewObject) {

        if(view instanceof View) {
            ViewParent p = ((View) view).getParent();
            if(p != null && p instanceof ViewGroup) {
                ((ViewGroup) p).removeView((View) view);
            }
        }
    }

    public static void viewEvaluateJavaScript(Object view,long viewObject,String code) {

        if(view instanceof IKerView) {
            ((IKerView) view).evaluateJavaScript(viewObject,code);
        }
    }

    public static void appendText(SpannableStringBuilder string,String text,String family,float size,boolean bold,boolean italic,int color) {

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

    public static void viewSetAttributedText(Object view,long viewObject, CharSequence string) {
        if(view instanceof IKerView) {
            ((IKerView) view).setAttributedText(viewObject,string);
        } else if(view instanceof TextView) {
            ((TextView) view).setText(string);
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

    public static void viewSetImage(Object view,long viewObject, Object image) {

        if(view instanceof IKerView) {
            if(image instanceof Drawable) {
                ((IKerView) view).setImage((Drawable) image);
            } else {
                ((IKerView) view).setImage(null);
            }
        } else if(view instanceof View) {
            if(image instanceof Drawable) {
                ((View) view).setBackground((Drawable) image);
            } else {
                ((View) view).setBackground(null);
            }
        }

    }

    public static void viewSetGravity(Object view,long viewObject,String gravity) {

    }

    public static void viewSetContent(Object view,long viewObject,String content,String contentType,String basePath) {

        if(view instanceof IKerView) {
            ((IKerView) view).setContent(viewObject,content,contentType,basePath);
        }
    }

    public static int getViewWidth(Object view) {

        if(view instanceof View) {
            Rect frame = (Rect) ((View) view).getTag(R.id.ker_frame);
            if(frame != null) {
                return frame.width;
            }
            return Math.max(((View) view).getWidth(),((View) view).getMeasuredWidth());
        }

        return 0;
    }

    public static int getViewHeight(Object view) {

        if(view instanceof View) {
            Rect frame = (Rect) ((View) view).getTag(R.id.ker_frame);
            if(frame != null) {
                return frame.height;
            }
            return Math.max(((View) view).getHeight(),((View) view).getMeasuredHeight());
        }

        return 0;
    }

    public static Object createView(App app,String name,long viewConfiguration) {

        View view = null;

        if(name != null && _viewClasss.containsKey(name)) {

            Class<?> isa = _viewClasss.get(name);

            try {
                Constructor<?> init = isa.getConstructor(Context.class);
                view = (View ) init.newInstance(app.activity().getApplicationContext());
            } catch (Throwable e) {
                Log.d("ker",Log.getStackTraceString(e));
            }
        }

        if(view == null) {
            view = new KerView(app.activity().getApplicationContext());
        }

        if(view instanceof IKerView){
            ((IKerView) view).setViewConfiguration(viewConfiguration);
        }

        return view;
    }

    public static void runApp(final App app,String URI,final Object query) {

        Package.getPackage(app.activity(), URI, new Package.Callback() {

            @Override
            public void onError(Throwable ex) {

            }

            @Override
            public void onLoad(Package pkg) {
                App.open(app.activity(),pkg.basePath,pkg.appkey,query);
            }

            @Override
            public void onProgress(long bytes, long total) {

            }
        });
    }

    private static Map<String,Class<?>> _viewClasss = new TreeMap<>();

    public static void addViewClass(String name,Class<?> viewClass) {
        _viewClasss.put(name,viewClass);
    }

    static  {
        _viewClasss.put("UILabel",KerTextView.class);
        _viewClasss.put("UIView",KerView.class);
        _viewClasss.put("KerButton",KerButton.class);
        _viewClasss.put("WKWebView",KerWebView.class);
        _viewClasss.put("UICanvasView",KerCanvasView.class);
    }

    public static void openlib() {

        final Handler v = new Handler();

        v.post(new Runnable() {
            @Override
            public void run() {
                loop();
                v.postDelayed(this,17);
            }
        });

    }

    public static void getPackage(App app, final long ptr, String URI) {

        retain(ptr);

        Package.getPackage(app.activity(), URI, new Package.Callback() {

            @Override
            public void onError(Throwable ex) {
                Map<String,Object> data = new TreeMap<>();
                data.put("error",ex.getLocalizedMessage());
                emit(ptr,"error",data);
                release(ptr);
            }

            @Override
            public void onLoad(Package pkg) {
                emit(ptr,"load",new TreeMap<>());
                release(ptr);
            }

            @Override
            public void onProgress(long bytes, long total) {

            }
        });

    }

    public static KerCanvas createCanvas(int width, int height) {
        return new KerCanvas(width,height);
    }

    public static void displayCanvas(KerCanvas canvas,Object view) {

        if(view instanceof KerCanvasView) {
            ((KerCanvasView) view).post(canvas.getDrawable());
        } else if(view instanceof View) {
            ((View) view).setBackground(canvas.getDrawable());
        }

    }

    public static void gc() {
       System.gc();
    }

    public native static void retain(long kerObject);
    public native static void release(long kerObject);
    public native static void setImage(long imageObject,Object image);
    public native static void loop();
    public native static void emit(long ptr,String name,Object data);
    public native static WebViewConfiguration getWebViewConfiguration(long kerObject);
    public native static String absolutePath(long ptr,String path);
}
