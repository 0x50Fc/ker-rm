package cn.kkmofang.ker;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by hailong11 on 2018/12/11.
 */

public final class JSContext {

    public static final int TYPE_NONE                     = 0 ;   /* no value, e.g. invalid index */
    public static final int TYPE_UNDEFINED                = 1 ;   /* Ecmascript undefined */
    public static final int TYPE_NULL                     = 2 ;   /* Ecmascript null */
    public static final int TYPE_BOOLEAN                  = 3 ;   /* Ecmascript boolean: 0 or 1 */
    public static final int TYPE_NUMBER                   = 4 ;   /* Ecmascript number: double */
    public static final int TYPE_STRING                   = 5 ;   /* Ecmascript string: CESU-8 / extended UTF-8 encoded */
    public static final int TYPE_OBJECT                   = 6 ;   /* Ecmascript object: includes objects, arrays, functions, threads */
    public static final int TYPE_BUFFER                   = 7 ;   /* fixed or dynamic, garbage collected byte buffer */
    public static final int TYPE_POINTER                  = 8 ;   /* raw void pointer */
    public static final int TYPE_LIGHTFUNC                = 9 ;   /* lightweight function pointer */


    public static final Charset UTF8 = Charset.forName("UTF-8");

    public interface Openlib {
        void open(long jsContext);
    }

    private static List<Openlib> _openlibs = new ArrayList<>();

    public static void addOpenlib(Openlib openlib){
        _openlibs.add(openlib);
    }

    public static void openlib(long jsContext) {
        for(Openlib v : _openlibs) {
            v.open(jsContext);
        }
    }

    public static void SetPrototype(long jsContext,int idx,String name) {
        GetGlobalString(jsContext,name);
        if(IsFunction(jsContext,-1)) {
            GetPrototype(jsContext,-1);
            SetPrototype(jsContext, idx - 2);
        }
        Pop(jsContext);
    }

    public static void PushObject(long jsContext,Object object) {

        if(object == null) {
            PushUndefined(jsContext);
        } else {
            PushObject(jsContext,object,Native.getPrototype(object));
        }

    }

    public static String stringValue(Object v,String defaultValue) {

        if(v != null) {
            if(v instanceof String) {
                return (String) v;
            }
            return v.toString();
        }

        return defaultValue;
    }

    public static native void PushUndefined(long jsContext);

    public static native void PushInt(long jsContext,int v);

    public static native void PushNumber(long jsContext,double v);

    public static native void PushBoolean(long jsContext,boolean v);

    public static native void PushString(long jsContext,String v);

    public static native void PushBytes(long jsContext,byte[] v);

    public static native void PushArray(long jsContext);

    public static native void PutProp(long jsContext,int idx);

    public static native void PushFunction(long jsContext,JSFunction func);

    public static native void PushObject(long jsContext);

    public static native void PushObject(long jsContext,Object object,String prototype);

    public static native void PushJSONString(long jsContext,String string);

    private static native void PushJSObject(long jsContext,long jsObject);

    public static void PushJSObject(long jsContext,JSObject jsObject) {
        if(jsObject == null ||jsObject.kerObject == 0) {
            PushUndefined(jsContext);
        } else {
            PushJSObject(jsContext, jsObject.kerObject);
        }
    }

    public static native void SetPrototype(long jsContext,int idx) ;

    public static native void PutGlobalString(long jsContext,String name);

    public static native void GetGlobalString(long jsContext,String name);

    public static native boolean IsObject(long jsContext,int idx);

    public static native boolean IsFunction(long jsContext,int idx);

    public static native boolean IsString(long jsContext,int idx);

    public static native boolean IsNumber(long jsContext,int idx);

    public static native boolean IsBoolean(long jsContext,int idx);

    public static native boolean IsArray(long jsContext,int idx);

    public static native void EnumObject(long jsContext,int idx);

    public static native void EnumArray(long jsContext,int idx);

    public static native boolean Next(long jsContext,int idx,boolean hasValue);

    public static native void Pop(long jsContext);

    public static native void Pop(long jsContext,int n);

    public static native void GetPrototype(long jsContext,int idx);

    public static native void PutProperty(long jsContext,int idx,String name,String type,Field field,boolean readonly);

    public static native void PutMethod(long jsContext,int idx,String name,String type,Method method);

    public static native int GetType(long jsContext,int idx);

    public static native int GetTop(long jsContext);

    public static native int ToInt(long jsContext,int idx);

    public static native double ToNumber(long jsContext,int idx);

    public static native boolean ToBoolean(long jsContext,int idx);

    public static native Object ToObject(long jsContext,int idx);

    public static native JSObject ToJSObject(long jsContext,int idx);

    public static native byte[] ToBytes(long jsContext,int idx);

    public static String ToString(long jsContext,int idx) {
        byte[] data = ToBytes(jsContext,idx);
        if(data != null) {
            return new String(data,UTF8);
        }
        return null;
    }

    public static String typeString(Class<?> isa) {
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

    public static String typeString(Method method) {
        StringBuffer sb = new StringBuffer();
        sb.append("(");
        for(Class<?> t : method.getParameterTypes()) {
            sb.append(typeString(t));
        }
        sb.append(")");
        sb.append(typeString(method.getReturnType()));
        return sb.toString();
    }

    public static void PushPrototype(long jsContext,Class<?> isa) {

        String name = Native.getPrototype(isa);

        if(name != null) {

            PushFunction(jsContext, new JSFunction() {
                @Override
                public int invoke(long jsContext) {
                    return 0;
                }
            });

            PushObject(jsContext);

            String base = Native.getPrototype(isa.getSuperclass());

            if(base != null) {
                SetPrototype(jsContext,-1,base);
            }

            Field fields[] = isa.getFields();

            for(Field fd : fields) {
                JSProperty p = fd.getAnnotation(JSProperty.class);
                if(p != null) {
                    if("".equals(p.name())) {
                        PutProperty(jsContext,-1,fd.getName(),typeString(fd.getType()),fd,p.readonly());
                    } else {
                        PutProperty(jsContext,-1,p.name(),typeString(fd.getType()),fd,p.readonly());
                    }
                }
            }

            Method methods[] = isa.getMethods();

            for(Method m : methods) {
                JSMethod p = m.getAnnotation(JSMethod.class);
                if(p != null) {
                    if("".equals(p.name())) {
                        PutMethod(jsContext,-1,m.getName(),typeString(m),m);
                    } else {
                        PutMethod(jsContext,-1,p.name(),typeString(m),m);
                    }
                }
            }

            SetPrototype(jsContext,-2);

            PutGlobalString(jsContext,name);

        }

    }

}
