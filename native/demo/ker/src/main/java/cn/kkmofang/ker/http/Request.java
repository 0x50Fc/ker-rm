package cn.kkmofang.ker.http;

/**
 * Created by hailong11 on 2018/12/13.
 */

public class Request extends Headers {

    public final String url;
    public String method = "GET";
    public Object data;
    public int timeout;

    public Request(String url) {
        this.url = url;
    }
}
