package cn.kkmofang.ker.http;

import java.io.File;

/**
 * Created by hailong11 on 2018/12/13.
 */

public interface Callback {

    void onResponse(Response resp) throws Throwable;

    void onData(byte[] data,int offset,int length) throws Throwable;

    void onDownload(File tempFile);

    void onSuccess();

    void onError(Throwable ex);

}
