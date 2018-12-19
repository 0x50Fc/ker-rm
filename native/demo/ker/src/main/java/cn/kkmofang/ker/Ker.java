package cn.kkmofang.ker;

import android.content.Intent;
import android.util.JsonWriter;
import android.util.Log;

import java.io.IOException;
import java.io.StringWriter;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;
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

    public static String encodeURL(String v) {
        if(v == null) {
            return  null;
        }
        try {
            return URLEncoder.encode(v,"UTF-8");
        } catch (UnsupportedEncodingException e) {
            return v;
        }
    }

    public static String decodeURL(String v) {
        if(v == null) {
            return  null;
        }
        try {
            return URLDecoder.decode(v,"UTF-8");
        } catch (UnsupportedEncodingException e) {
            return v;
        }
    }

    public static String encodeJSON(Object object) {
        StringWriter sw = new StringWriter();
        JsonWriter w = new JsonWriter(sw);
        try {
            encodeJSON(object,w);
        } catch (IOException e) {
            Log.e("ker",Log.getStackTraceString(e));
        }
        return sw.toString();
    }

    public static void encodeJSON(Object object,JsonWriter w) throws IOException {

        if(object == null) {
            w.nullValue();
        } else if(object instanceof String) {
            w.value((String) object);
        } else if(object instanceof Long) {
            w.value(((Long) object).longValue());
        } else if(object instanceof Integer) {
            w.value(((Integer) object).longValue());
        } else if(object instanceof Number) {
            w.value((Number) object);
        } else if(object instanceof Boolean) {
            w.value((Boolean) object);
        } else if(object instanceof Map) {
            w.beginObject();
            Map<?,?> m = (Map<?,?>) object;
            for(Object key : m.keySet()) {
                w.name(stringValue(key,""));
                encodeJSON(m.get(key),w);
            }
            w.endObject();
        } else if(object.getClass().isArray()) {
            w.beginArray();
            int n = Array.getLength(object);
            for(int i=0;i<n;i++) {
                encodeJSON(Array.get(object,i));
            }
            w.endArray();
        } else if(object instanceof List) {
            w.beginArray();
            for(Object v : (List<?>) object) {
                encodeJSON(v);
            }
            w.endArray();
        } else {
            w.beginObject();
            for(Field fd : object.getClass().getFields()) {
                Object v;
                try {
                    v = fd.get(object);
                } catch (IllegalAccessException e) {
                    continue;
                }
                w.name(fd.getName());
                encodeJSON(v,w);
            }
            w.endObject();
        }
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
