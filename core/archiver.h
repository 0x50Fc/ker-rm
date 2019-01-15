//
//  archiver.h
//  Ker
//
//  Created by hailong11 on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef kk_archiver_h
#define kk_archiver_h

#include <core/kk.h>

namespace kk {
    
    enum {
        ArchiverTypeNil,
        ArchiverTypeString,
        ArchiverTypeByteArray,
        ArchiverTypeInt32,
        ArchiverTypeInt64,
        ArchiverTypeFloat,
        ArchiverTypeDouble,
        ArchiverTypeBoolean,
        ArchiverTypeObject,
        ArchiverTypeArray,
        ArchiverTypeDone
    };
    
    typedef kk::Ubyte ArchiverType;
    
    class Archiver;
    class UnArchiver;
    
    class ArchiverObject {
    public:
        virtual void encode(Archiver * archiver) = 0;
        virtual void decode(UnArchiver * unArchiver) = 0;
    };
    
    class Archiver : public Object {
    public:
        virtual void beginObject();
        virtual void endObject();
        virtual void beginArray();
        virtual void endArray();
        virtual void encodeNil();
        virtual void encode(kk::CString v);
        virtual void encode(const void * data,size_t size);
        virtual void encode(kk::Int32 v);
        virtual void encode(kk::Int64 v);
        virtual void encode(kk::Float v);
        virtual void encode(kk::Double v);
        virtual void encode(kk::Boolean v);
        virtual void encode(ArchiverObject * object);
    protected:
        virtual void encodeValue(ArchiverType type,const void * data, size_t size) = 0;
    };
    
    class UnArchiver : public Object {
    public:
        UnArchiver();
        virtual ArchiverType next();
        virtual void endObject();
        union {
            kk::CString stringValue;
            kk::Int32 int32Value;
            kk::Int64 int64Value;
            kk::Float floatValue;
            kk::Double doubleValue;
            kk::Boolean booleanValue;
            kk::Byte * byteArrayValue;
        };
        virtual kk::Int32 length();
        virtual void decode(ArchiverObject * object);
    protected:
        virtual Byte * get(size_t index) = 0;
        ssize_t _index;
        kk::Int32 _length;
    };
    
    class MemArchiver : public Archiver {
    public:
        virtual const void * data(size_t * size);
    protected:
        virtual void encodeValue(ArchiverType type,const void * data, size_t size);
        Buffer _buffer;
    };
    
    class MemUnArchiver : public UnArchiver {
    public:
        MemUnArchiver(void * data,kk::Uint size);
        MemUnArchiver(ArrayBuffer * data);
    protected:
        kk::Strong<ArrayBuffer> _data;
        virtual Byte * get(size_t index);
    };
    
    
}

#endif /* archiver_h */
