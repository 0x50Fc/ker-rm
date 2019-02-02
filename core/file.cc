//
//  file.c
//  Ker
//
//  Created by zhanghailong on 2019/1/26.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "file.h"
#include <sys/stat.h>
#include <unistd.h>
#include "crypto.h"
#include "uri.h"
#include <stdio.h>

namespace kk {
 
    Blob::Blob(Object * object,kk::Uint offset,kk::Uint size,kk::CString type):_size(size),_object(object),_offset(offset) {
        if(type != nullptr) {
            _type = type;
        }
    }
    
    Blob::Blob(Any & object,Any & options):_size(0),_object(nullptr),_offset(0) {
        if(object.type == TypeObject) {
            
            do {
                
                {
                    ArrayBuffer * v = object.objectValue;
                    if(v != nullptr) {
                        _object = v;
                        _size = v->byteLength();
                        break;
                    }
                }
                
                {
                    _Array * v = object.objectValue;
                    
                    if(v != nullptr) {
                        
                        Buffer b;
                        
                        v->forEach([&b](Any & v)->void{
                            if(v.type == TypeObject) {
                                ArrayBuffer * a = v.objectValue;
                                if(a != nullptr) {
                                    b.append(a->data(), a->byteLength());
                                }
                            } else {
                                CString a = v;
                                if(a != nullptr) {
                                    b.append(a, (kk::Uint) kk::CStringLength(a));
                                }
                            }
                        });
                        
                        if(b.byteLength() > 0) {
                            _size = b.byteLength();
                            _object = new ArrayBuffer(b.data(),_size);
                        }
                        break;
                    }
                }
                
            } while (0);
            
        } else {
            CString v = object;
            if(v != nullptr) {
                _size = (kk::Uint) kk::CStringLength(v);
                _object = new ArrayBuffer((void *)v,_size);
            }
        }
        
        if(options.type == TypeObject) {
            do {
                {
                    _TObject * v = options.objectValue;
                    if(v != nullptr) {
                        v->forEach([this](Any & value,Any & key)->void{
                            kk::CString k = key;
                            if(kk::CStringEqual(k, "type")) {
                                _type = (kk::CString) value;
                            }
                        });
                        break;
                    }
                }
                {
                    JSObject * v = options.objectValue;
                    
                    if(v != nullptr) {
                        
                        duk_context * ctx = v->jsContext();
                        void * heapptr = v->heapptr();
                        
                        if(ctx && heapptr) {
                            duk_push_heapptr(ctx, heapptr);
                            duk_get_prop_string(ctx, -1, "type");
                            if(duk_is_string(ctx, -1)) {
                                _type = duk_to_string(ctx, -1);
                            }
                            duk_pop_2(ctx);
                        }
                    
                        break;
                    }
                }
            } while(0);
        } else {
            kk::CString v = options;
            if(v != nullptr) {
                _type = v;
            }
        }
    }
    
    Blob::~Blob() {
    }
    
    kk::Uint Blob::size() {
        return _size;
    }
    
    kk::CString Blob::type() {
        return _type.c_str();
    }
    
    kk::Strong<Blob> Blob::slice(kk::Int start,kk::Int end,kk::CString type) {
        kk::Int b = 0, e = 0;
        
        if(start < 0) {
            b = _size + start;
        } else{
            b = start;
        }
        
        if(end < 0) {
            e = _size + end;
        } else{
            e = end;
        }
        
        if(b < 0) {
            b = 0;
        }
        
        if(b > _size) {
            return new Blob(nullptr,0,0,type);
        }
        
        if(e > _size) {
            e = _size;
        }
        
        if(e <= b) {
            return new Blob(nullptr,0, 0,type);;
        }
        
        return new Blob(this,(kk::Uint) b,(kk::Uint)(e - b),type);
        
    }
    
    kk::Boolean Blob::read(void * dest) {
        return read(dest,0, _size);
    }
    
    kk::Boolean Blob::read(void * dest,kk::Uint offset,kk::Uint size) {
        
        {
            Blob * v = _object;
            
            if(v != nullptr) {
                return v->read(dest, _offset + offset, size);
            }
        }
        
        {
            ArrayBuffer * v = _object;
            if(v != nullptr) {
                if(_offset + offset + size <= v->byteLength()) {
                    memcpy(dest, v->data() + _offset + offset, size);
                    return true;
                }
            }
        }
        
        return false;
    }
    
