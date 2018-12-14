package cn.kkmofang.ker;

import java.util.Map;

/**
 * Created by zhanghailong on 2018/12/14.
 */

public final class Ker {

    public static Object get(Object object,String key) {

        if(object == null) {
            return null;
        }

        if(object instanceof Map) {
            Map<?,?> m = (Map<?,?>) object;
            if(m.containsKey(key)) {
                return m.get(key);
            }
            return null;
        }

        if(object instanceof JSObject) {
            return ((JSObject) object).get(key);
        }

        return null;
    }

    public static String stringValue(Object v,String defaultValue) {
        if(v != null) {
            if(v instanceof String) {
                return (String) v;
            }
            return v.toString();
        }
        return defaultValue;
    }

    public static boolean booleanValue(Object v,boolean defaultValue) {
        if(v != null) {
            if(v instanceof String) {
                return !"false".equals(v);
            } else if( v instanceof Boolean) {
                return (Boolean) v;
            } else if(v instanceof Number) {
                return ((Number) v).doubleValue() != 0;
            }
            return true;
        }
        return defaultValue;
    }

    public static String getString(Object object,String key,String defaultValue) {
        return stringValue(get(object,key),defaultValue);
    }

}
