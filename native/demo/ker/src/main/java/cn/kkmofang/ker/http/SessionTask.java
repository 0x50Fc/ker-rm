package cn.kkmofang.ker.http;

/**
 * Created by zhanghailong on 2018/12/13.
 */

public interface SessionTask {
    long id();
    void cancel();
}
