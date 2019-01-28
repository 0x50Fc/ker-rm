//
//  file.h
//  Ker
//
//  Created by zhanghailong on 2019/1/26.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef kk_file_h
#define kk_file_h

#include <core/kk.h>
#include <core/jit.h>
#include <core/event.h>
#include <core/stream.h>

namespace kk {
    
    class Blob : public Object {
    public:
        Blob(Object * object,kk::Uint offset,kk::Uint size,kk::CString type);
        virtual ~Blob();
        virtual kk::Uint size();
        virtual kk::CString type();
        virtual kk::Strong<Blob> slice(kk::Int start,kk::Int end,kk::CString type);
        virtual kk::Boolean read(void * dest);
        
        Ker_CLASS(Blob, Object, "Blob")
        
    protected:
        virtual kk::Boolean read(void * dest,kk::Uint offset,kk::Uint size);
        kk::Strong<Object> _object;
        kk::Uint _size;
        kk::Uint _offset;
        kk::String _type;
    };
    
    class File : public Blob {
    public:
        virtual kk::CString path();
        virtual kk::CString name();
        virtual kk::Int lastModified();
        
        virtual kk::Boolean isWritable();
        virtual void remove(std::function<void()> && done);
        virtual void move(File * to,std::function<void()> && done);
        virtual void copy(File * to,std::function<void()> && done);
        
        virtual kk::Strong<InputStream> openInputStream();
        virtual kk::Strong<OutputStream> openOutputStream(kk::Boolean append);
        
        virtual void jit_remove(JSObject * done);
        virtual void jit_move(File * to,JSObject * done);
        virtual void jit_copy(File * to,JSObject * done);
        
        Ker_CLASS(File, Blob, "File")
        
        static kk::Strong<File> open(kk::CString path,kk::CString name,kk::CString type,kk::Boolean isWritable);
        
        static kk::Strong<File> openURI(kk::CString uri,kk::CString type);

        static void mkdir(kk::CString dirname);
        
        static void Openlib();
        
    protected:
        File(kk::CString path, kk::Uint size,kk::CString type,kk::CString name,kk::Int lastModified,kk::Boolean isWritable);
        virtual kk::Boolean read(void * dest,kk::Uint offset,kk::Uint size);
        kk::String _path;
        kk::String _name;
        kk::Int _lastModified;
        kk::Boolean _isWritable;
        
    };
    
    enum FileReaderType {
        FileReaderTypeText,
        FileReaderTypeDataURL,
        FileReaderTypeArrayBuffer
    };
    
    enum  {
        FileReaderStateEMPTY,
        FileReaderStateLOADING,
        FileReaderStateDONE
    };
    
    typedef kk::Uint FileReaderState;
    
    class FileReader : public EventEmitter {
    public:
        FileReader();
        virtual ~FileReader();
        virtual void readAsText(kk::Blob * v);
        virtual void readAsDataURL(kk::Blob * v);
        virtual void readAsArrayBuffer(kk::Blob * v);
        virtual void abort();
        virtual JSObject * onload();
        virtual void setOnload(JSObject * func);
        virtual JSObject * onerror();
        virtual void setOnerror(JSObject * func);
        virtual kk::CString error();
        virtual FileReaderState readyState();
        virtual kk::Any & result();
        
        Ker_CLASS(FileReader, EventEmitter, "FileReader")
        
    protected:
        virtual void doError(kk::CString errmsg);
        virtual void doDone();
        virtual void read(kk::Blob * v,FileReaderType type);
        kk::Strong<DispatchSource> _fd;
        kk::Strong<DispatchQueue> _queue;
        kk::Any _result;
        kk::String _error;
        FileReaderState _readyState;
        kk::Strong<JSObject> _onload;
        kk::Strong<JSObject> _onerror;
    };
    
}

#endif /* file_h */
