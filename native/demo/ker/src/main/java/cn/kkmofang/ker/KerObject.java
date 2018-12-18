package cn.kkmofang.ker;

/**
 * Created by zhanghailong on 2018/12/18.
 */

public class KerObject {

    protected long _ptr;

    public KerObject(long ptr){
        _ptr = ptr;
        Native.retain(ptr);
    }

    public long ptr() {
        return _ptr;
    }

    public void recycle() {
        if(_ptr != 0) {
            Native.release(_ptr);
            _ptr = 0;
        }
    }

    protected void finalize() throws Throwable {
        if(_ptr != 0) {
            Native.release(_ptr);
            _ptr = 0;
        }
        super.finalize();
    }

}
