//
//  stream.c
//  Ker
//
//  Created by zhanghailong on 2019/1/24.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "stream.h"
#include "jit.h"
#include <sys/stat.h>
#include <unistd.h>

namespace kk  {
    
    FileInputStream::FileInputStream(kk::CString path,StreamFileType fileType):_fd(nullptr) {
        switch (fileType) {
            case StreamFileTypeText:
                _fd = fopen(path, "r");
                break;
            case StreamFileTypeBinary:
                _fd = fopen(path, "rb");
                break;
            default:
                break;
        }
    }
    
    FileInputStream::~FileInputStream() {
        if(_fd != nullptr) {
            fclose(_fd);
            _fd = nullptr;
        }
    }
    
    void FileInputStream::close() {
        if(_fd != nullptr) {
            fclose(_fd);
            _fd = nullptr;
        }
    }
    
    ssize_t FileInputStream::read(void * data, size_t length) {
        if(_fd == nullptr) {
            return -1;
        }
        return fread(data, 1, length, _fd);
    }
    
    FileOutputStream::FileOutputStream(kk::CString path,StreamFileType fileType) {
        
        struct stat st;
        
        if(-1 != stat(path, &st)) {
            unlink(path);
        }
        
        switch (fileType) {
            case StreamFileTypeText:
                _fd = fopen(path, "w");
                break;
            case StreamFileTypeBinary:
                _fd = fopen(path, "wb");
                break;
            default:
                break;
        }
    }
    
    FileOutputStream::~FileOutputStream() {
        if(_fd != nullptr) {
            fclose(_fd);
            _fd = nullptr;
        }
    }
    
    void FileOutputStream::close() {
        if(_fd != nullptr) {
            fclose(_fd);
            _fd = nullptr;
        }
    }
    
    ssize_t FileOutputStream::write(const void * data, size_t length) {
        if(_fd == nullptr) {
            return -1;
        }
        return fwrite(data, 1, length, _fd);
    }
    
    BufferInputStream::BufferInputStream(InputStream * input,kk::Uint size):_input(input) {
        _buffer.capacity(MAX(2048,size));
    }
    
    void BufferInputStream::close() {
        if(_input != nullptr) {
            _input->close();
            _input = nullptr;
        }
    }
    
    ssize_t BufferInputStream::read(void * data, size_t length) {
        
        if(length > _buffer.byteLength()) {
            
            if(_input != nullptr) {
                _buffer.capacity(_buffer.byteLength() + MAX(2048,(kk::Uint) length));
                
                ssize_t n = _input->read(_buffer.data() + _buffer.byteLength(), _buffer.size() - _buffer.byteLength());
                
                if(n == -1) {
                    return -1;
                }
                if(n > 0) {
                    _buffer.setByteLength(_buffer.byteLength() + (kk::Uint) n);
                }
            }
            
        }
        
        kk::Uint n = MIN((kk::Uint) length,_buffer.byteLength());
        
        if(n > 0) {
            memcpy(data, _buffer.data(), n);
            _buffer.drain(n);
        }
        
        return n;
    }
    
    BufferOutputStream::BufferOutputStream(OutputStream * output,kk::Uint size):_output(output),_maxSize(size) {
    }
    
    BufferOutputStream::~BufferOutputStream() {
        flush();
    }
    
    void BufferOutputStream::close() {
        flush();
        if(_output != nullptr) {
            _output->close();
            _output = nullptr;
        }
    }
    
    ssize_t BufferOutputStream::write(const void * data, size_t length) {
        
        if(_output == nullptr) {
            return -1;
        }
        
        _buffer.append(data, (kk::Uint) length);
        
        if(_buffer.byteLength() >= _maxSize) {
            ssize_t n = _output->write(_buffer.data(), _buffer.byteLength());
            if(n >0){
                _buffer.drain((kk::Uint) n);
            } else {
                return n;
            }
        }
        
        return length;
    }
    
    kk::Boolean BufferOutputStream::flush() {
        
        if(_output == nullptr) {
            return false;
        }
        
        ssize_t n = _buffer.byteLength();
        
        if(n > 0 ){
            n = _output->write(_buffer.data(), (size_t) n);
            if(n > 0) {
                _buffer.drain((kk::Uint) n);
            }
        }
        
        return _buffer.byteLength() == 0;
    }
    
    void Stream::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushInterface<Stream>(ctx, [](duk_context *ctx)->void{
                
                kk::PutMethod(ctx, -1, "close", &Stream::close);
                
            });
            
            kk::PushClass<BufferInputStream,InputStream *,kk::Uint>(ctx, [](duk_context *ctx)->void{
                
            });
            
            kk::PushClass<BufferOutputStream,OutputStream *,kk::Uint>(ctx, [](duk_context *ctx)->void{
                kk::PutMethod(ctx, -1, "flush", &BufferOutputStream::flush);
            });
            
        });
        
    }

}
