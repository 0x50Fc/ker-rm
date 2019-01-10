package cn.kkmofang.ker;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;

import static java.lang.reflect.Proxy.newProxyInstance;

/**
 * Created by zhanghailong on 2018/12/11.
 */

public class JSObject extends Object implements InvocationHandler {

    protected long _ptr;

    JSObject(long ptr) {
        _ptr = ptr;
        retain(_ptr);
    }

    public long ptr() {
        return _ptr;
    }

    public Object get(String key) {
        if(_ptr == 0) {
            return null;
        }
        return get(_ptr,key);
    }

    public void set(String key,Object value) {
        if(_ptr == 0) {
            return;
        }
        set(_ptr,key,value);
    }

    public Object call(Object ... args) {
        if(_ptr == 0) {
            return null;
        }
        return call(_ptr,args);
    }

    public Object callA(Object[] args) {
        if(_ptr == 0) {
            return null;
        }
        return call(_ptr,args);
    }

    public Object invoke(String name,Object ... args) {
        if(_ptr == 0) {
            return null;
        }
        return invoke(_ptr,name,args);
    }

    public Object invokeA(String name,Object[] args) {
        if(_ptr == 0) {
            return null;
        }
        return invoke(_ptr,name,args);
    }

    public <T extends Object> T implemenetInterface(Class<T> isa) {

        Class<?>[] interfaces;

        if(isa.isInterface()) {
            interfaces = new Class<?>[]{isa};
        } else {
            interfaces = isa.getInterfaces();
        }

        return (T) Proxy.newProxyInstance(isa.getClassLoader(), interfaces, this);
    }

    public void recycle() {
        if(_ptr != 0) {
            release(_ptr);
            _ptr = 0;
        }
    }

    protected void finalize() throws Throwable {
        recycle();
        super.finalize();
    }

    private final static native void retain(long ptr);
    private final static native void release(long ptr);
    private final static native Object get(long ptr,String key);
    private final static native void set(long ptr,String key,Object value);
    private final static native Object call(long ptr,Object[] args);
    private final static native Object invoke(long ptr,String name,Object[] args);

    @Override
    public Object invoke(Object o, Method method, Object[] objects) throws Throwable {
        String name = method.getName();
        if(name.startsWith("get") && method.getParameterTypes().length == 0 && method.getReturnType() != Void.class) {
            String key = name.substring(3,4).toLowerCase() + name.substring(4);
            Object v = get(key);
            if(v == null || method.getReturnType().isAssignableFrom(v.getClass())) {
                return v;
            }
            return null;
        } else if(name.startsWith("set") && method.getParameterTypes().length == 1 && method.getReturnType() == Void.class && objects != null && objects.length > 0) {

            String key = name.substring(3,4).toLowerCase() + name.substring(4);
            Object v = objects[0];

            if(v == null || method.getParameterTypes()[0].isAssignableFrom(v.getClass())) {
                set(key,v);
            }

            return null;
        } else {
            return invokeA(method.getName(),objects);
        }
    }
}