    File::File(kk::CString path, kk::Uint size,kk::CString type,kk::CString name,kk::Int lastModified,kk::Boolean isWritable):Blob(nullptr,0,size,type),_lastModified(0),_name(name),_path(path),_isWritable(isWritable) {
    }
    
    kk::CString File::path() {
        return _path.c_str();
    }
    
    kk::CString File::name() {
        return _name.c_str();
    }
    
    kk::Int File::lastModified() {
        return _lastModified;
    }
    
    kk::Boolean File::isWritable() {
        return _isWritable;
    }
    
    void File::remove(std::function<void()> && done) {
        
        if(_isWritable) {
            kk::Strong<DispatchQueue> queue = getCurrentDispatchQueue();
            kk::String p(_path);
            
            IODispatchQueue()->async([p,done,queue]()->void{
                ::remove(p.c_str());
                if(done != nullptr) {
                    queue->async((std::function<void()> &&)done);
                }
            });
        }
    }
    
    void File::mkdir(kk::CString dirname) {
        struct stat st;
        if(-1 == stat(dirname, &st)) {
            kk::String s = CStringPathDirname(dirname);
            if(!s.empty()) {
                File::mkdir(s.c_str());
            }
            ::mkdir(dirname, 0777);
        }
    }
    
    void File::move(File * toFile,std::function<void()> && done) {
        
        if(toFile && toFile->isWritable()) {
            kk::Strong<DispatchQueue> queue = getCurrentDispatchQueue();
            kk::String p(_path);
            kk::String to(toFile->path());
            
            IODispatchQueue()->async([p,done,queue,to]()->void{
                unlink(to.c_str());
                kk::String dir = CStringPathDirname(to.c_str());
                File::mkdir(dir.c_str());
                rename(p.c_str(), to.c_str());
                if(done != nullptr) {
                    queue->async([done]()->void{
                        done();
                    });
                }
            });
        }
        
    }
    
    kk::Strong<InputStream> File::openInputStream() {
        return new FileInputStream(_path.c_str(),StreamFileTypeBinary);
    }
    
    kk::Strong<OutputStream> File::openOutputStream(kk::Boolean append) {
        if(isWritable()) {
            kk::String p = CStringPathDirname(_path.c_str());
            File::mkdir(p.c_str());
            return new FileOutputStream(_path.c_str(),StreamFileTypeBinary,append);
        }
        return nullptr;
    }
    
    void File::copy(File * toFile,std::function<void()> && done) {
        
        if(toFile && toFile->isWritable() ) {
            
            kk::Strong<DispatchQueue> queue = getCurrentDispatchQueue();
            kk::String p(_path);
            kk::String to(toFile->path());
            
            IODispatchQueue()->async([p,done,queue,to]()->void{
                
                FILE * fd = fopen(p.c_str(), "rb");
                
                if(fd != nullptr) {
                    
                    kk::String dir = kk::CStringPathDirname(to.c_str());
                    
                    File::mkdir(dir.c_str());
                    
                    FILE * out = fopen(to.c_str(), "wb");
                    
                    if(out != nullptr) {
                        
                        char data[204800];
                        ssize_t n;
                        
                        while((n = fread(data, 1, sizeof(data), fd)) > 0) {
                            fwrite(data, 1, n, out);
                        }
                              
                        fclose(out);
                    }
                    
                    fclose(fd);
                }
                
                if(done != nullptr) {
                    queue->async([done]()->void{
                        done();
                    });
                }
            });
        }
        
    }
    
    void File::jit_remove(JSObject * done) {
        if(done == nullptr) {
            remove(nullptr);
        } else{
            kk::Strong<JSObject> s = done;
            remove([s]()->void{
                s->invoke<void>(nullptr);
            });
        }
    }
    
    void File::jit_move(File * to,JSObject * done) {
        if(done == nullptr) {
            move(to,nullptr);
        } else{
            kk::Strong<JSObject> s = done;
            move(to,[s]()->void{
                s->invoke<void>(nullptr);
            });
        }
    }
    
    void File::jit_copy(File * to,JSObject * done) {
        if(done == nullptr) {
            copy(to,nullptr);
        } else{
            kk::Strong<JSObject> s = done;
            copy(to,[s]()->void{
                s->invoke<void>(nullptr);
            });
        }
    }
    
