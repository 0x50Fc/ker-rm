package cn.kkmofang.ker;

import java.io.File;
import java.util.Map;

import cn.kkmofang.ker.http.Callback;
import cn.kkmofang.ker.http.Request;
import cn.kkmofang.ker.http.Response;
import cn.kkmofang.ker.http.Session;
import cn.kkmofang.ker.http.SessionTask;

/**
 * Created by zhanghailong on 2019/1/4.
 */

public final class KerHttp {

    public static SessionTask send(final long id, String url, String method, Map<String,String> headers, byte[] data) {

        Request req = new Request(url);
        req.data = data;
        req.method = method;

        if(headers != null) {
            for(String key : headers.keySet()){
                String v = headers.get(key);
                req.addHeader(key,v);
            }
        }

        return Session.getDefaultSession().dataTask(req, new Callback() {

            @Override
            public void onResponse(Response resp) throws Throwable {
                KerHttp.onResponse(id,resp.code,resp.getHeaderMap());
            }

            @Override
            public void onData(byte[] data, int offset, int length) throws Throwable {
                KerHttp.onData(id,data,offset,length);
            }

            @Override
            public void onDownload(File tempFile) {

            }

            @Override
            public void onSuccess() {
                KerHttp.onDone(id);
            }

            @Override
            public void onError(Throwable ex) {
                KerHttp.onError(id,ex.getLocalizedMessage());
            }
        });

    }

    private final static native void onResponse(long id,int statusCode,Map<String,String> headers);
    private final static native void onData(long id,byte[] data,int offset,int size);
    private final static native void onError(long id,String errmsg);
    private final static native void onDone(long id);

}
