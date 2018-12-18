package cn.kkmofang.ker.image;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.telecom.Call;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.logging.Handler;

import cn.kkmofang.ker.Cyrpto;
import cn.kkmofang.ker.http.Callback;
import cn.kkmofang.ker.http.Request;
import cn.kkmofang.ker.http.Response;
import cn.kkmofang.ker.http.Session;


/**
 * Created by zhanghailong on 2018/12/18.
 */

public class ImageCache {

    private Map<String,WeakReference<Drawable>> _images = new TreeMap<>();
    private Map<String,List<Callback>> _loadings = new TreeMap<>();

    public Drawable getImageWithCache(String URI) {
        synchronized (_images) {
            if(_images.containsKey(URI)) {
                WeakReference<Drawable> v = _images.get(URI);
                Drawable image = v.get();
                if(image == null) {
                    _images.remove(URI);
                }
                return image;
            }
        }
        return null;
    }

    private static ImageCache _defaultImageCache = new ImageCache();

    public static ImageCache getDefaultImageCache() {
        return _defaultImageCache;
    }

    public void getImage(final Context context , final String URI, final Callback callback) {

        synchronized (_loadings) {
            if(_loadings.containsKey(URI)) {
                _loadings.get(URI).add(callback);
                return;
            }
        }

        final File path;

        if(URI.startsWith("http://") || URI.startsWith("https://")) {
            path = new File(context.getCacheDir(),"/ker-image/" + Cyrpto.MD5(URI));
        } else if(URI.contains("://")) {
            callback.onError(new Exception("错误的URI " + URI));
            return;
        } else {
            path = new File(URI);
        }

        if(path.exists()) {
            Session.getDefaultSession().execute(new Runnable() {
                @Override
                public void run() {
                    Drawable image =  Drawable.createFromPath(path.getAbsolutePath());
                    if(image == null) {
                        callback.onError(new Exception("错误的图片 " +URI));
                    } else {
                        callback.onImage(image);
                    }
                }
            });
        } else if(URI.startsWith("http://") || URI.startsWith("https://")){
            synchronized (_loadings) {
                List<Callback> cbs = new ArrayList<>();
                cbs.add(callback);
                _loadings.put(URI,cbs);
            }

            Session.getDefaultSession().downloadTask(new Request(URI), new cn.kkmofang.ker.http.Callback() {

                @Override
                public void onResponse(Response resp) throws Throwable {

                }

                @Override
                public void onData(byte[] data, int offset, int length) throws Throwable {

                }

                @Override
                public void onDownload(File tempFile) {

                    List<Callback> cbs = null;

                    synchronized (_loadings) {
                        if(_loadings.containsKey(URI)) {
                            cbs = _loadings.get(URI);
                        }
                    }

                    tempFile.renameTo(path);

                    Drawable image =  Drawable.createFromPath(path.getAbsolutePath());

                    if(image != null) {
                        synchronized (_images) {
                            _images.put(URI,new WeakReference<>(image));
                        }
                    }

                    if(cbs != null) {
                        for(Callback cb : cbs) {
                            if(image == null) {
                                cb.onError(new Exception("错误的图片 " +URI));
                            } else {
                                cb.onImage(image);
                            }
                        }
                    }
                }

                @Override
                public void onSuccess() {

                }

                @Override
                public void onError(Throwable ex) {

                    List<Callback> cbs = null;

                    synchronized (_loadings) {
                        if(_loadings.containsKey(URI)) {
                            cbs = _loadings.get(URI);
                        }
                    }

                    if(cbs != null) {
                        for(Callback cb : cbs) {
                            cb.onError(ex);
                        }
                    }
                }
            });

        } else {
            callback.onError(new Exception("图片不存在 " + URI));
        }

    }

    public interface Callback {

        void onError(Throwable ex);

        void onImage(Drawable image);

    }
}
