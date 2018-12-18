package cn.kkmofang.wx;

import cn.kkmofang.ker.JSContext;
import cn.kkmofang.ker.JSMethod;
import cn.kkmofang.ker.JSObject;
import cn.kkmofang.ker.JSPrototype;
import cn.kkmofang.ker.App;
import cn.kkmofang.ker.Page;

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
                JSContext.PushPrototype(jsContext,RequestTask.class);

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

    @JSMethod()
    public RequestTask request(JSObject jsObject) {

        return null;
    }

    @JSMethod()
    public void reportAnalytics(String name) {

    }
}
