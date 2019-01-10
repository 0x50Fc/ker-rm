package cn.kkmofang.ker.http;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

/**
 * Created by hailong11 on 2018/12/13.
 */

public class Headers {

    protected List<Header> _headers = new LinkedList<>();
    protected Map<String,Header> _headerMap = new TreeMap<>();

    public void addHeader(String key,String value) {
        Header v = new Header(key,value);
        _headers.add(v);
        _headerMap.put(key,v);
    }

    public void setHeader(String key,String value) {
        Iterator<Header> i = _headers.iterator();
        while(i.hasNext()) {
            Header v = i.next();
            if(key.equals(v.key)) {
                i.remove();
            }
        }
        Header v = new Header(key,value);
        _headers.add(v);
        _headerMap.put(key,v);
    }

    public String getHeaderValue(String key) {
        if(_headerMap.containsKey(key)) {
            return _headerMap.get(key).value;
        }
        return null;
    }

    public Header[] getHeaders() {
        return _headers.toArray(new Header[_headers.size()]);
    }

    public Map<String,String> getHeaderMap() {
        Map<String,String> v = new TreeMap<>();
        for(Header h : _headers) {
            v.put(h.key,h.value);
        }
        return v;
    }

}
