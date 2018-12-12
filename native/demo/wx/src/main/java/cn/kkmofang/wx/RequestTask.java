package cn.kkmofang.wx;

import cn.kkmofang.ker.JSMethod;
import cn.kkmofang.ker.JSObject;
import cn.kkmofang.ker.JSPrototype;

/**
 * Created by zhanghailong on 2018/12/12.
 */
@JSPrototype("WXRequestTask")
public interface RequestTask {

    @JSMethod()
    void onHeadersReceived(JSObject jsObject);

    @JSMethod()
    void offHeadersReceived(JSObject jsObject);

    @JSMethod()
    void abort();

}
