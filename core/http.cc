//
//  http.cpp
//  Ker
//
//  Created by zhanghailong on 2019/1/2.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include <core/http.h>
#include <core/uri.h>
#include <minizip/unzip.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

namespace kk {

    HTTPRequest::HTTPRequest():HTTPRequest(getCurrentDispatchQueue()) {

    }
    
    HTTPRequest::HTTPRequest(DispatchQueue * queue):_queue(queue),_responseType(HTTPResponseTypeNone) {
        
    }
    
    HTTPRequest::~HTTPRequest() {
        
    }
    
    void HTTPRequest::open(kk::CString method,kk::CString url,HTTPResponseType responseType) {
        if(method != nullptr) {
            _method = method;
        }
        if(url != nullptr) {
            _url = url;
        }
        _responseType = responseType;
        if(_responseType == HTTPResponseTypeFile || _responseType == HTTPResponseTypeUnzip) {
            _tempFile = GetDirectory(kTemporaryDirectory);
            _tempFile.append("/");
            char v[] = "XXXXXXXX";
            mkstemp(v);
            _tempFile.append(v);
        }
    }
    
    void HTTPRequest::send() {
        send(nullptr,0);
    }
    
    void HTTPRequest::send(kk::CString text) {
        if(text == nullptr) {
            send(nullptr, 0);
        } else {
            send((void *) text,kk::CStringLength(text));
        }
    }
    
    void HTTPRequest::send(kk::ArrayBuffer * buffer) {
        if(buffer == nullptr) {
            send(nullptr,0);
        } else {
            send(buffer->data(),buffer->byteLength());
        }
    }
    
    void HTTPRequest::send(kk::Any value) {
        switch (value.type) {
            case TypeString:
                send(value.stringValue);
                return;
            case TypeObject:
            {
                kk::ArrayBuffer * v = value;
                if(v != nullptr) {
                    send(v);
                    return ;
                }
            }
                break;
            default:
                break;
        }
        send(nullptr,0);
    }
    
    void HTTPRequest::setRequestHeader(kk::CString key,kk::CString value) {
        if(key == nullptr) {
            return;
        }
        if(value == nullptr) {
            auto i = _reqHeaders.find(key);
            if(i != _reqHeaders.end()) {
                _reqHeaders.erase(i);
            }
        } else {
            _reqHeaders[key] = value;
        }
    }
    
    kk::CString HTTPRequest::getResponseHeader(kk::CString key) {
        if(key == nullptr) {
            return nullptr;
        }
        auto i = _respHeaders.find(key);
        if(i != _respHeaders.end()) {
            return i->second.c_str();
        }
        return nullptr;
    }
    
    kk::Int HTTPRequest::statusCode() {
        return _statusCode;
    }

    DispatchQueue * HTTPRequest::queue() {
        return _queue;
    }
    
    HTTPResponseType HTTPRequest::responseType() {
        return _responseType;
    }
    
    std::map<kk::String,kk::String> & HTTPRequest::reqHeaders() {
        return _reqHeaders;
    }
    
    std::map<kk::String,kk::String> & HTTPRequest::respHeaders() {
        return _respHeaders;
    }
    
    kk::Native * HTTPRequest::native() {
        if(_native != nullptr) {
            return _native->native();
        }
        return nullptr;
    }
    
    void HTTPRequest::setNative(kk::Native * native) {
        if(native == nullptr) {
            _native = nullptr;
        } else {
            _native = new NativeObject(native);
        }
    }

    static void HTTPRequestMkdirs(kk::String path) {
        struct stat st;
        if(-1 == stat(path.c_str(),&st)) {
            auto i = path.find_last_of("/");
            if(i != std::string::npos) {
                HTTPRequestMkdirs(path.substr(0,i));
            }
            mkdir(path.c_str(), 0777);
        }
    }

    void HTTPRequest::onResponse(kk::Int statusCode) {
        _statusCode = statusCode;
        if(_responseType == HTTPResponseTypeFile || _responseType == HTTPResponseTypeUnzip) {
            FILE * fd = fopen(_tempFile.c_str(), "wb");
            if(fd != nullptr){
                fclose(fd);
            } else {
                kk::Log("[HTTP] Not Open File: %s",_tempFile.c_str());
            }
        }
        kk::Weak<HTTPRequest> req = this;
        _queue->async([req]()->void{
            kk::Strong<HTTPRequest> r = req.operator->();
            if(r != nullptr) {
                kk::Strong<kk::Event> e = new Event();
                r->emit("response", e);
            }
        });
    }
    
    void HTTPRequest::onData(void * data, size_t size) {
        if(_responseType == HTTPResponseTypeNone) {
            kk::Strong<ArrayBuffer> v = new ArrayBuffer(data,(kk::Uint) size);
            kk::Weak<HTTPRequest> req = this;
            _queue->async([req,v]()->void{
                kk::Strong<HTTPRequest> r = req.operator->();
                if(r != nullptr) {
                    kk::Strong<kk::Event> e = new Event();
                    e->setData(v.operator->());
                    r->emit("data", e);
                }
            });
        } else if(_responseType == HTTPResponseTypeFile || _responseType == HTTPResponseTypeUnzip) {
            FILE * fd = fopen(_tempFile.c_str(), "ab");
            if(fd != nullptr){
                if(size != fwrite(data, 1, size, fd)) {
                    kk::Log("[HTTP] Write File Error: %s",_tempFile.c_str());
                }
                fclose(fd);
            } else {
                kk::Log("[HTTP] Not Open File: %s",_tempFile.c_str());
            }
        } else {
            _buf.capacity(_buf.byteLength() + MAX((kk::Uint) size, 2048));
            memcpy(_buf.data() + _buf.byteLength(), data, size);
            _buf.setByteLength(_buf.byteLength() + (kk::Uint) size);
        }
    }
    
