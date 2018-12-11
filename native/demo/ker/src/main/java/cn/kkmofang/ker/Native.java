package cn.kkmofang.ker;

/**
 * Created by zhanghailong on 2018/12/11.
 */

public final class Native {

    public static String getPrototype(Class<?> isa) {
        while(isa != Object.class) {
            JSPrototype p = isa.getAnnotation(JSPrototype.class);
            if(p != null) {
                if("".equals(p.value())){
                    return isa.getName().replace(".","_");
                } else {
                    return p.value();
                }
            }
            isa = isa.getSuperclass();
        }
        return null;
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

    }

    public static Object toObject(long jsContext,int idx) {
        return null;
    }


    public native static void retain(long kerObject);
    public native static void release(long kerObject);

}
