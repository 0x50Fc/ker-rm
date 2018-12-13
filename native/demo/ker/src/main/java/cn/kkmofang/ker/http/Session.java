package cn.kkmofang.ker.http;

import android.os.Handler;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.util.List;
import java.util.Map;
import java.util.concurrent.PriorityBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * Created by zhanghailong on 2018/12/13.
 */

public class Session {


    private final ThreadPoolExecutor _executor;
    private final PriorityBlockingQueue<Runnable> _queue;
    private long _autoId;
    public int defaultTimeout;

    public Session() {
        _autoId = 0;
        _queue = new PriorityBlockingQueue<>();
        _executor = new ThreadPoolExecutor(1,5, 6,TimeUnit.SECONDS,_queue);
    }

    private static Session _defaultSession;

    public static Session getDefaultSession() {
        if(_defaultSession == null) {
            _defaultSession = new Session();
        }
        return _defaultSession;
    }

    public SessionTask dataTask(Request req,Callback callback,int priority) {
        return new DataSessionTask(req,callback,++_autoId, priority);
    }

    public SessionTask uploadTask(Request req,Callback callback,int priority) {
        return new UploadSessionTask(req,callback,++_autoId, priority);
    }

    public SessionTask downloadTask(Request req,Callback callback,int priority) {
        return new DowonloadSessionTask(req,callback,++_autoId, priority);
    }

    private class DataSessionTask extends SessionTaskBase {

        public DataSessionTask(Request req, Callback callback, long id,int priority) {
            super(req, callback, id, priority);
        }
    }

    private class UploadSessionTask extends SessionTaskBase {

        public UploadSessionTask(Request req, Callback callback, long id,int priority) {
            super(req, callback, id, priority);
        }
    }

    private class DowonloadSessionTask extends SessionTaskBase {

        private File _tmpFile;
        private FileOutputStream _out;

        public DowonloadSessionTask(Request req, Callback callback, long id,int priority) {
            super(req, callback, id, priority);
            try {
                _tmpFile = File.createTempFile("Ker-",".download");
            } catch (IOException e) {
                onError(e);
                _queue.remove(this);
            }
        }

        protected void onResponse(Response resp) throws Throwable{
            _out = new FileOutputStream(_tmpFile);
            super.onResponse(resp);
        }

        protected void onError(Throwable ex) {
            if(_out != null) {
                try {
                    _out.close();
                } catch (IOException e) {
                }
            }
            super.onError(ex);
        }

        protected void onSuccess() throws Throwable {
            if(_out != null) {
                try {
                    _out.close();
                } catch (IOException e) {
                }
            }
            if(_cb != null) {
                _cb.onDownload(_tmpFile);
            }
            super.onSuccess();
            _tmpFile.delete();
        }

        protected void onData(byte[] data,int offset,int length) throws Throwable {

            if(_out != null) {
                _out.write(data,offset,length);
            }

            super.onData(data,offset,length);
        }
    }

    private class SessionTaskBase implements SessionTask ,Runnable, Comparable<SessionTaskBase>  {

        protected final long _id;
        protected final Request _req;
        protected final Callback _cb;
        protected final int _priority;

        public SessionTaskBase(Request req,Callback callback,long id,int priority) {
            _id = id;
            _req = req;
            _cb = callback;
            _priority = priority;
            _executor.execute(this);
        }



        @Override
        public long id() {
            return _id;
        }

        @Override
        public void cancel() {
            _executor.remove(this);
        }

        protected void onError(Throwable ex) {

            if(_cb != null) {
                _cb.onError(ex);
            }
        }

        protected void onResponse(Response resp) throws Throwable {

            if(_cb != null) {
                _cb.onResponse(resp);
            }

        }

        protected void onData(byte[] data,int offset,int length) throws Throwable {
            if(_cb != null) {
                _cb.onData(data,offset,length);
            }
        }

        protected void onSuccess() throws Throwable {
            if(_cb != null) {
                _cb.onSuccess();
            }
        }