    kk::Boolean File::read(void * dest,kk::Uint offset,kk::Uint size) {
        
        FILE * fd = fopen(_path.c_str(), "rb");
        
        if(fd == nullptr) {
            return false;
        }
        
        if(offset > 0) {
            do {
                ssize_t n = fseek(fd, offset, SEEK_SET);
                if(n == -1) {
                    if(errno == EAGAIN) {
                        continue;
                    }
                    fclose(fd);
                    return false;
                }
                break;
            } while(1);
        }
        
        do {
            ssize_t n = fread(dest,1,size,fd);
            if(n == -1) {
                if(errno == EAGAIN) {
                    continue;
                }
                fclose(fd);
                return false;
            }
            fclose(fd);
            return n == size;
        } while(1);

    }

    kk::Strong<File> File::open(kk::CString path,kk::CString name,kk::CString type,kk::Boolean isWritable) {
        
        if(path == nullptr) {
            return nullptr;
        }
        
        struct stat st = {0};
        
        stat(path, &st);
        
        if(type == nullptr) {
            if(kk::CStringHasSuffix(path, ".jpg")) {
                type = "image/jpg";
            } else if(kk::CStringHasSuffix(path, ".jpeg")) {
                type = "image/jpeg";
            } else if(kk::CStringHasSuffix(path, ".png")) {
                type = "image/png";
            } else if(kk::CStringHasSuffix(path, ".gif")) {
                type = "image/gif";
            } else if(kk::CStringHasSuffix(path, ".webp")) {
                type = "image/webp";
            } else if(kk::CStringHasSuffix(path, ".mp3")) {
                type = "audio/mp3";
            } else if(kk::CStringHasSuffix(path, ".mp4")) {
                type = "video/mp4";
            } else if(kk::CStringHasSuffix(path, ".mov")) {
                type = "video/mov";
            } else if(kk::CStringHasSuffix(path, ".amr")) {
                type = "audio/amr";
            } else if(kk::CStringHasSuffix(path, ".spx")) {
                type = "audio/speex";
            } else if(kk::CStringHasSuffix(path, ".xml")) {
                type = "text/xml";
            } else if(kk::CStringHasSuffix(path, ".html")) {
                type = "text/html";
            }
        }
        
        if(name == nullptr) {
            name = CStringPathBasename(path);
        }
        
        return new File(path,(kk::Uint) st.st_size,type,name,(kk::Int) st.st_mtimespec.tv_sec,isWritable);
    }
    
    kk::Strong<File> File::openURI(kk::CString uri,kk::CString type) {
        if(isURLInDirectory(uri)) {
            kk::String p = ResolvePath(uri);
            return open(p.c_str(), uri, type, isWritableURI(uri));
        }
        return nullptr;
    }

    FileReader::FileReader():_queue(getCurrentDispatchQueue()),_readyState(FileReaderStateEMPTY) {
        
    }
    
    FileReader::~FileReader() {
        if(_fd != nullptr) {
            _fd->cancel();
            _fd = nullptr;
        }
    }
    
    void FileReader::readAsText(kk::Blob * v) {
        read(v, FileReaderTypeText);
    }
    
    void FileReader::readAsDataURL(kk::Blob * v) {
        read(v, FileReaderTypeDataURL);
    }
    
    void FileReader::readAsArrayBuffer(kk::Blob * v) {
        read(v, FileReaderTypeArrayBuffer);
    }
    
    void FileReader::abort() {
        if(_fd != nullptr) {
            _fd->setEvent(nullptr);
            _fd->cancel();
            _fd = nullptr;
        }
    }
    
    void FileReader::doError(kk::CString errmsg) {
        _error = errmsg;
        _readyState = FileReaderStateDONE;
        
        kk::Strong<Event> e = new Event(this);
        
        if(_onerror != nullptr) {
            _onerror->invoke<void,Event *>(nullptr, e);
        }
        
        emit("error", e);
        
    }
    
    void FileReader::doDone() {
        _readyState = FileReaderStateDONE;
        
        kk::Strong<Event> e = new Event(this);
        
        if(_onload != nullptr) {
            _onload->invoke<void,Event *>(nullptr, e);
        }
        
        emit("load", e);
    }
    
    JSObject * FileReader::onload() {
        return _onload;
    }
    
    void FileReader::setOnload(JSObject * func) {
        _onload = func;
    }
    
    JSObject * FileReader::onerror() {
        return _onerror;
    }
    
    void FileReader::setOnerror(JSObject * func) {
        _onerror = func;
    }
    
    kk::CString FileReader::error() {
        if(_error.empty()) {
            return nullptr;
        }
        return _error.c_str();
    }
    
    FileReaderState FileReader::readyState() {
        return _readyState;
    }
    
