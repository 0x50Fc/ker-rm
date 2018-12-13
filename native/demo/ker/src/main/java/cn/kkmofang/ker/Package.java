package cn.kkmofang.ker;

import android.content.Context;
import android.telecom.Call;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import cn.kkmofang.ker.http.Request;
import cn.kkmofang.ker.http.Response;
import cn.kkmofang.ker.http.Session;

/**
 * Created by zhanghailong on 2018/12/13.
 */

public class Package {

    public final String URI;
    public final String basePath;
    public final String appkey;

    private Package(String URI,String basePath,String appkey) {
        this.URI = URI;
        this.basePath = basePath;
        this.appkey = appkey;
    }

    public static Package getPackage(Context context, String URI) {

        if(URI == null) {
            return null;
        }

        String basePath;
        String appkey;

        if(URI.startsWith("ker-tmp://")) {
            basePath = context.getCacheDir().getAbsolutePath() + "/ker-tmp/" + URI.substring(10);
            appkey = Cyrpto.MD5(basePath);
        } else if(URI.startsWith("ker-data://")) {
            basePath = context.getDir("ker", Context.MODE_PRIVATE).getAbsolutePath() + "/" + URI.substring(11);
            appkey = Cyrpto.MD5(basePath);
        } else if(URI.startsWith("ker-app://")) {
            basePath = context.getDir("ker-app", Context.MODE_PRIVATE).getAbsolutePath() + "/" + URI.substring(10);
            appkey = Cyrpto.MD5(basePath);
        } else if(URI.startsWith("http://") || URI.startsWith("https://")) {
            appkey = Cyrpto.MD5(URI);
            basePath = context.getDir("ker", Context.MODE_PRIVATE).getAbsolutePath() + "/" + appkey;
        } else if(URI.contains("://")) {
            return null;
        } else {
            appkey = Cyrpto.MD5(URI);
            basePath = URI;
        }

        return new Package(URI,basePath,appkey);
    }

    private static Map<String,List<Callback>> _loadings = new TreeMap<>();

    public static void getPackage(Context context, String URI, Callback callback) {

        final Package pkg = getPackage(context,URI);

        if(pkg == null) {
            callback.onError(new Exception("错误的应用包地址 " + URI));
            return;
        }

        synchronized (_loadings) {

            if(_loadings.containsKey(pkg.appkey)) {
                _loadings.get(pkg).add(callback);
                return;
            }
        }

        File file = new File(pkg.basePath);

        if(file.exists() && file.isDirectory()) {
            callback.onLoad(pkg);
        } else if(URI.startsWith("http://") || URI.startsWith("https://")) {

            synchronized (_loadings) {
                List<Callback> cbs = new ArrayList<>();
                cbs.add(callback);
                _loadings.put(pkg.appkey,cbs);
            }

            Session.getDefaultSession().downloadTask(new Request(URI), new cn.kkmofang.ker.http.Callback() {

                private long _bytes;
                private long _length;

                @Override
                public void onResponse(Response resp) {
                    String v = resp.getHeaderValue("Content-Length");
                    if(v == null) {
                        _length = -1;
                    } else {
                        try {
                            _length = Long.valueOf(v);
                        }
                        catch(Throwable e) {
                            _length = -1;
                        }
                    }
                }

                @Override
                public void onData(byte[] data, int offset, int length) {

                    _bytes += length;

                    Callback[] cbs = null;

                    synchronized (_loadings) {
                        if(_loadings.containsKey(pkg.appkey)) {
                            List<Callback> v = _loadings.get(pkg.appkey);
                            cbs = v.toArray(new Callback[v.size()]);
                        }
                    }

                    if(cbs != null) {
                        for(Callback cb : cbs) {
                            cb.onProgress(_bytes,_length);
                        }
                    }
                }

                public void upZip(File zipFile, File toDir) throws IOException {

                    ZipFile zfile= new ZipFile(zipFile);

                    try {

                        Enumeration<? extends ZipEntry> zList = zfile.entries();

                        ZipEntry entry;

                        byte[] data = new byte[204800];
                        int n;

                        while (zList.hasMoreElements()) {

                            entry = zList.nextElement();

                            if (entry.isDirectory()) {
                                File f = new File(toDir, entry.getName());
                                f.mkdirs();
                                continue;
                            }

                            FileOutputStream out = new FileOutputStream(new File(toDir, entry.getName()));

                            try {

                                InputStream in = zfile.getInputStream(entry);

                                try {
                                    while((n = in.read(data)) > 0) {
                                        out.write(data,0,n);
                                    }
                                }
                                finally {
                                    in.close();
                                }
                            }
                            finally {
                                out.close();
                            }

                        }
                    }
                    finally {
                        zfile.close();
                    }

                }

                @Override
                public void onDownload(File tempFile) {
                    File dir = new File(pkg.basePath + "-unzip/");
                    dir.mkdirs();
                    try {
                        upZip(tempFile,dir);
                        dir.renameTo(new File(pkg.basePath));

                        Callback[] cbs = null;

                        synchronized (_loadings) {
                            if(_loadings.containsKey(pkg.appkey)) {
                                List<Callback> v = _loadings.remove(pkg.appkey);
                                cbs = v.toArray(new Callback[v.size()]);
                            }
                        }

                        if(cbs != null) {
                            for(Callback cb : cbs) {
                                cb.onLoad(pkg);
                            }
                        }

                    } catch (IOException e) {
                        dir.delete();
                        onError(new Exception("错误的应用包",e));
                    }
                }

                @Override
                public void onSuccess() {

                }

                @Override
                public void onError(Throwable ex) {

                    Callback[] cbs = null;

                    synchronized (_loadings) {
                        if(_loadings.containsKey(pkg.appkey)) {
                            List<Callback> v = _loadings.remove(pkg.appkey);
                            cbs = v.toArray(new Callback[v.size()]);
                        }
                    }

                    if(cbs != null) {
                        for(Callback cb : cbs) {
                            cb.onError(ex);
                        }
                    }

                }

            },0);

        } else {
            callback.onError(new Exception("未找到应用包 " + URI));
        }
    }

    public interface Callback {

        void onError(Throwable ex);

        void onLoad(Package pkg);

        void onProgress(long bytes,long total);

    }

}
