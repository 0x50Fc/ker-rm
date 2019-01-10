package cn.kkmofang.wx;

/**
 * Created by zhanghailong on 2018/12/19.
 */

public interface WXRequestObject {

    String getUrl();
    Object getData();
    Object getHeader();
    String getMethod();
    String getDataType();
    String getResponseType();
    void success(WXRequestRes res);
    void fail(String errmsg);
    void complete();

}
