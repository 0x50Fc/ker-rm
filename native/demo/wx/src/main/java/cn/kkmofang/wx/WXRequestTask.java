package cn.kkmofang.wx;

import java.util.Map;

import cn.kkmofang.ker.JSMethod;
import cn.kkmofang.ker.JSObject;
import cn.kkmofang.ker.JSPrototype;
import cn.kkmofang.ker.http.Response;
import cn.kkmofang.ker.http.Session;
import cn.kkmofang.ker.http.SessionTask;

/**
 * Created by zhanghailong on 2018/12/12.
 */
@JSPrototype("WXRequestTask")
public class WXRequestTask {

    private SessionTask _task;
    private JSObject _onHeadersReceived;

    public void setTask(SessionTask task) {
        _task = task;
    }

    @JSMethod()
    public void onHeadersReceived(JSObject jsObject) {
        _onHeadersReceived = jsObject;
    }

    @JSMethod()
    public void offHeadersReceived(JSObject jsObject) {
        if(jsObject == null || _onHeadersReceived == jsObject) {
            _onHeadersReceived = null;
        }
    }

    public void onResponseHeader(Map<String,String> header) {
        if(_onHeadersReceived != null) {
            _onHeadersReceived.call(header);
        }
    }

    @JSMethod()
    public void abort() {
        _task.cancel();
    }

}
