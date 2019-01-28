//
//  uri.cc
//  Ker
//
//  Created by zhanghailong on 2019/1/2.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include <core/uri.h>

namespace kk {
    
    URI::URI(kk::CString v) {
        if(v != nullptr) {
            kk::String s(v);
            auto i = s.find("://");
            if(i == std::string::npos) {
                i = 0;
            } else {
                _scheme.append(s.data(),i);
                i += 3;
            }
            auto n = s.find("#",i);
            if(n != std::string::npos) {
                _fragment.append(s.data() + n + 1);
                s = s.substr(0,n);
            }
            n = s.find("?",i);
            if(n != std::string::npos) {
                _query.append(s.data() + n + 1);
                s = s.substr(0,n);
            }
            if(_scheme.empty()) {
                _path = s;
            } else {
                
                n = s.find("/",i);
                
                if(n != std::string::npos) {
                    _path.append(s.data() + n );
                    s = s.substr(0,n);
                }
                
                _host.append(s.data() + i);
                
                n = _host.find(":");
                
                if(n != std::string::npos) {
                    _port.append(_host.data() + n + 1);
                    _hostname.append(_host.data(),n);
                } else {
                    _hostname = _host;
                }
            }
        }
    }
    
    kk::CString URI::scheme() {
        return _scheme.c_str();
    }
    
    void URI::setScheme(kk::CString v) {
        _scheme = v;
    }
    
    kk::CString URI::host() {
        return _host.c_str();
    }
    
    void URI::setHost(kk::CString v) {
        _host = v;
        auto n = _host.find(":");
        if(n != std::string::npos) {
            _port.append(_host.data() + n + 1);
            _hostname.append(_host.data(),n);
        } else {
            _hostname = _host;
        }
    }
    
    kk::CString URI::hostname() {
        return _hostname.c_str();
    }
    
    void URI::setHostname(kk::CString v) {
        _hostname = v;
        _host = v;
        if(!_port.empty()) {
            _host.append(":");
            _host.append(_port);
        }
    }
    
    kk::CString URI::port() {
        return _port.c_str();
    }
    
    void URI::setPort(kk::CString v) {
        _port = v;
        _hostname = _host;
        if(!_port.empty()) {
            _host.append(":");
            _host.append(_port);
        }
    }
    
    kk::CString URI::path() {
        return _path.c_str();
    }
    
    void URI::setPath(kk::CString v) {
        _path = v;
    }
    
    kk::CString URI::query() {
        if(_query.empty() && _queryObject != nullptr) {
            auto b = _queryObject->begin();
            auto e = _queryObject->end();
            auto i = b;
            while(i != e) {
                if(i != b) {
                    _query.append("&");
                }
                _query.append(i->first);
                _query.append("=");
                _query.append(encodeURL(i->second.c_str()));
                i ++;
            }
        }
        return _query.c_str();
    }
    
    void URI::setQuery(kk::CString v) {
        _query = v;
        _queryObject = nullptr;
    }
    
    kk::CString URI::fragment() {
        return _fragment.c_str();
    }
    
    void URI::setFragment(kk::CString v) {
        _fragment = v;
    }
    
    kk::TObject<kk::String,kk::String> & URI::queryObject() {
        if(_queryObject == nullptr) {
            kk::TObject<kk::String,kk::String> * v = new kk::TObject<kk::String,kk::String>();
            _queryObject = v;
            if(!_query.empty()) {
                std::vector<kk::String> items;
                kk::CStringSplit(_query.c_str(), "&", items);
                std::vector<kk::String> kv;
                auto i = items.begin();
                while(i != items.end()) {
                    kv.clear();
                    kk::CStringSplit((*i).c_str(), "=", kv);
                    if(kv.size() > 1) {
                        (*v)[kv[0]] = decodeURL(kv[1].c_str());
                    } else if(kv.size() > 0) {
                        (*v)[kv[0]] = "";
                    }
                    i ++;
                }
            }
        }
        return *_queryObject;
    }
    
    kk::CString URI::get(kk::CString key) {
        if(key == nullptr) {
            return nullptr;
        }
        kk::TObject<kk::String,kk::String> & v = queryObject();
        auto i = v.find(key);
        if(i != v.end()) {
            return i->second.c_str();
        }
        return nullptr;
    }
    
