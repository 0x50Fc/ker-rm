package cn.kkmofang.ker;

/**
 * Created by hailong11 on 2018/12/18.
 */

public class KerQueue extends KerObject {

    public KerQueue(long ptr) {
        super(ptr);
    }

    public void async(Runnable run) {
        if(_ptr != 0) {
            async(ptr(), run);
        }
    }

    public void sync(Runnable run) {
        if(_ptr != 0) {
            sync(ptr(), run);
        }
    }

    private static final native void async(long ptr,Runnable run);
    private static final native void sync(long ptr,Runnable run);
}
