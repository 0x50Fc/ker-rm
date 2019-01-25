//
//  uri.h
//  Ker
//
//  Created by zhanghailong on 2019/1/2.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef kk_uri_h
#define kk_uri_h

#include <core/kk.h>

namespace kk {
    
    class URI;
    
    extern kk::CString Directorys[];
    
    extern kk::CString kTemporaryDirectory;
    extern kk::CString kDataDirectory;
    extern kk::CString kAppDirectory;
    extern kk::CString kNativeDirectory;
    
    kk::String ResolveURI(kk::CString path);
    kk::String ResolvePath(kk::CString uri);
    kk::String ResolvePath(URI &u);
    kk::Boolean isWritableURI(kk::CString uri);
    extern kk::String GetDirectory(kk::CString name);
    
    class URI : public Object {
    public:
        URI(kk::CString v);
        virtual kk::CString scheme();
        virtual void setScheme(kk::CString v);
        virtual kk::CString host();
        virtual void setHost(kk::CString v);
        virtual kk::CString hostname();
        virtual void setHostname(kk::CString v);
        virtual kk::CString port();
        virtual void setPort(kk::CString v);
        virtual kk::CString path();
        virtual void setPath(kk::CString v);
        virtual kk::CString query();
        virtual void setQuery(kk::CString v);
        virtual kk::CString fragment();
        virtual void setFragment(kk::CString v);
        virtual kk::TObject<kk::String,kk::String> & queryObject();
        virtual kk::CString get(kk::CString key);
        virtual void set(kk::CString key,kk::CString value);
        virtual kk::String toString();
        
        static kk::String encodeURL(kk::CString value);
        static kk::String decodeURL(kk::CString value);
       
    protected:
        kk::String _scheme;
        kk::String _host;
        kk::String _hostname;
        kk::String _port;
        kk::String _path;
        kk::String _query;
        kk::String _fragment;
        kk::Strong<kk::TObject<kk::String,kk::String>> _queryObject;
    };
    
}

#endif /* uri_h */
