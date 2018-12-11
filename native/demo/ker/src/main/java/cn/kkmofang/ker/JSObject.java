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

    protected void finalize() throws Throwable {
        Native.release(kerObject);
        super.finalize();
    }

}
