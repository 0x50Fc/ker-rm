//
//  archiver.c
//  Ker
//
//  Created by zhanghailong on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "archiver.h"
#include "bio.h"

namespace kk {
 
    void Archiver::beginObject() {
        encodeValue(ArchiverTypeObject,nullptr,0);
    }
    
    void Archiver::endObject() {
        encodeValue(ArchiverTypeDone,nullptr,0);
    }
    
    void Archiver::beginArray() {
        encodeValue(ArchiverTypeArray,nullptr,0);
    }
    
    void Archiver::endArray() {
        encodeValue(ArchiverTypeDone,nullptr,0);
    }
    

    void Archiver::encodeNil() {
        encodeValue(ArchiverTypeNil,nullptr,0);
    }
    
    void Archiver::encode(kk::CString v) {
        if(v == nullptr) {
            encodeValue(ArchiverTypeNil,nullptr,0);
        } else {
            encodeValue(ArchiverTypeString,v,kk::CStringLength(v) + 1);
        }
    }
    
    void Archiver::encode(const void * data,size_t size) {
        if(data == nullptr || size <= 0) {
            encodeValue(ArchiverTypeNil,nullptr,0);
        } else {
            encodeValue(ArchiverTypeByteArray, data, size);
        }
    }
    
    
    void Archiver::encode(kk::Int32 v) {
        Byte m[Bio::Int32_Size];
        size_t n = Bio::encode(v, m, sizeof(m));
        encodeValue(ArchiverTypeInt32,m,n);
    }
    
   
    void Archiver::encode(kk::Int64 v) {
        Byte m[Bio::Int64_Size];
        size_t n = Bio::encode(v, m, sizeof(m));
        encodeValue(ArchiverTypeInt64,m,n);
    }
    
    void Archiver::encode(kk::Float v) {
        Byte m[Bio::Float_Size];
        size_t n = Bio::encode(v, m, sizeof(m));
        encodeValue(ArchiverTypeFloat,m,n);
    }
    
    void Archiver::encode(kk::Double v) {
        Byte m[Bio::Double_Size];
        size_t n = Bio::encode(v, m, sizeof(m));
        encodeValue(ArchiverTypeDouble,m,n);
    }
    
    void Archiver::encode(kk::Boolean v) {
        Byte m = v ? 1 : 0;
        encodeValue(ArchiverTypeBoolean, &m, 1);
    }
    
    void Archiver::encode(ArchiverObject * object) {
        if(object == nullptr) {
            encodeNil();
        } else {
            beginObject();
            object->encode(this);
            endObject();
        }
    }
    
    UnArchiver::UnArchiver():_index(0),_length(0) {
        
    }
    
    void UnArchiver::decode(ArchiverObject * object) {
        
        if(object != nullptr) {
            object->decode(this);
        }
        
        endObject();
    }
    
    void UnArchiver::endObject() {
        
        ArchiverType type = ArchiverTypeDone;
        
        int l = 1;
        
        do {
            type = next();
            if(type == ArchiverTypeObject || type == ArchiverTypeArray) {
                l ++;
            } else if(type == ArchiverTypeDone) {
                l --;
            }
        } while(type != ArchiverTypeDone || l != 0);
        
    }
    
    ArchiverType UnArchiver::next() {
        Byte * b = get(_index);
        
        if(b == nullptr) {
            return ArchiverTypeDone;
        }
        
        ArchiverType type = (ArchiverType) b[0];
        b += Bio::Byte_Size;
        Bio::decode(& _length, b + Bio::Byte_Size, Bio::Int32_Size);
        b += Bio::Int32_Size;
        switch (type) {
            case ArchiverTypeInt32:
                Bio::decode(&int32Value, b, _length);
                break;
            case ArchiverTypeInt64:
                Bio::decode(&int64Value, b, _length);
                break;
            case ArchiverTypeFloat:
                Bio::decode(&floatValue, b, _length);
                break;
            case ArchiverTypeDouble:
                Bio::decode(&doubleValue, b, _length);
                break;
            case ArchiverTypeBoolean:
                booleanValue = b[0] != 0;
                break;
            case ArchiverTypeString:
                stringValue = b;
                break;
            case ArchiverTypeByteArray:
                byteArrayValue = b;
                break;
            default:
                int64Value = 0;
                break;
        }
        return type;
    }
    
    kk::Int32 UnArchiver::length(){
        return _length;
    }
    
    
    const void * MemArchiver::data(size_t * size) {
        if(size) {
            * size = _buffer.byteLength();
        }
        return _buffer.data();
    }
    
    void MemArchiver::encodeValue(ArchiverType type,const void * data, size_t size) {
        
        kk::Uint n = (kk::Uint) size + (kk::Uint) Bio::Byte_Size + (kk::Uint) Bio::Int32_Size;
        
        _buffer.capacity(_buffer.byteLength() + n);
        
        Byte * b = (Byte *) _buffer.data() + _buffer.byteLength();
        
        _buffer.setByteLength(_buffer.byteLength() + n);
        
        Bio::encode((Byte) type, b, Bio::Byte_Size);
        Bio::encode((kk::Int32) size, b + Bio::Byte_Size, Bio::Int32_Size);
        
        if(size > 0) {
            if(data == nullptr) {
                memset(b + Bio::Byte_Size + Bio::Int32_Size, 0, size);
            } else {
                memcpy(b + Bio::Byte_Size + Bio::Int32_Size, data, size);
            }
        }
        
    }
    
    MemUnArchiver::MemUnArchiver(void * data,Uint size):MemUnArchiver(new kk::ArrayBuffer(data,size)) {
        
    }
    
    MemUnArchiver::MemUnArchiver(ArrayBuffer * data):UnArchiver(),_data(data) {
        
    }

    Byte * MemUnArchiver::get(size_t index) {
        if(_data == nullptr) {
            return nullptr;
        }
        if(index < _data->byteLength()) {
            return (Byte *) _data->data() + index;
        }
        return nullptr;
    }
    
}