    void HTTPRequest::onError(kk::CString errmsg) {
        if(_responseType == HTTPResponseTypeFile || _responseType == HTTPResponseTypeUnzip) {
            remove(_tempFile.c_str());
        }
        kk::String err(errmsg);
        kk::Weak<HTTPRequest> req = this;
        _queue->async([req,err]()->void{
            kk::Strong<HTTPRequest> r = req.operator->();
            if(r != nullptr) {
                kk::Strong<kk::Event> e = new Event();
                e->setData(new TObject<kk::String, kk::String>({{"errmsg",err}}));
                r->emit("error", e);
            }
        });
    }

    
    void HTTPRequest::onDone() {
        
        if(_responseType == HTTPResponseTypeUnzip) {
            
            kk::String dir = _tempFile;
            kk::String errmsg;
            
            dir.append("-unzip/");
            
            unzFile zf = unzOpen(_tempFile.c_str());
            
            if(zf) {
                
                unz_file_info fi;
                char szFileName[2048];
                char data[204800];
                ssize_t n;
   
                int err = unzGoToFirstFile(zf);
                
                while(err == UNZ_OK) {
                    
                    unzGetCurrentFileInfo(zf, &fi, szFileName, sizeof(szFileName), nullptr, 0, nullptr, 0);
                    
                    if(kk::CStringHasSuffix(szFileName, "/")) {
                        HTTPRequestMkdirs(dir + szFileName);
                    } else {
                        
                        kk::String p = dir + szFileName;
                        HTTPRequestMkdirs(CStringPathDeleteLast(p.c_str()));

                        FILE * fd = fopen(p.c_str(), "wb");
                        
                        if(fd) {
                            
                            if(unzOpenCurrentFile(zf) == UNZ_OK) {
                                
                                while((n = unzReadCurrentFile(zf, data, sizeof(data))) > 0) {
                                    fwrite(data, 1, n, fd);
                                }
                                
                                unzCloseCurrentFile(zf);
                                
                            } else {
                                errmsg = "解压文件出错";
                            }
                            
                            fclose(fd);
                        } else {
                            errmsg = "无法创建文件";
                        }
                        
                    }
                    
                    err = unzGoToNextFile(zf);
                }
                
                unzClose(zf);
                
                if(!errmsg.empty()) {
                    unlink(_tempFile.c_str());
                    unlink(dir.c_str());
                    this->onError(errmsg.c_str());
                    return;
                } else {
                    
                    unlink(_tempFile.c_str());;
                    
                    _tempFile = dir;
                }
            } else {
                
                this->onError("错误的应用包");
                return;
                
            }
            
        }
        
        kk::Weak<HTTPRequest> req = this;
        _queue->async([req]()->void{
            kk::Strong<HTTPRequest> r = req.operator->();
            if(r != nullptr) {
                kk::Strong<kk::Event> e = new Event();
                r->emit("done", e);
                remove(r->_tempFile.c_str());
            }
        });
    }
    
    void HTTPRequest::onProgress(kk::Int64 bytes,kk::Int64 total) {
        kk::Weak<HTTPRequest> req = this;
        _queue->async([req,bytes,total]()->void{
            kk::Strong<HTTPRequest> r = req.operator->();
            if(r != nullptr) {
                kk::Strong<kk::Event> e = new Event();
                e->setData(new TObject<kk::String, kk::Int64>({{"bytes",bytes},{"total",total}}));
                r->emit("progress", e);
            }
        });
    }
    
    kk::Buffer& HTTPRequest::response() {
        return _buf;
    }
    
    kk::String HTTPRequest::responseText() {
        kk::String v;
        if(_buf.byteLength() != 0) {
            v.append((kk::CString) _buf.data(),(size_t) _buf.byteLength());
        }
        return v;
    }
    
    kk::String HTTPRequest::responseFile() {
        return _tempFile;
    }
    
    kk::Strong<kk::ArrayBuffer> HTTPRequest::responseArrayBuffer() {
        if(_buf.byteLength() == 0) {
            return nullptr;
        }
        return new ArrayBuffer(_buf.data(),_buf.byteLength());
    }
    
    void HTTPRequest::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<HTTPRequest>(ctx, [](duk_context * ctx)->void{
                
                kk::PutMethod<HTTPRequest,void,kk::CString,kk::CString>(ctx, -1, "open", &HTTPRequest::open);
                kk::PutMethod<HTTPRequest,void,kk::Any>(ctx, -1, "send", &HTTPRequest::send);
                kk::PutMethod<HTTPRequest,void,kk::CString,kk::CString>(ctx, -1, "setRequestHeader", &HTTPRequest::setRequestHeader);
                kk::PutMethod<HTTPRequest,kk::CString,kk::CString>(ctx, -1, "getResponseHeader", &HTTPRequest::getResponseHeader);
                kk::PutMethod<HTTPRequest,kk::Int>(ctx, -1, "statusCode", &HTTPRequest::statusCode);
                
            });
            
        });
        
    }
    
    
}
