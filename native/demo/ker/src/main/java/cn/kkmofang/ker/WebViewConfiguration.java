package cn.kkmofang.ker;

/**
 * Created by zhanghailong on 2018/12/17.
 */

public class WebViewConfiguration {

    public final static int USER_SCRIPT_TYPE_DOCUMENT_START = 0;
    public final static int USER_SCRIPT_TYPE_DOCUMENT_END = 1;
    public final static int USER_ACTION_POLICY_ALLOW = 0;
    public final static int USER_ACTION_POLICY_CANCEL = 1;

    public UserScript userScripts[];

    public UserAction userActions[];

    public static class UserScript {
        public String source;
        public int type;
    }

    public static class UserAction {
        public String pattern;
        public int policy;
    }

}
