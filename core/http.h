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
#include <core/file.h>

namespace kk {
    
    struct FormDataEntry {
        kk::String key;
        kk::String value;
        kk::Strong<Blob> object;
    };
    
    class FormData : public Object {
    public:
        FormData();
        virtual void append(kk::CString key,kk::CString value);
        virtual void append(kk::CString key,kk::Blob * Blob, kk::CString filename);
        virtual void append(kk::CString key,kk::File * file);
        virtual void append(kk::CString key,kk::Any v);
        virtual kk::CString contentType();
        virtual void read(Buffer & data);
        
        Ker_CLASS(FormData, Object, "FormData")
        
    protected:
        Buffer _b;
        std::vector<FormDataEntry> _entrys;
    };
    
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
        virtual void send(FormData * data);
        virtual void send(kk::Any value);
        virtual void send(void * data,size_t size);
        virtual void send();
        virtual void cancel();
        virtual HTTPResponseType responseType();
        virtual kk::Buffer& response();
        virtual kk::String responseText();
        virtual kk::Strong<File> responseFile();
        virtual kk::Strong<kk::ArrayBuffer> responseArrayBuffer();
        virtual void setRequestHeader(kk::CString key,kk::CString value);
        virtual kk::CString getResponseHeader(kk::CString key);
        virtual kk::Int statusCode();
        virtual DispatchQueue * queue();
        virtual std::map<kk::String,kk::String> & reqHeaders();
        virtual std::map<kk::String,kk::String> & respHeaders();
        virtual kk::Strong<TObject<kk::String,kk::String>> responseHeaders();
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