    kk::Any & FileReader::result() {
        return _result;
    }
    
    kk::Uint64 FileReader::size() {
        return _size;
    }
    
    kk::CString FileReader::md5() {
        return _md5.c_str();
    }
    
    void FileReader::read(kk::Blob * v,FileReaderType type) {
        
        if(v == nullptr || v->size() == 0) {
            return ;
        }
        
        _error.clear();
        _readyState = FileReaderStateLOADING;
        _result = nullptr;
        
        if(_fd != nullptr) {
            _fd->setEvent(nullptr);
            _fd->cancel();
            _fd = nullptr;
        }
        
        kk::Strong<Blob> b = v;
        kk::Strong<FileReader> rd = this;
        kk::Strong<DispatchSource> fd = createDispatchSource(0, DispatchSourceTypeTimer, IODispatchQueue());
        _fd = (DispatchSource *) fd;
        fd->setTimer(0, 0);
        fd->setEvent([rd, this,b,fd,type]()->void{
            
            kk::Ubyte * data = (kk::Ubyte *) malloc(b->size() + 1);
            
            if(b->read(data)) {
 
                Crypto C;
                this->_size = b->size();
                this->_md5 = C.MD5(data,b->size());
                
                switch (type) {
                    case FileReaderTypeText:
                        data[b->size()] = 0;
                        this->_result = (kk::CString) data;
                        break;
                    case FileReaderTypeDataURL:
                    {
                        Buffer v;
                        
                        v.format("data:%s;base64,",b->type());
                        C.encodeBASE64(data, b->size(), v);
                        this->_result = v.toCString();
                    }
                        break;
                    default:
                        this->_result = new ArrayBuffer(data,b->size());
                        break;
                }
                
                this->_queue->async([rd,this]()->void{
                    this->doDone();
                });
            } else {
                this->_queue->async([rd,this]()->void{
                    this->doError("File Reader Error");
                });
            }
            
            free(data);
            
            fd->suspend();
            
        });
        fd->resume();
    }
    
    void File::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<Blob,kk::Any,kk::Any>(ctx, [](duk_context *ctx)->void{
                
                kk::PutProperty(ctx, -1, "size", &Blob::size);
                kk::PutProperty(ctx, -1, "type", &Blob::type);
                kk::PutStrongMethod(ctx, -1, "slice", &Blob::slice);
                
            });
            
            kk::PushInterface<File>(ctx, [](duk_context *ctx)->void{
                
                kk::PutProperty(ctx, -1, "name", &File::name);
                kk::PutProperty(ctx, -1, "lastModified", &File::lastModified);
                kk::PutMethod(ctx, -1, "remove", &File::jit_remove);
                kk::PutMethod(ctx, -1, "move", &File::jit_move);
                kk::PutMethod(ctx, -1, "copy", &File::jit_copy);
                kk::PutStrongMethod(ctx, -1, "openInputStream", &File::openInputStream);
                kk::PutStrongMethod(ctx, -1, "openOutputStream", &File::openOutputStream);
                
                kk::PushStrongFunction(ctx, &File::openURI);
                duk_put_prop_string(ctx, -3, "open");
                
                
            });
            
            kk::PushClass<FileReader>(ctx, [](duk_context *ctx)->void{
                
                duk_push_int(ctx, FileReaderStateEMPTY);
                duk_put_prop_string(ctx, -3, "EMPTY");
                
                duk_push_int(ctx, FileReaderStateLOADING);
                duk_put_prop_string(ctx, -3, "LOADING");
                
                duk_push_int(ctx, FileReaderStateDONE);
                duk_put_prop_string(ctx, -3, "DONE");
                
                kk::PutMethod(ctx, -1, "readAsText", &FileReader::readAsText);
                kk::PutMethod(ctx, -1, "readAsDataURL", &FileReader::readAsDataURL);
                kk::PutMethod(ctx, -1, "readAsArrayBuffer", &FileReader::readAsArrayBuffer);
                kk::PutProperty(ctx, -1, "md5", &FileReader::md5);
                kk::PutProperty(ctx, -1, "size", &FileReader::size);
                kk::PutProperty(ctx, -1, "result", &FileReader::result);
                
                kk::PutMethod(ctx, -1, "abort", &FileReader::abort);
                kk::PutProperty(ctx, -1, "onload", &FileReader::onload, &FileReader::setOnload);
                kk::PutProperty(ctx, -1, "onerror", &FileReader::onerror, &FileReader::setOnerror);
                
            });
            
        });
        
    }
    
    
}