        @Override
        public void run() {

            try {

                URL u;

                if ("POST".equals(_req.method) || "PUT".equals(_req.method)) {
                    u = new URL(_req.url);
                } else {

                    StringBuffer sb = new StringBuffer();
                    sb.append(_req.url);

                    if(_req.data != null) {

                        if(_req.data instanceof Map) {

                            String dot;

                            if(_req.url.endsWith("?") || _req.url.contains("&")) {
                                dot = "";
                            } else if(_req.url.contains("?")) {
                                dot = "&";
                            } else {
                                dot = "?";
                            }

                            Map<?,?> m = (Map<?,?>) _req.data;

                            for(Object key : m.keySet()) {
                                sb.append(dot);
                                dot = "&";
                                sb.append(stringValue(key,""));
                                sb.append("=");
                                sb.append(URLEncoder.encode(stringValue(m.get(key),""),"UTF-8"));
                            }
                        }

                    }
                    u = new URL(sb.toString());
                }


                int timeout = _req.timeout;

                if(timeout == 0) {
                    timeout = defaultTimeout;
                }

                if(timeout == 0) {
                    timeout = 60 * 1000;
                }

                HttpURLConnection conn = (HttpURLConnection) u.openConnection();

                conn.setRequestMethod(_req.method);
                conn.setConnectTimeout(timeout);
                conn.setUseCaches(false);

                if("POST".equals(_req.method) || "PUT".equals(_req.method)) {
                    if(_req.data != null ) {
                        if(_req.data instanceof FormData) {
                            _req.setHeader("Content-Type",((FormData)_req.data).contentType());
                        } else if(_req.data instanceof Map) {
                            _req.setHeader("Content-Type","application/x-www-form-urlencoded");
                        }
                    }
                }

                for(Header v : _req.getHeaders()) {
                    conn.addRequestProperty(v.key,v.value);
                }

                if("POST".equals(_req.method) || "PUT".equals(_req.method)) {

                    if(_req.data != null) {
                        conn.setDoOutput(true);
                    } else {
                        conn.setDoOutput(false);
                    }

                } else {
                    conn.setDoOutput(false);
                }

                conn.connect();

                if("POST".equals(_req.method) || "PUT".equals(_req.method)) {

                    if(_req.data != null) {

                        OutputStream out = conn.getOutputStream();

                        try {

                            if (_req.data instanceof byte[]) {
                                out.write((byte[]) _req.data);
                            } else if (_req.data instanceof String) {
                                out.write(((String) _req.data).getBytes("UTF-8"));
                            } else if (_req.data instanceof FormData) {
                                out.write(((FormData) _req.data).content());
                            } else if(_req.data instanceof Map) {
                                StringBuffer sb = new StringBuffer();
                                String dot = "";
                                Map<?,?> m = (Map<?,?>) _req.data;

                                for(Object key : m.keySet()) {
                                    sb.append(dot);
                                    dot = "&";
                                    sb.append(stringValue(key,""));
                                    sb.append("=");
                                    sb.append(URLEncoder.encode(stringValue(m.get(key),""),"UTF-8"));
                                }

                                out.write(sb.toString().getBytes("UTF-8"));
                            }

                            out.flush();

                        } finally {
                            out.close();
                        }


                    }

                }

                Response resp = new Response(conn.getResponseCode());

                Map<String,List<String>> m = conn.getHeaderFields();

                for(String key : m.keySet()) {

                    if(key == null) {
                        continue;
                    }

                    for(String value : m.get(key)) {
                        resp.addHeader(key,value);
                    }

                }

                onResponse(resp);

                InputStream in = conn.getInputStream();

                try {

                    byte[] data = new byte[2048];
                    int n;

                    while((n = in.read(data)) >0) {
                        onData(data,0,n);
                    }
                }
                finally {
                    in.close();
                }

                onSuccess();

            }
            catch (Throwable ex) {
                onError(ex);
            }
        }

        public String stringValue(Object v,String defaultValue) {
            if(v != null) {
                if(v instanceof String) {
                    return (String)v;
                }
                return v.toString();
            }
            return defaultValue;
        }

        @Override
        public int compareTo(SessionTaskBase sessionTaskBase) {
            if(_priority > sessionTaskBase._priority) {
                return -1;
            } else if(_priority == sessionTaskBase._priority) {
                return 0;
            }
            return 1;
        }
    }
}
