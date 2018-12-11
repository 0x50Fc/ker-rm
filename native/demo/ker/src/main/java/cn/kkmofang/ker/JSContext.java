package cn.kkmofang.ker;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

/**
 * Created by hailong11 on 2018/12/11.
 */

public final class JSContext {

    public static void SetPrototype(long jsContext,int idx,String name) {
        GetGlobalString(jsContext,name);
        if(IsFunction(jsContext,-1)) {
            GetPrototype(jsContext,-1);
            SetPrototype(jsContext, idx - 2);
        }
        Pop(jsContext);
    }


    public static native void PushFunction(long jsContext,JSFunction func);

    public static native void PushObject(long jsContext);

    public static native void SetPrototype(long jsContext,int idx) ;

    public static native void PushGlobalString(long jsContext,String name);

    public static native void GetGlobalString(long jsContext,String name);

    public static native boolean IsObject(long jsContext,int idx);

    public static native boolean IsFunction(long jsContext,int idx);

    public static native void Pop(long jsContext);

    public static native void GetPrototype(long jsContext,int idx);

    public static native void PutProperty(long jsContext,int idx,String name,Field field,boolean readonly);

    public static native void PutMethod(long jsContext,int idx,String name,String type,Method method);

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
                        PutProperty(jsContext,-1,fd.getName(),fd,p.readonly());
                    } else {
                        PutProperty(jsContext,-1,p.name(),fd,p.readonly());
                    }
                }
            }

            Method methods[] = isa.getMethods();

            for(Method m : methods) {
                JSMethod p = m.getAnnotation(JSMethod.class);
                if(p != null) {
                    if("".equals(p.name())) {
                        PutMethod(jsContext,-1,m.getName(),p.type(),m);
                    } else {
                        PutMethod(jsContext,-1,p.name(),p.type(),m);
                    }
                }
            }

            SetPrototype(jsContext,-2);

            PushGlobalString(jsContext,name);

        }

    }

}
