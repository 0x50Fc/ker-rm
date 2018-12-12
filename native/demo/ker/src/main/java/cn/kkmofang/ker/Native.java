package cn.kkmofang.ker;

import android.graphics.drawable.BitmapDrawable;

import java.lang.reflect.Array;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

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
            for(Object key : m.keySet()) {
                JSContext.PushString(jsContext,JSContext.stringValue(key,""));
                pushObject(jsContext,m.get(key));
                JSContext.PutProp(jsContext,-3);
            }
        } else {
            JSContext.PushObject(jsContext,object);
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

    public static Object getImageWithCache(String URI,String basePath) {
        return null;
    }

    public static void getImage(long imageObject,String URI,String basePath) {

    }

    public static void viewObtain(Object view,long viewObject) {

    }

    public static void viewRecycle(Object view,long viewObject) {

    }

    public static void viewSetAttribute(Object view,long viewObject,String key,String value) {

    }

    public static void viewSetFrame(Object view,long viewObject,float x,float y,float width,float height) {

    }

    public static void viewSetContentSize(Object view,long viewObject,float width,float height) {

    }

    public static void viewSetContentOffset(Object view,long viewObject,float x,float y,boolean animated) {

    }

    public static float viewGetContentOffsetX(Object view,long viewObject) {
        return 0;
    }

    public static float viewGetContentOffsetY(Object view,long viewObject) {
        return 0;
    }

    public static void viewAddSubview(Object view,long viewObject,Object subview,int position) {

    }

    public static void viewRemoveView(Object view,long viewObject) {

    }

    public static void viewEvaluateJavaScript(Object view,long viewObject,String code) {

    }

    public static void viewSetAttributedText(Object view,long viewObject, long textObject) {

    }

    public static void viewSetImage(Object view,long viewObject, Object image) {

    }

    public static void viewSetGravity(Object view,long viewObject,String gravity) {

    }

    public static void viewSetContent(Object view,long viewObject,String content,String contentType,String basePath) {

    }

    public static int getViewWidth(Object view) {
        return 0;
    }

    public static int getViewHeight(Object view) {
        return 0;
    }

    public static Object createView(App app,String name,long viewConfiguration) {
        return null;
    }

    public static void runPackage(App app,String basePath,String appkey) {

    }

    public native static void retain(long kerObject);
    public native static void release(long kerObject);
    public native static void setImage(long imageObject,Object image);
}
