package cn.kkmofang.ker;

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

        return null;
    }

    public void set(String key,Object value) {

    }

    public Object call(Object ... args) {

        return null;
    }

    public Object invoke(String name,Object ... args) {

        return null;
    }

    protected void finalize() throws Throwable {
        Native.release(kerObject);
        super.finalize();
    }

}
