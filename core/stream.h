//
//  stream.h
//  Ker
//
//  Created by zhanghailong on 2019/1/24.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef kk_stream_h
#define kk_stream_h

#include <core/kk.h>

namespace kk {
    
    class Stream : public Object {
    public:
        
        virtual void close() = 0;
        
        Ker_CLASS(Stream, Object, "Stream");
        
        static void Openlib();
        
    };
    
    class InputStream : public Stream {
    public:
        virtual ssize_t read(void * data, size_t length) = 0;
    };
    
    class OutputStream : public Stream {
    public:
        virtual ssize_t write(const void * data, size_t length) = 0;
    };
    
    enum StreamFileType {
        StreamFileTypeBinary,
        StreamFileTypeText
    };
    
    class FileInputStream : public InputStream{
    public:
        FileInputStream(kk::CString path,StreamFileType fileType);
        virtual ~FileInputStream();
        virtual void close();
        virtual ssize_t read(void * data, size_t length);
        
    protected:
        FILE * _fd;
    };
    
    class FileOutputStream : public OutputStream {
    public:
        FileOutputStream(kk::CString path,StreamFileType fileType);
        virtual ~FileOutputStream();
        virtual void close();
        virtual ssize_t write(const void * data, size_t length);
        
    protected:
        FILE * _fd;
    };
    
    class BufferInputStream : public InputStream {
    public:
        BufferInputStream(InputStream * input,kk::Uint size);
        virtual void close();
        virtual ssize_t read(void * data, size_t length);
        
        Ker_CLASS(BufferInputStream, InputStream, "BufferInputStream");
        
    protected:
        kk::Strong<InputStream> _input;
        Buffer _buffer;
    };
    
    class BufferOutputStream : public OutputStream {
    public:
        BufferOutputStream(OutputStream * input,kk::Uint size);
        virtual ~BufferOutputStream();
        virtual void close();
        virtual ssize_t write(const void * data, size_t length);
        virtual kk::Boolean flush();
        
        Ker_CLASS(BufferOutputStream, OutputStream, "BufferOutputStream");
        
    protected:
        kk::Strong<OutputStream> _output;
        Buffer _buffer;
        kk::Uint _maxSize;
    };
    
}

#endif /* stream_h */
