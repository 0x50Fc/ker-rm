package cn.kkmofang.ker;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.JavascriptInterface;
import android.webkit.WebChromeClient;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebSettings;
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
        init();
    }

    public KerWebView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    protected void init() {
        setOverScrollMode(View.OVER_SCROLL_NEVER);
    }

    @SuppressLint("JavascriptInterface")
    @Override
    public void setViewConfiguration(long viewId, final WebViewConfiguration configuration,long appid) {

        final Handler handler =  new Handler();
        final WeakReference<KerWebView> webView = new WeakReference<>(this);

        WebSettings settings = getSettings();

        settings.setAllowFileAccess(true);
        settings.setAllowFileAccessFromFileURLs(true);
        settings.setAllowUniversalAccessFromFileURLs(true);
        settings.setJavaScriptEnabled(true);
        settings.setDefaultTextEncodingName("UTF-8");
        settings.setSupportZoom(false);
        settings.setBuiltInZoomControls(false);
        settings.setDisplayZoomControls(false);
        settings.setUseWideViewPort(true);

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

                            if(v != null && v._viewId != 0 && v._appid !=0 ) {
                                Map<String,Object> data = new TreeMap<>();
                                data.put("url",url);
                                KerUI.emit(v._appid,"action",v._viewId,data);
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
                    String path = KerUI.resolvePath(url);
                    String mimeType = KerUI.mimeType(path,null,"application/o-stream");
                    try {
                        return new WebResourceResponse(mimeType,null,new FileInputStream(path));
                    } catch (FileNotFoundException e) {
                        Log.d("ker",Log.getStackTraceString(e));
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

            KerWebView b = webView.get();
            if(b != null && b._viewId != 0 && b._appid != 0) {
                KerUI.emit(b._appid, "data", b._viewId,new JSONString(text));
            }

            }

        },"ker");

    }


    @Override
    public void setAttributeValue(long viewId, String key, String value,long appid) {

        if("src".equals(key)) {
            if(value != null) {
                if(value.contains("://")) {
                    loadUrl(value);
                } else {
                    loadUrl(KerUI.resolveURI(value,appid));
                }
            }
        } else if("#text".equals(key)) {
            if(value != null) {
                loadDataWithBaseURL(KerUI.resolveURI("/",appid),value,"text/html","utf-8",null);
            }
        }

        KerView.setAttributeValue(this,viewId,key,value,appid);
    }

    private long _viewId = 0;
    private long _appid = 0;

    @Override
    public void recycle(long viewId,long appid) {
        if(_viewId == viewId) {
            _viewId = 0;
            _appid = 0;
        }
    }

    @Override
    public void obtain(long viewId,long appid) {
        _viewId = viewId;
        _appid = appid;
    }

    @Override
    public void setContent(long viewId, String content, String contentType, String basePath,long appid) {

        if(contentType == null) {
            contentType = "text/html";
        }

        if(content == null) {
            content = "";
        }

        String baseURL = KerUI.resolveURI(basePath,appid);

        loadDataWithBaseURL(baseURL,content,contentType,"utf-8",null);

    }

    @Override
    public void evaluateJavaScript(long object, String evaluateCode,long appid) {

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
    public void setImage(long viewId,Object image,long appid) {
        if(image instanceof Drawable) {
            setBackground((Drawable) image);
        } else if(image instanceof Image) {
            setBackground(((Image) image).getDrawable());
        } else {
            setBackground(null);
        }
    }

    @Override
    public void setAttributedText(long object, CharSequence string,long appid) {
        if(string == null) {
            return;
        }
        setContent(object,string.toString(),"text/html","",appid);
    }

    @Override
    public void setContentSize(long viewId, int width, int height, long appid) {

    }

    @Override
    public void setContentOffset(long viewId, int x, int y, boolean animated, long appid) {
        scrollTo(x,y);
    }

    @Override
    public ViewGroup contentView() {
        return this;
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {

        int count = getChildCount();

        for(int i=0;i<count;i++) {

            View v = getChildAt(i);

            if(v != null && v.getVisibility() == View.VISIBLE) {

                Rect frame = (Rect) v.getTag(R.id.ker_frame);

                if(frame != null) {
                    v.layout(frame.x,frame.y,frame.x + frame.width,frame.y + frame.height);
                } else {
                    v.layout(l,t,r,b);
                }

            }

        }
    }

}
