package cn.kkmofang.ker.http;

/**
 * Created by hailong11 on 2018/12/13.
 */

public class Response extends Headers {

    public final int code;

    public Response(int code) {
        this.code = code;
    }

}
