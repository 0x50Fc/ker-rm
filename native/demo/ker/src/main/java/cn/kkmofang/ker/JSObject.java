package cn.kkmofang.ker;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;

import static java.lang.reflect.Proxy.newProxyInstance;

/**
 * Created by zhanghailong on 2018/12/11.
 */

public final class JSObject {

    public final long kerObject;

    JSObject(long kerObject) {
        this.kerObject = kerObject;
        Native.retain(kerObject);
    }

    public Object get(String key) {
        return get(kerObject,key);
    }

    public void set(String key,Object value) {
        set(kerObject,key,value);
    }

    public Object call(Object ... args) {
        return call(kerObject,args);
    }

    public Object callA(Object[] args) {
        return call(kerObject,args);
    }

    public Object invoke(String name,Object ... args) {
        return invoke(kerObject,name,args);
    }

    public Object invokeA(String name,Object[] args) {
        return invoke(kerObject,name,args);
    }

    public <T extends Object> T implemenetInterface(Class<T> isa) {

        Class<?>[] interfaces;

        if(isa.isInterface()) {
            interfaces = new Class<?>[]{isa};
        } else {
            interfaces = isa.getInterfaces();
        }

        final JSObject object = this;

        return (T) Proxy.newProxyInstance(isa.getClassLoader(), interfaces, new InvocationHandler() {
            @Override
            public Object invoke(Object o, Method method, Object[] objects) throws Throwable {
                String name = method.getName();
                if(name.startsWith("get") && method.getParameterTypes().length == 0 && method.getReturnType() != Void.class) {
                    String key = name.substring(3,4).toLowerCase() + name.substring(4);
                    Object v = object.get(key);
                    if(v == null || method.getReturnType().isAssignableFrom(v.getClass())) {
                        return v;
                    }
                    return null;
                } else if(name.startsWith("set") && method.getParameterTypes().length == 1 && method.getReturnType() == Void.class && objects != null && objects.length > 0) {

                    String key = name.substring(3,4).toLowerCase() + name.substring(4);
                    Object v = objects[0];

                    if(v == null || method.getParameterTypes()[0].isAssignableFrom(v.getClass())) {
                        object.set(key,v);
                    }

                    return null;
                } else {
                    return object.invokeA(method.getName(),objects);
                }
            }
        });
    }

    protected void finalize() throws Throwable {
        Native.release(kerObject);
        super.finalize();
    }

    private final static native Object get(long ptr,String key);
    private final static native void set(long ptr,String key,Object value);
    private final static native Object call(long ptr,Object[] args);
    private final static native Object invoke(long ptr,String name,Object[] args);

}
