package cn.kkmofang.wx;

import cn.kkmofang.ker.JSMethod;
import cn.kkmofang.ker.JSObject;
import cn.kkmofang.ker.JSPrototype;
import cn.kkmofang.ker.KerUI;
import cn.kkmofang.ker.Page;

/**
 * Created by zhanghailong on 2018/12/12.
 */
@JSPrototype("WXObject")
public class WXObject {

    static {

        Page.addOpenlib(new Page.Openlib() {
            @Override
            public void open(Page page) {
                page.addLibrary("wx",new WXObject());
            }
        });

        KerUI.addOpenlib(new KerUI.Openlib() {

            @Override
            public void open(long appid) {
                KerUI.addPrototype(appid,WXObject.class);
                KerUI.addPrototype(appid,WXRequestTask.class);
                KerUI.addLibrary(appid,"wx",new WXObject());
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
