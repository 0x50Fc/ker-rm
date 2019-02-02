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

#define FormDataToken "8jej23fkdxxd"
#define FormDataTokenBegin "--8jej23fkdxxd"
#define FormDataTokenEnd "--8jej23fkdxxd--"
#define FormDataMutilpartType "multipart/form-data; boundary=8jej23fkdxxd"
    
    FormData::FormData() {
        
    }
    
    void FormData::append(kk::CString key,kk::CString value) {
        if(key == nullptr) {
            return ;
        }
        FormDataEntry entry = {key , value ? value : "",nullptr};
        _entrys.push_back(entry);
    }
    
    void FormData::append(kk::CString key,kk::Blob * Blob, kk::CString filename) {
        if(key == nullptr) {
            return ;
        }
        FormDataEntry entry = {key , filename ? filename : "blob",Blob};
        _entrys.push_back(entry);
    }
    
    void FormData::append(kk::CString key,kk::File * file) {
        if(key == nullptr) {
            return ;
        }
        FormDataEntry entry = {key , file->name() ,file};
        _entrys.push_back(entry);
    }
    
    void FormData::append(kk::CString key,kk::Any v) {
        if(v.type == TypeObject) {
            {
                kk::File * a = v.objectValue;
                if(a != nullptr) {
                    append(key, a);
                    return;
                }
            }
            {
                kk::Blob * a = v.objectValue;
                if(a != nullptr) {
                    append(key, a, "blob");
                    return;
                }
            }
        } else {
            kk::CString vv = v;
            if(vv != nullptr) {
                append(key, vv);
            }
        }
    }
    
    kk::CString FormData::contentType() {
        return FormDataMutilpartType;
    }
    
    void FormData::read(Buffer & data) {
        auto i = _entrys.begin();
        auto e = _entrys.end();
        while(i != e) {
            auto & entry = * i;
            Blob * object = entry.object;
            if(object != nullptr) {
                data.format("%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n",FormDataTokenBegin,entry.key.c_str(),entry.value.c_str());
                data.format("Content-Type: %s\r\nContent-Transfer-Encoding: binary\r\n\r\n",object->type());
                data.capacity(data.byteLength() + object->size());
                if(object->read(data.data() + data.byteLength())) {
                    data.setByteLength(data.byteLength() + object->size());
                }
                data.format("\r\n");
            } else {
                data.format("%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n",FormDataTokenBegin,entry.key.c_str(),entry.value.c_str());
            }
            i ++;
        }
        data.format("%s",FormDataTokenEnd);
    }
    
    
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
    
    void HTTPRequest::send(FormData * data) {
        if(data == nullptr) {
            send(nullptr,0);
            return ;
        }
        kk::Strong<Buffer> b = new Buffer();
        kk::Strong<DispatchQueue> queue = (DispatchQueue *) _queue;
        kk::Strong<HTTPRequest> r = this;
        kk::Strong<FormData> d = data;
        IODispatchQueue()->async([b,r,queue,d]()->void{
            Buffer & data = b.operator*();
            d->read(data);
            queue->async([b,r,d]()->void{
                HTTPRequest * req = r.operator->();
                req->send(b->data(), b->byteLength());
            });
        });
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
    
    kk::Strong<TObject<kk::String,kk::String>> HTTPRequest::responseHeaders() {
        return new TObject<kk::String, kk::String>(_respHeaders);
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
                kk::String p = r->_tempFile;
                IODispatchQueue()->async([p]()->void{
                    remove(p.c_str());
                });
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
    
    kk::Strong<File> HTTPRequest::responseFile() {
        kk::String p = ResolveURI(_tempFile.c_str());
        kk::CString contentType = getResponseHeader("Content-Type");
        if(contentType == nullptr) {
            contentType = getResponseHeader("content-type");
        }
        return File::open(_tempFile.c_str(), p.c_str(), contentType, true);
    }
    
    kk::Strong<kk::ArrayBuffer> HTTPRequest::responseArrayBuffer() {
        if(_buf.byteLength() == 0) {
            return nullptr;
        }
        return new ArrayBuffer(_buf.data(),_buf.byteLength());
    }
    
    void HTTPRequest::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<FormData>(ctx, [](duk_context * ctx)->void{
                kk::PutMethod<FormData,void,kk::CString,kk::Any>(ctx, -1, "append", &FormData::append);
            });
            
            kk::PushClass<HTTPRequest>(ctx, [](duk_context * ctx)->void{
                
                duk_push_int(ctx, HTTPResponseTypeNone);
                duk_put_prop_string(ctx, -3, "ResponseTypeNone");
                duk_push_int(ctx, HTTPResponseTypeArrayBuffer);
                duk_put_prop_string(ctx, -3, "ResponseTypeArrayBuffer");
                duk_push_int(ctx, HTTPResponseTypeString);
                duk_put_prop_string(ctx, -3, "ResponseTypeString");
                duk_push_int(ctx, HTTPResponseTypeFile);
                duk_put_prop_string(ctx, -3, "ResponseTypeFile");
                duk_push_int(ctx, HTTPResponseTypeUnzip);
                duk_put_prop_string(ctx, -3, "ResponseTypeUnzip");
                
                kk::PutMethod<HTTPRequest,void,kk::CString,kk::CString,HTTPResponseType>(ctx, -1, "open", &HTTPRequest::open);
                kk::PutMethod<HTTPRequest,void,kk::Any>(ctx, -1, "send", &HTTPRequest::send);
                kk::PutMethod<HTTPRequest,void>(ctx, -1, "cancel", &HTTPRequest::cancel);
                kk::PutMethod<HTTPRequest,void,kk::CString,kk::CString>(ctx, -1, "setRequestHeader", &HTTPRequest::setRequestHeader);
                kk::PutMethod<HTTPRequest,kk::CString,kk::CString>(ctx, -1, "getResponseHeader", &HTTPRequest::getResponseHeader);
                kk::PutProperty<HTTPRequest,kk::Int>(ctx, -1, "statusCode", &HTTPRequest::statusCode);
                kk::PutProperty<HTTPRequest,kk::String>(ctx, -1, "responseText", &HTTPRequest::responseText);
                kk::PutStrongProperty<HTTPRequest,kk::File>(ctx, -1, "responseFile", &HTTPRequest::responseFile);
                kk::PutStrongProperty<HTTPRequest,kk::ArrayBuffer>(ctx, -1, "responseArrayBuffer", &HTTPRequest::responseArrayBuffer);
                kk::PutStrongProperty<HTTPRequest,kk::TObject<kk::String, kk::String>>(ctx, -1, "responseHeaders", &HTTPRequest::responseHeaders);
                
            });
            
        });
        
    }
    
    
}
