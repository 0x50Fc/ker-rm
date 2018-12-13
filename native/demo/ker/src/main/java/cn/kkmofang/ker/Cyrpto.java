package cn.kkmofang.ker;

import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * Created by zhanghailong on 2018/12/13.
 */

public class Cyrpto {

    public static String MD5(String string) {
        try {
            MessageDigest M = MessageDigest.getInstance("MD5");
            byte[] m = M.digest(string.getBytes("UTF-8"));
            StringBuffer sb = new StringBuffer();
            for(byte b :m ){
                String s = Integer.toHexString(b);
                if(s.length() < 2) {
                    s = "0" + s;
                } else if(s.length() > 2) {
                    s = s.substring(s.length() - 2);
                }
                sb.append(s);
            }
            return sb.toString().toLowerCase();
        } catch (NoSuchAlgorithmException e) {
        } catch (UnsupportedEncodingException e) {
        }
        return null;
    }
}
