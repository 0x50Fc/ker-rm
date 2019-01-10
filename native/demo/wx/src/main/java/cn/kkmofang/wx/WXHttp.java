package cn.kkmofang.wx;

import android.os.Handler;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.Map;

import cn.kkmofang.ker.JSONString;
import cn.kkmofang.ker.Ker;
import cn.kkmofang.ker.http.Callback;
import cn.kkmofang.ker.http.Request;
import cn.kkmofang.ker.http.Response;
import cn.kkmofang.ker.http.Session;

/**
 * Created by zhanghailong on 2018/12/19.
 */

public class WXHttp {

    public WXRequestTask request(final WXRequestObject object) {

        String URI = object.getUrl();

        if(URI == null) {
            object.fail("Not Found URL");
            object.complete();
            return null;
        }

        String method = object.getMethod();

        if(method == null) {
            method = "GET";
        }

        final Object data = object.getData();

        if("GET".equals(method)) {
            StringBuffer sb = new StringBuffer();
            sb.append(URI);
            String dot;
            if(URI.endsWith("?")) {
                dot = "";
            } else if(URI.contains("?")) {
                dot = "&";
            } else {
                dot = "?";
            }

            if(data instanceof Map) {
                Map<?,?> m = (Map<?,?>) data;
                for(Object key : m.keySet()) {
                    Object value = m.get(key);
                    if(key != null && value != null) {
                        sb.append(dot);
                        dot = "&";
                        sb.append(Ker.stringValue(key,""));
                        sb.append("=");
                        sb.append(Ker.encodeURL(Ker.stringValue(value,"")));
                    }
                }
            }

            URI = sb.toString();
        }

        final Request req = new Request(URI);

        req.method = method;

        final Object header = object.getHeader();

        if(header != null && header instanceof Map) {
            Map<?,?> m = (Map<?,?>) data;
            for(Object key : m.keySet()) {
                Object value = m.get(key);
                if(key != null && value != null) {
                    req.addHeader(Ker.stringValue(key,""),Ker.stringValue(value,""));
                }
            }
        }

        if(!"GET".equals(method)) {

            String contentType = req.getHeaderValue("Content-Type");

            if(data != null) {

                if(data instanceof Map) {

                    if("json".equals(contentType)) {
                        req.data = Ker.encodeJSON(data);
                    } else {
                        StringBuffer sb = new StringBuffer();
                        String dot = "";
                        Map<?,?> m = (Map<?,?>) data;
                        for(Object key : m.keySet()) {
                            Object value = m.get(key);
                            if(key != null && value != null) {
                                sb.append(dot);
                                dot = "&";
                                sb.append(Ker.stringValue(key,""));
                                sb.append("=");
                                sb.append(Ker.encodeURL(Ker.stringValue(value,"")));
                            }
                        }
                        req.data = sb.toString();
                    }

                } else if(data instanceof String) {
                    req.data = data;
                } else if(data instanceof byte[]) {
                    req.data = data;
                }

            }
        }

        final WXRequestTask task = new WXRequestTask();
        final String dataType = object.getDataType();

        task.setTask(Session.getDefaultSession().dataTask(req, new Callback() {

            protected String contentType;
            protected ByteArrayOutputStream _out;
            protected WXRequestRes _res;
            @Override
            public void onResponse(Response resp) throws Throwable {
                _res = new WXRequestRes();
                _out = new ByteArrayOutputStream();
                _res.statusCode = resp.code;
                _res.headers = req.getHeaderMap();
                contentType = resp.getHeaderValue("Content-Type");

                final Map<String,String> header = _res.headers;

                task.onResponseHeader(header);

            }

            @Override
            public void onData(byte[] data, int offset, int length) throws Throwable {
                _out.write(data,offset,length);
            }

            @Override
            public void onDownload(File tempFile) {

            }

            @Override
            public void onSuccess() {

                try {
                    _out.flush();
                } catch (IOException e) {
                }

                String encoding = "UTF-8";

                if(contentType != null && (contentType.contains("charset=gbk") || contentType.contains("charset=gb2312"))) {
                    encoding = "GBK";
                }

                if("json".equals(dataType)) {
                    try {
                        _res.data = new JSONString(new String(_out.toByteArray(),encoding));
                    } catch (UnsupportedEncodingException e) {
                        Log.e("ker",Log.getStackTraceString(e));
                    }
                } else if("arraybuffer".equals(dataType)) {
                    _res.data = _out.toByteArray();
                } else {
                    try {
                        _res.data = new String(_out.toByteArray(),encoding);
                    } catch (UnsupportedEncodingException e) {
                        Log.e("ker",Log.getStackTraceString(e));
                    }
                }

                final WXRequestRes res = _res;

                object.success(res);
                object.complete();

            }

            @Override
            public void onError(Throwable ex) {
                final String errmsg = ex.getLocalizedMessage();
                object.fail(errmsg);
                object.complete();
            }

        }));

        return task;
    }

}
