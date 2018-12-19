package cn.kkmofang.wx;

import java.io.File;
import java.net.URLEncoder;
import java.util.Map;

import cn.kkmofang.ker.JSContext;
import cn.kkmofang.ker.JSMethod;
import cn.kkmofang.ker.JSObject;
import cn.kkmofang.ker.JSPrototype;
import cn.kkmofang.ker.App;
import cn.kkmofang.ker.Ker;
import cn.kkmofang.ker.Page;
import cn.kkmofang.ker.http.Callback;
import cn.kkmofang.ker.http.Request;
import cn.kkmofang.ker.http.Response;
import cn.kkmofang.ker.http.Session;
import cn.kkmofang.ker.http.SessionTask;

/**
 * Created by zhanghailong on 2018/12/12.
 */
@JSPrototype("WXObject")
public class WXObject {

    static {

        JSContext.addOpenlib(new JSContext.Openlib() {
            @Override
            public void open(long jsContext) {

                JSContext.PushPrototype(jsContext,WXObject.class);
                JSContext.PushPrototype(jsContext,WXRequestTask.class);

            }
        });

        Page.addOpenlib(new Page.Openlib() {
            @Override
            public void open(long jsContext, Page page) {
                page.addLibrary("wx",new WXObject());
            }
        });

        App.addOpenlib(new App.Openlib() {

            @Override
            public void open(long jsContext, App app) {
                JSContext.PushObject(jsContext,new WXObject());
                JSContext.PutGlobalString(jsContext,"wx");
            }

        });
    }

    private final WXHttp _http = new WXHttp();

    @JSMethod()
    public WXRequestTask request(JSObject jsObject) {
        if(jsObject == null) {
            return null;
        }
        return _http.request(jsObject.implemenetInterface(WXRequestObject.class));
    }

    @JSMethod()
    public void reportAnalytics(String name) {

    }
}