    void URI::set(kk::CString key,kk::CString value) {
        if(key == nullptr) {
            return;
        }
        if(value == nullptr) {
            kk::TObject<kk::String,kk::String> & v = queryObject();
            auto i = v.find(key);
            if(i != v.end()) {
                v.erase(i);
            }
        } else {
            kk::TObject<kk::String,kk::String> & v = queryObject();
            v[key] = value;
        }
        _query.clear();
    }
    
    kk::String URI::toString() {
        kk::String v;
        
        if(!_scheme.empty()) {
            v.append(_scheme);
            v.append("://");
        }
        
        if(!_host.empty()) {
            v.append(_host);
        }
        
        if(!_path.empty()) {
            v.append(_path);
        }
        
        if(_query.empty()) {
            query();
        }
        
        if(!_query.empty()) {
            v.append("?");
            v.append(_query);
        }
        
        if(!_fragment.empty()) {
            v.append("#");
            v.append(_fragment);
        }
        
        return v;
    }
    
    kk::String URI::encodeURL(kk::CString value) {
    
        kk::String v;
        
        unsigned char * p = (unsigned char *) value;
        char fmt[8];
        
        while(p && *p) {
            
            if((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z')) {
                v.append((char *) p,1);
            } else if(*p == ' ') {
                v.append("+");
            } else {
                snprintf(fmt, sizeof(fmt), "%%%02x",*p);
                v.append(fmt);
            }
            
            p ++;
        }
        
        return v;
        
    }
    
    kk::String URI::decodeURL(kk::CString value) {
        
        kk::String v;
        
        char * p = (char *) value;
        int i;
        unsigned char b;
        
        while(p && *p) {
            
            if(*p == '%') {
                sscanf(p, "%%%02x",&i);
                b = 0x0ff & i;
                v.append((char *) &b, 1);
                p += 2;
            } else if(*p == '+') {
                v.append(" ", 1);
            } else {
                v.append(p,1);
            }
            
            p ++;
        }
        
        return v;
        
    }
    
    kk::CString kTemporaryDirectory = "tmp";
    kk::CString kDataDirectory = "data";
    kk::CString kAppDirectory = "app";
    kk::CString kNativeDirectory = "native";
    kk::CString Directorys[] = {kTemporaryDirectory,kDataDirectory,kAppDirectory,kNativeDirectory,nullptr};
    
    kk::String ResolveURI(kk::CString path) {
        
        kk::CString *p = Directorys;
        
        while(*p) {
            kk::String s = GetDirectory(*p);
            s.append("/");
            if(kk::CStringHasPrefix(path, s.c_str())) {
                String v;
                v.append("ker-");
                v.append(*p);
                v.append(":///");
                v.append(path + s.length());
                return v;
            }
            p ++;
        }
        
        return path;
    }
    
    kk::String ResolvePath(kk::CString uri) {
        kk::String v;
        kk::URI u(uri);
        return ResolvePath(u);
    }
    
    kk::String ResolvePath(URI & u) {
        kk::String v;
        if(kk::CStringHasPrefix(u.scheme(), "ker-")) {
            v.append(GetDirectory(u.scheme() + 4));
            if(kk::CStringHasSuffix(v.c_str(), "/") && kk::CStringHasPrefix(u.path(), "/")) {
                v.append(u.path() + 1);
            } else {
                v.append(u.path());
            }
        } else if(kk::CStringHasPrefix(u.path(), "/")){
            v.append(u.path() + 1);
        } else {
            v.append(u.path());
        }
        return v;
    }
    
    kk::Boolean isWritableURI(kk::CString uri) {
        return kk::CStringEqual(uri, "ker-data://") || kk::CStringEqual(uri, "ker-tmp://");
    }
    
    kk::Boolean isURLInDirectory(kk::CString uri) {
        return kk::CStringEqual(uri, "ker-data://")
            || kk::CStringEqual(uri, "ker-tmp://")
            || kk::CStringEqual(uri, "ker-app://")
            || kk::CStringEqual(uri, "ker-native://");
    }

    
}
