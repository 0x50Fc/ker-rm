package cn.kkmofang.ker;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.view.ViewGroup;
import android.webkit.JavascriptInterface;
import android.webkit.WebChromeClient;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.lang.ref.WeakReference;
import java.util.Map;
import java.util.TreeMap;
import java.util.regex.Pattern;

/**
 * Created by zhanghailong on 2018/12/17.
 */

public class KerWebView extends WebView implements IKerView {

    static {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            WebView.setWebContentsDebuggingEnabled(true);
        }
    }

    public KerWebView(Context context) {
        super(context);
    }

    public KerWebView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @SuppressLint("JavascriptInterface")
    @Override
    public void setViewConfiguration(long viewConfiguration) {

        final Handler handler =  new Handler();
        final WeakReference<KerWebView> webView = new WeakReference<>(this);

        final WebViewConfiguration configuration = Native.getWebViewConfiguration(viewConfiguration);

        getSettings().setAllowFileAccess(true);
        getSettings().setAllowFileAccessFromFileURLs(true);
        getSettings().setAllowUniversalAccessFromFileURLs(true);
        getSettings().setJavaScriptEnabled(true);
        getSettings().setDefaultTextEncodingName("UTF-8");

        setWebChromeClient(new WebChromeClient(){


        });

        setWebViewClient(new WebViewClient(){

            public void evaluateJavascript(WebView view, String source) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                    view.evaluateJavascript(source,null);
                } else {
                    view.loadUrl("javascript:(function(){" +source+ "})();");
                }
            }
            public void onPageFinished(WebView view, String url) {

                if(configuration != null && configuration.userActions != null) {

                    for(WebViewConfiguration.UserScript us : configuration.userScripts) {
                        evaluateJavascript(view,us.source);
                    }

                }

                evaluateJavascript(view,"window.dispatchEvent(new Event('ker'));");

                super.onPageFinished(view,url);
            }

            public boolean handlerURL(String url) {
                if(configuration != null && configuration.userActions != null) {

                    for(WebViewConfiguration.UserAction ua : configuration.userActions) {

                        Pattern pattern = Pattern.compile(ua.pattern);

                        if(pattern.matcher(url).find()) {

                            KerWebView v = webView.get();

                            if(v != null && v._kerObject != 0) {
                                Map<String,Object> data = new TreeMap<>();
                                data.put("url",url);
                                Native.emit(v._kerObject,"action",data);
                            }


                            if(ua.policy == WebViewConfiguration.USER_ACTION_POLICY_ALLOW) {
                                return true;
                            } else {
                                return false;
                            }
                        }
                    }

                }
                return true;
            }

            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                return handlerURL(url);
            }

            @Override
            public boolean shouldOverrideUrlLoading(WebView view, WebResourceRequest request) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    return handlerURL(request.getUrl().toString());
                }
                return super.shouldOverrideUrlLoading(view,request);
            }

            public WebResourceResponse handleRequest(WebView view,String url,String method) {
                if(url.startsWith("ker-data://") || url.startsWith("ker-tmp://") || url.startsWith("ker-app://")) {
                    String path = Package.resolvePath(getContext(),url);
                    String mimeType = Package.mimeType(path,null,"application/o-stream");
                    try {
                        return new WebResourceResponse(mimeType,null,new FileInputStream(path));
                    } catch (FileNotFoundException e) {
                        Log.e("ker",Log.getStackTraceString(e));
                        return null;
                    }
                }
                return null;
            }

            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, String url) {
                WebResourceResponse v = handleRequest(view,url,"GET");
                if(v == null) {
                    return super.shouldInterceptRequest(view,url);
                }
                return v;
            }

            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                WebResourceResponse v = null;

                if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
                    v = handleRequest(view,request.getUrl().toString(),request.getMethod());
                }

                if(v == null) {
                    return super.shouldInterceptRequest(view,request);
                }

                return v;
            }
        });

        addJavascriptInterface(new Object(){

            @SuppressLint("JavascriptInterface")
            @JavascriptInterface
            public void postMessage(final String text) {

                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        KerWebView b = webView.get();
                        if(b != null && b._kerObject != 0) {
                            Native.emit(b._kerObject, "data", new JSONString(text));
                        }
                    }
                });

            }

        },"ker");
    }

    @Override
    public void setAttributeValue(long object, String key, String value) {

        if("src".equals(key)) {
            if(value != null) {
                if(value.contains("://")) {
                    loadUrl(value);
                } else {
                    loadUrl(Package.resolveURI(getContext(),Native.absolutePath(object,value)));
                }
            }
        } else if("#text".equals(key)) {
            if(value != null) {
                loadDataWithBaseURL(Package.resolveURI(getContext(),Native.absolutePath(object,"/")),value,"text/html","utf-8",null);
            }
        }

        KerView.setAttributeValue(this,object,key,value);
    }

    private long _kerObject = 0;

    @Override
    public void recycle(long object) {
        if(_kerObject == object) {
            _kerObject = 0;
        }
    }

    @Override
    public void obtain(long object) {

        _kerObject = object;

    }

    @Override
    public void setContent(long object, String content, String contentType, String basePath) {

        if(contentType == null) {
            contentType = "text/html";
        }

        if(content == null) {
            content = "";
        }

        String baseURL = Package.resolveURI(getContext(),Native.absolutePath(object,basePath));

        if(!baseURL.endsWith("/")) {
            baseURL = baseURL + "/";
        }

        loadDataWithBaseURL(baseURL,content,contentType,"utf-8",null);

    }

    @Override
    public void evaluateJavaScript(long object, String evaluateCode) {

        if(evaluateCode == null) {
            return ;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            evaluateJavascript(evaluateCode,null);
        } else {
            loadUrl("javascript:(function(){" +evaluateCode+ "})();");
        }

    }

    @Override
    public void setImage(Drawable image) {
        setBackground(image);
    }

    @Override
    public ViewGroup contentView() {
        return this;
    }


}
