package cn.kkmofang.ker;

import android.content.Intent;

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

    public static Font fontValue(String v, Font font) {
        if(font == null) {
            font = new Font();
        }

        if(v != null) {
            for(String s : v.split(" ")) {
                if(s.endsWith("px")) {
                    try {
                        font.size = Float.valueOf(s.substring(0, s.length() - 2));
                    } catch (Throwable e) {

                    }
                } else  if("bold".equals(s)) {
                    font.weight = Font.WEIGHT_BOLD;
                } else if("italic".equals(s)) {
                    font.style = Font.STYLE_ITALIC;
                } else {
                    font.family = s;
                }
            }
        }

        return font;
    }
    public static int colorValue(String v, int defaultValue) {

        if(v != null) {
            if(v.startsWith("#")) {
                if(v.length() == 4) {
                    int r = Integer.valueOf(v.substring(1,2),16);
                    int g = Integer.valueOf(v.substring(2,3),16);
                    int b = Integer.valueOf(v.substring(3,4),16);
                    return 0xff000000 | ((r | r << 4) << 16) | ((g | g << 4) << 8) | (b | b << 4);
                } else if(v.length() == 7) {
                    int r = Integer.valueOf(v.substring(1,3),16);
                    int g = Integer.valueOf(v.substring(3,5),16);
                    int b = Integer.valueOf(v.substring(5,7),16);
                    return 0xff000000 | (r << 16) | (g << 8) | b;
                } else if(v.length() == 9) {
                    int a = Integer.valueOf(v.substring(1,3),16);
                    int r = Integer.valueOf(v.substring(3,5),16);
                    int g = Integer.valueOf(v.substring(5,7),16);
                    int b = Integer.valueOf(v.substring(7,9),16);
                    return (a << 24) | (r << 16) | (g << 8) | b;
                }
            } else if(v.startsWith("rgba(") && v.endsWith(")")) {
                String[] vs = v.substring(5,v.length() - 6).split(",");
                if(vs.length > 3) {
                    int r = Integer.valueOf(vs[0], 10);
                    int g = Integer.valueOf(vs[1], 10);
                    int b = Integer.valueOf(vs[2], 10);
                    float a = Float.valueOf(vs[3]);
                    return ((int)(255 * a) << 24) | (r << 16) | ( g << 8) | b;
                }
            }
        }

        return defaultValue;
    }
}
