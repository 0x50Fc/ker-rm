//
//  http.h
//  Ker
//
//  Created by zhanghailong on 2019/1/2.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef kk_http_h
#define kk_http_h

#include <core/event.h>
#include <core/dispatch.h>

namespace kk {
    
    enum  {
        HTTPResponseTypeNone,
        HTTPResponseTypeString,
        HTTPResponseTypeArrayBuffer,
        HTTPResponseTypeFile,
        HTTPResponseTypeUnzip
    };
    
    typedef kk::Uint HTTPResponseType;
    
    class HTTPRequest : public EventEmitter {
    public:
        HTTPRequest(DispatchQueue * queue);
        HTTPRequest();
        virtual ~HTTPRequest();
        virtual void open(kk::CString method,kk::CString url,HTTPResponseType responseType);
        virtual void send(kk::CString text);
        virtual void send(kk::ArrayBuffer * buffer);
        virtual void send(kk::Any value);
        virtual void send(void * data,size_t size);
        virtual void send();
        virtual void cancel();
        virtual HTTPResponseType responseType();
        virtual kk::Buffer& response();
        virtual kk::String responseText();
        virtual kk::String responseFile();
        virtual kk::Strong<kk::ArrayBuffer> responseArrayBuffer();
        virtual void setRequestHeader(kk::CString key,kk::CString value);
        virtual kk::CString getResponseHeader(kk::CString key);
        virtual kk::Int statusCode();
        virtual DispatchQueue * queue();
        virtual std::map<kk::String,kk::String> & reqHeaders();
        virtual std::map<kk::String,kk::String> & respHeaders();
        virtual kk::Native * native();
        virtual void setNative(kk::Native * native);
        
        virtual void onResponse(kk::Int statusCode);
        virtual void onData(void * data, size_t size);
        virtual void onError(kk::CString errmsg);
        virtual void onProgress(kk::Int64 bytes,kk::Int64 total);
        virtual void onDone();
        
        Ker_CLASS(HTTPRequest,EventEmitter,"HTTPRequest")
        
        static void Openlib();
        
    protected:
        kk::Strong<DispatchQueue> _queue;
        std::map<kk::String,kk::String> _reqHeaders;
        std::map<kk::String,kk::String> _respHeaders;
        kk::Int _statusCode;
        kk::String _method;
        kk::String _url;
        HTTPResponseType _responseType;
        kk::Buffer _buf;
        kk::String _tempFile;
        kk::Strong<kk::NativeObject> _native;
    };
    
}

#endif /* http_h */
