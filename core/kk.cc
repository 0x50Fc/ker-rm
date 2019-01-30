//
//  kk.cc
//  kk
//
//  Created by zhanghailong on 2018/10/24.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <core/kk.h>
#include <typeinfo>
#include <string.h>

namespace kk {
    
    Object::Object(): _retainCount(0) {
        Zombies * z = Zombies::current();
        if(z != nullptr) {
            z->alloc(this);
        }
    }
    
    Object::~Object(){
        
        Zombies * z = Zombies::current();
        
        if(z != nullptr) {
            z->dealloc(this);
        }
        
    }
    
    String Object::toString() {
        return String(typeid(this).name());
    }
    
    void Object::release() {
        
        Zombies * z = Zombies::current();
        
        if(z != nullptr) {
            z->release(this);
        }
        
        Atomic * a = Atomic::current();
        if(a != nullptr) {
            a->lock();
        }
        _retainCount --;
        if(_retainCount == 0) {
            if(a != nullptr) {
                a->addObject(this);
            } else {
                delete this;
            }
        }
        if(a != nullptr) {
            a->unlock();
        }
    }
    
    void Object::retain() {
        
        Zombies * z = Zombies::current();
        
        if(z != nullptr) {
            z->retain(this);
        }
        
        Atomic * a = Atomic::current();
        if(a != nullptr) {
            a->lock();
        }
        _retainCount ++;
        if(a != nullptr) {
            a->unlock();
        }
    }
    
    int Object::retainCount() {
        return _retainCount;
    }
    
    void Object::weak(Object ** ptr) {
        
        assert(ptr != nullptr);
        
        Zombies * z = Zombies::current();
        
        if(z != nullptr) {
            z->weak(this, ptr);
        }
        
        Atomic * a = Atomic::current();
        if(a != nullptr) {
            a->weak(this, ptr);
        }
        
    }
    
    void Object::unWeak(Object ** ptr) {
        
        Zombies * z = Zombies::current();
        
        if(z != nullptr) {
            z->unWeak(this, ptr);
        }
        
        Atomic * a = Atomic::current();
        if(a != nullptr) {
            a->unweak(this, ptr);
        }
        
    }
    
#ifndef KER_ZOMBIES
    Zombies * Zombies::current() {
        return nullptr;
    }
#endif
    
    Atomic::Atomic(){
        pthread_mutex_init(&_lock, NULL);
        pthread_mutex_init(&_objectLock, NULL);
    }
    
    
    Atomic::~Atomic() {
        pthread_mutex_destroy(&_lock);
        pthread_mutex_destroy(&_objectLock);
    }
    
    void Atomic::lock() {
        pthread_mutex_lock(&_lock);
    }
    
    void Atomic::unlock() {
        
        pthread_mutex_unlock(&_lock);
        
        Object * v = nullptr;
        
        do {
            
            pthread_mutex_lock(&_objectLock);
            
            if(_objects.empty()) {
                v = nullptr;
            } else {
                v = _objects.front();
                _objects.pop();
            }
            
            if(v != nullptr) {
                auto i = _weakObjects.find(v);
                if(i != _weakObjects.end()) {
                    auto & q = i->second;
                    auto n = q.begin();
                    auto e = q.end();
                    while(n != e) {
                        Object ** ptr = * n;
                        * ptr = nullptr;
                        n ++;
                    }
                    _weakObjects.erase(i);
                }
            }
            
            pthread_mutex_unlock(&_objectLock);
            
            if(v != nullptr && v->retainCount() == 0) {
                delete v;
            }
            
        } while (v);
    }
    
    
    void Atomic::weak(Object * object,Object ** ptr) {
    
        pthread_mutex_lock(&_objectLock);
        
        auto i = _weakObjects.find(object);
        
        if(i == _weakObjects.end()) {
            _weakObjects[object] = {ptr};
        } else {
            i->second.insert(ptr);
        }
        
        pthread_mutex_unlock(&_objectLock);
    }
    
    void Atomic::unweak(Object * object,Object ** ptr) {
        
        pthread_mutex_lock(&_objectLock);
        
        auto i = _weakObjects.find(object);
        
        if(i != _weakObjects.end()) {
            auto & s = i->second;
            auto n = s.find(ptr);
            if(n != s.end()) {
                s.erase(n);
            }
        }
        
        pthread_mutex_unlock(&_objectLock);
        
    }
    
    void Atomic::addObject(Object * object) {
        pthread_mutex_lock(&_objectLock);
        _objects.push(object);
        pthread_mutex_unlock(&_objectLock);
    }
   
    
    Atomic * Atomic::current() {
        static Atomic * a = nullptr;
        if(a == nullptr) {
            a = new Atomic();
        }
        return a;
    }
    
    ArrayBuffer::ArrayBuffer(void * data,kk::Uint size) {
        _size = size;
        _data = (kk::Ubyte *) malloc(size);
        memcpy(_data,data, size);
    }
    
    ArrayBuffer::ArrayBuffer(kk::Uint size):_data(nullptr),_size(0) {
        if(size > 0) {
            _size = size;
            _data = (kk::Ubyte *) malloc(size);
            memset(_data, 0, size);
        }
    }
    
    ArrayBuffer::~ArrayBuffer() {
        if(_data) {
            free(_data);
        }
    }
    
    kk::Uint ArrayBuffer::byteLength() {
        return _size;
    }
    
    kk::Ubyte * ArrayBuffer::data() {
        return _data;
    }
    
    Buffer::Buffer():_data(nullptr),_size(sizeof(_buf)),_length(0) {
        
    }
    
    Buffer::~Buffer() {
        if(_data) {
            free(_data);
        }
    }
    
    kk::Uint Buffer::byteLength() {
        return _length;
    }
    
    kk::Uint Buffer::size() {
        return _size;
    }
    
    void Buffer::setByteLength(kk::Uint length) {
        _length = length;
    }
    
    kk::Ubyte * Buffer::data() {
        if(_data) {
            return _data;
        }
        return _buf;
    }
    
    void Buffer::capacity(kk::Uint size) {
        if(_size < size) {
            kk::Ubyte * b = data();
            if(_data) {
                _data = (kk::Ubyte *) realloc(_data, size);
            } else {
                _data = (kk::Ubyte *) malloc(size);
            }
            _size = size;
            if(b == _buf && _length > 0) {
                memcpy(_data, _buf,_length);
            }
        }
    }
    
    void Buffer::append(const void * p, kk::Uint size) {
        kk::Uint n = size > 1024 ? size : 1024;
        capacity(_length + n);
        kk::Ubyte * b = data();
        memcpy(b + _length, p, size);
        setByteLength(_length + (kk::Uint) size);
    }
    
    void Buffer::format(const char * format,...) {
        va_list ap;
        va_start(ap, format);
        formatv(format,ap);
        va_end(ap);
    }
    
    void Buffer::formatv(const char * format,va_list ap) {
        int n = vsnprintf(nullptr, 0, format, ap);
        capacity(_length + n);
        char * b = (char *) data();
        n = vsnprintf(b + _length, _size - _length, format, ap);
        setByteLength(_length + n);
    }
    
    void Buffer::append(Any v){
        if(v.type == TypeString) {
            append(v.stringValue, (kk::Uint) v.length);
        } else if(v.type == TypeObject) {
            ArrayBuffer * a = v.objectValue;
            if(a != nullptr) {
                append(a->data(), a->byteLength());
            }
        }
    }
    

    void Buffer::drain(kk::Uint length) {
        kk::Ubyte * b = data();
        kk::Uint s = byteLength() - length;
        if(s > 0) {
            memcpy(b, b + length, s);
        }
        setByteLength(s);
    }
    
    kk::String Buffer::toString() {
        kk::String v;
        v.append((char *) data(),byteLength());
        return v;
    }
    
    kk::CString Buffer::toCString(){
        capacity(byteLength() + 1);
        kk::Ubyte * b = data();
        b[byteLength()] = 0;
        return (kk::CString) b;
    }
    
    Ref::Ref():_object(nullptr) {
        
    }
    
    Object * Ref::get() {
        return _object;
    }

    Function::~Function() {
    }
    
    Any::~Any() {
        if(_data) {
            free(_data);
        }
    }
    
    Any::Any(const Any & v):Any() {
        type = v.type;
        objectValue = v.objectValue;
        stringValue = v.stringValue;
        length = v.length;
        if(v.type == TypeString) {
            setLString(v.stringValue, v.length);
        }
    }
    
    Any::Any():type(TypeNil),objectValue(nullptr),stringValue(nullptr),length(0),_size(0),_data(nullptr) {
        
    }
    
    Any::Any(Function * v):type(TypeFunction),objectValue(v),stringValue(nullptr),length(0),_size(0),_data(nullptr) {
        
    }
    
    Any::Any(Object * v):type(TypeObject),objectValue(v),stringValue(nullptr),length(0),_size(0),_data(nullptr){
        
    }
    
    Any::Any(Int8 v):Any() {
        type = TypeInt8;
        int8Value = v;
    }
    
    Any::Any(Uint8 v):Any() {
        type = TypeUint8;
        uint8Value = v;
    }
    
    Any::Any(Int16 v):Any()  {
        type = TypeInt16;
        int16Value = v;
    }
    
    Any::Any(Uint16 v):Any() {
        type = TypeUint16;
        uint16Value = v;
    }
    
    Any::Any(Int32 v):Any()  {
        type = TypeInt32;
        int32Value = v;
    }
    
    Any::Any(Uint32 v):Any() {
        type = TypeUint32;
        uint32Value = v;
    }
    
    Any::Any(Int64 v):Any()  {
        type = TypeInt64;
        int64Value = v;
    }
    
    Any::Any(Uint64 v):Any() {
        type = TypeUint64;
        uint64Value = v;
    }
    
    Any::Any(Boolean v):Any() {
        type = TypeBoolean;
        booleanValue = v;
    }
    
    Any::Any(Float32 v):Any()  {
        type = TypeFloat32;
        float32Value = v;
    }
    
    Any::Any(Float64 v):Any()  {
        type = TypeFloat64;
        float64Value = v;
    }
    
    Any::Any(String & v):Any() {
        type = TypeString;
        setLString(v.c_str(), v.size());
    }
    
    Any::Any(String v):Any() {
        type = TypeString;
        setLString(v.c_str(), v.size());
    }
    
    
    Any::Any(CString v):Any() {
        if(v != nullptr) {
            type = TypeString;
            setCString(v);
        }
    }
    
    Any::Any(Native * native):Any() {
        if(native != nullptr) {
            type = TypeObject;
            objectValue = new NativeObject(native);
        }
    }
    
    void Any::reset() {
        stringValue = nullptr;
        length = 0;
        objectValue = nullptr;
        type = TypeNil;
    }
    
    Any & Any::operator=(std::nullptr_t v) {
        reset();
        return * this;
    }
    
    Any & Any::operator=(Object *v) {
        reset();
        type = TypeObject;
        objectValue = v;
        return * this;
    }
    
    Any & Any::operator=(Function *v) {
        reset();
        type = TypeFunction;
        objectValue = v;
        return * this;
    }
    
    Any & Any::operator=(Int8 v) {
        reset();
        type = TypeInt8;
        int8Value = v;
        return * this;
    }
    
    Any & Any::operator=(Uint8 v) {
        reset();
        type = TypeUint8;
        uint8Value = v;
        return * this;
    }
    
    Any & Any::operator=(Int16 v) {
        reset();
        type = TypeInt16;
        int16Value = v;
        return * this;
    }
    
    Any & Any::operator=(Uint16 v){
        reset();
        type = TypeUint16;
        uint16Value = v;
        return * this;
    }
    
    Any & Any::operator=(Int32 v) {
        reset();
        type = TypeInt32;
        int32Value = v;
        return * this;
    }
    Any & Any::operator=(Uint32 v) {
        reset();
        type = TypeUint32;
        uint32Value = v;
        return * this;
    }
    
    Any & Any::operator=(Int64 v) {
        reset();
        type = TypeInt64;
        int64Value = v;
        return * this;
    }
    
    Any & Any::operator=(Uint64 v) {
        reset();
        type = TypeUint64;
        uint64Value = v;
        return * this;
    }
    
    Any & Any::operator=(Boolean v) {
        reset();
        type = TypeBoolean;
        booleanValue = v;
        return * this;
    }
    
    Any & Any::operator=(Float32 v) {
        reset();
        type = TypeFloat32;
        float32Value = v;
        return * this;
    }
    
    Any & Any::operator=(Float64 v) {
        reset();
        type = TypeFloat64;
        float64Value = v;
        return * this;
    }
    
    Any & Any::operator=(String & v) {
        reset();
        type = TypeString;
        setLString(v.c_str(), v.length());
        return * this;
    }
    Any & Any::operator=(const String & v) {
        reset();
        type = TypeString;
        setLString(v.c_str(), v.length());
        return * this;
    }
    Any & Any::operator=(CString v) {
        reset();
        if(v != nullptr) {
            type = TypeString;
            setCString(v);
        }
        return * this;
    }
    Any & Any::operator=(const Any & v) {
        reset();
        type = v.type;
        objectValue = v.objectValue;
        stringValue = v.stringValue;
        length = v.length;
        if(v.type == TypeString) {
            setLString(v.stringValue, v.length);
        }
        return * this;
    }
    
    Any & Any::operator=(Native * v) {
        reset();
        if(v != nullptr) {
            type = TypeObject;
            objectValue = new NativeObject(v);
        }
        return * this;
    }
    
    Any::operator CString () {
        switch(type) {
            case TypeInt8:
            {
                return this->sprintf("%d",int8Value);
            }
                break;
            case TypeUint8:
            {
                return this->sprintf( "%u",uint8Value);
            }
                break;
            case TypeInt16:
            {
                return this->sprintf("%d",int16Value);
            }
                break;
            case TypeUint16:
            {
                return this->sprintf("%u",uint16Value);
            }
                break;
            case TypeInt32:
            {
                return this->sprintf("%d",int32Value);
            }
                break;
            case TypeUint32:
            {
                return this->sprintf("%u",uint32Value);
            }
                break;
            case TypeInt64:
            {
                return this->sprintf("%lld",int64Value);
            }
                break;
            case TypeUint64:
            {
                return this->sprintf("%llu",uint64Value);
            }
                break;
            case TypeFloat32:
            {
                return this->sprintf("%g",float32Value);
            }
                break;
            case TypeFloat64:
            {
                return this->sprintf("%g",float64Value);
            }
                break;
            case TypeBoolean:
            {
                return this->sprintf("%s",booleanValue ? "true":"false");
            }
                break;
            case TypeObject:
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    kk::String s = NativeObject::stringValue(v->native());
                    return this->sprintf("%s",s.c_str());
                }
            }
                break;
            default:
                break;
        }
        return stringValue;
    }
    
    Any::operator Int8() {
        switch(type) {
            case TypeInt8:
                return int8Value;
            case TypeUint8:
                return uint8Value;
            case TypeInt16:
                return int16Value;
            case TypeUint16:
                return uint16Value;
            case TypeInt32:
                return int32Value;
            case TypeUint32:
                return uint32Value;
            case TypeInt64:
                return int64Value;
            case TypeUint64:
                return uint64Value;
            case TypeFloat32:
                return float32Value;
            case TypeFloat64:
                return float64Value;
            case TypeBoolean:
                return booleanValue;
            case TypeString:
                return atoi(stringValue);
            case TypeObject:
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    return NativeObject::intValue(v->native());
                }
            }
                break;
            default:
                break;
        }
        return 0;
    }
    Any::operator Uint8() {
        switch(type) {
            case TypeInt8:
                return int8Value;
            case TypeUint8:
                return uint8Value;
            case TypeInt16:
                return int16Value;
            case TypeUint16:
                return uint16Value;
            case TypeInt32:
                return int32Value;
            case TypeUint32:
                return uint32Value;
            case TypeInt64:
                return int64Value;
            case TypeUint64:
                return uint64Value;
            case TypeFloat32:
                return float32Value;
            case TypeFloat64:
                return float64Value;
            case TypeBoolean:
                return booleanValue;
            case TypeString:
                return atoi(stringValue);
            case TypeObject:
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    return NativeObject::intValue(v->native());
                }
            }
                break;
            default:
                break;
        }
        return 0;
    }
    Any::operator Int16() {
        switch(type) {
            case TypeInt8:
                return int8Value;
            case TypeUint8:
                return uint8Value;
            case TypeInt16:
                return int16Value;
            case TypeUint16:
                return uint16Value;
            case TypeInt32:
                return int32Value;
            case TypeUint32:
                return uint32Value;
            case TypeInt64:
                return int64Value;
            case TypeUint64:
                return uint64Value;
            case TypeFloat32:
                return float32Value;
            case TypeFloat64:
                return float64Value;
            case TypeBoolean:
                return booleanValue;
            case TypeString:
                return atoi(stringValue);
            case TypeObject:
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    return NativeObject::intValue(v->native());
                }
            }
                break;
            default:
                break;
        }
        return 0;
    }
    Any::operator Uint16() {
        switch(type) {
            case TypeInt8:
                return int8Value;
            case TypeUint8:
                return uint8Value;
            case TypeInt16:
                return int16Value;
            case TypeUint16:
                return uint16Value;
            case TypeInt32:
                return int32Value;
            case TypeUint32:
                return uint32Value;
            case TypeInt64:
                return int64Value;
            case TypeUint64:
                return uint64Value;
            case TypeFloat32:
                return float32Value;
            case TypeFloat64:
                return float64Value;
            case TypeBoolean:
                return booleanValue;
            case TypeString:
                return atoi(stringValue);
            case TypeObject:
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    return NativeObject::intValue(v->native());
                }
            }
                break;
            default:
                break;
        }
        return 0;
    }
    Any::operator Int32() {
        switch(type) {
            case TypeInt8:
                return int8Value;
            case TypeUint8:
                return uint8Value;
            case TypeInt16:
                return int16Value;
            case TypeUint16:
                return uint16Value;
            case TypeInt32:
                return int32Value;
            case TypeUint32:
                return uint32Value;
            case TypeInt64:
                return (Int32)int64Value;
            case TypeUint64:
                return (Int32)uint64Value;
            case TypeFloat32:
                return float32Value;
            case TypeFloat64:
                return float64Value;
            case TypeBoolean:
                return booleanValue;
            case TypeString:
                return atoi(stringValue);
            case TypeObject:
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    return NativeObject::intValue(v->native());
                }
            }
                break;
            default:
                break;
        }
        return 0;
    }
    Any::operator Uint32() {
        switch(type) {
            case TypeInt8:
                return int8Value;
            case TypeUint8:
                return uint8Value;
            case TypeInt16:
                return int16Value;
            case TypeUint16:
                return uint16Value;
            case TypeInt32:
                return int32Value;
            case TypeUint32:
                return uint32Value;
            case TypeInt64:
                return (Uint32) int64Value;
            case TypeUint64:
                return (Uint32) uint64Value;
            case TypeFloat32:
                return float32Value;
            case TypeFloat64:
                return float64Value;
            case TypeBoolean:
                return booleanValue;
            case TypeString:
                return atoi(stringValue);
            case TypeObject:
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    return NativeObject::intValue(v->native());
                }
            }
                break;
            default:
                break;
        }
        return 0;
    }
    Any::operator Int64() {
        switch(type) {
            case TypeInt8:
                return int8Value;
            case TypeUint8:
                return uint8Value;
            case TypeInt16:
                return int16Value;
            case TypeUint16:
                return uint16Value;
            case TypeInt32:
                return int32Value;
            case TypeUint32:
                return uint32Value;
            case TypeInt64:
                return int64Value;
            case TypeUint64:
                return uint64Value;
            case TypeFloat32:
                return float32Value;
            case TypeFloat64:
                return float64Value;
            case TypeBoolean:
                return booleanValue;
            case TypeString:
                return atoll(stringValue);
            case TypeObject:
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    return NativeObject::int64Value(v->native());
                }
            }
                break;
            default:
                break;
        }
        return 0;
    }
    Any::operator Uint64() {
        switch(type) {
            case TypeInt8:
                return int8Value;
            case TypeUint8:
                return uint8Value;
            case TypeInt16:
                return int16Value;
            case TypeUint16:
                return uint16Value;
            case TypeInt32:
                return int32Value;
            case TypeUint32:
                return uint32Value;
            case TypeInt64:
                return int64Value;
            case TypeUint64:
                return uint64Value;
            case TypeFloat32:
                return float32Value;
            case TypeFloat64:
                return float64Value;
            case TypeBoolean:
                return booleanValue;
            case TypeString:
                return atoll(stringValue);
            case TypeObject:
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    return NativeObject::int64Value(v->native());
                }
            }
                break;
            default:
                break;
        }
        return 0;
    }
    Any::operator Float32() {
        switch(type) {
            case TypeInt8:
                return int8Value;
            case TypeUint8:
                return uint8Value;
            case TypeInt16:
                return int16Value;
            case TypeUint16:
                return uint16Value;
            case TypeInt32:
                return int32Value;
            case TypeUint32:
                return uint32Value;
            case TypeInt64:
                return int64Value;
            case TypeUint64:
                return uint64Value;
            case TypeFloat32:
                return float32Value;
            case TypeFloat64:
                return float64Value;
            case TypeBoolean:
                return booleanValue;
            case TypeString:
                return atof(stringValue);
            case TypeObject:
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    return NativeObject::doubleValue(v->native());
                }
            }
                break;
            default:
                break;
        }
        return 0;
    }
    Any::operator Float64() {
        switch(type) {
            case TypeInt8:
                return int8Value;
            case TypeUint8:
                return uint8Value;
            case TypeInt16:
                return int16Value;
            case TypeUint16:
                return uint16Value;
            case TypeInt32:
                return int32Value;
            case TypeUint32:
                return uint32Value;
            case TypeInt64:
                return int64Value;
            case TypeUint64:
                return uint64Value;
            case TypeFloat32:
                return float32Value;
            case TypeFloat64:
                return float64Value;
            case TypeBoolean:
                return booleanValue;
            case TypeString:
                return atof(stringValue);
            case TypeObject:
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    return NativeObject::doubleValue(v->native());
                }
            }
                break;
            default:
                break;
        }
        return 0;
    }
    Any::operator Boolean() {
        switch(type) {
            case TypeInt8:
                return int8Value != 0;
            case TypeUint8:
                return uint8Value !=0 ;
            case TypeInt16:
                return int16Value != 0;
            case TypeUint16:
                return uint16Value != 0;
            case TypeInt32:
                return int32Value != 0;
            case TypeUint32:
                return uint32Value != 0;
            case TypeInt64:
                return int64Value != 0;
            case TypeUint64:
                return uint64Value != 0;
            case TypeFloat32:
                return float32Value != 0;
            case TypeFloat64:
                return float64Value != 0;
            case TypeBoolean:
                return booleanValue;
            case TypeString:
                return length > 0;
            case TypeObject:
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    return NativeObject::booleanValue(v->native());
                }
            }
                return objectValue.get() != nullptr;
            default:
                return false;
        }
    }
    
    Any::operator String() {
        return (CString) (* this);
    }
    
    Any::operator Object*() {
        return objectValue.get();
    }
    
    Any::operator Function*() {
        return dynamic_cast<Function *>(objectValue.get());
    }
    
    
    CString Any::sprintf(CString format,...) {
        
        va_list va;
        size_t n;
        
        va_start(va,format);
        n = vsnprintf(nullptr, 0, format, va);
        va_end(va);
        
        
        
        if(_size == 0) {
            _size = n;
            _data = malloc(_size);
        } else if(_size < n) {
            _size = n;
            _data = realloc(_data, _size);
        }
        
        va_start(va,format);
        
        vsnprintf((char *)_data, _size, format, va);
        
        va_end(va);
        
        return (CString) _data;
    }
    
    void Any::setCString(CString string) {
        setLString(string, string ?  strlen(string) : 0);
    }
    
    void Any::setLString(CString string,size_t len) {
        
        if(string) {
            
            if(_size == 0) {
                _size = len + 1;
                _data = malloc(_size);
            } else if(_size < len + 1) {
                _size = len + 1;
                _data = realloc(_data, _size);
            }
            
            memcpy(_data, string, len + 1);
            
            stringValue = (CString) _data;
            length = len;
            
        } else if(_size > 0){
            * (char *) _data = 0;
            length = 0;
        } else {
            length = 0;
        }
    }
    
    Any Any::copy() {
        Any v(*this);
        Copying * object = dynamic_cast<Copying *>( v.objectValue.get() );
        if(object != nullptr) {
            kk::Strong<Object> o = object->copy();
            v = o.get();
        }
        return v;
    }
    
    void Any::get(kk::CString key,Any & value) {
        if(type == TypeObject && key != nullptr) {
            {
                _TObject * v = objectValue;
                if(v != nullptr) {
                    Any k = key;
                    v->get(k, value);
                    return;
                }
            }
            {
                NativeObject * v = objectValue;
                if(v != nullptr) {
                    kk::Strong<NativeObject> r = NativeObject::get(v->native(), key);
                    value = (NativeObject *) r;
                    return;
                }
            }
            {
                Getter * v = objectValue;
                if(v != nullptr) {
                    v->get(key, value);
                    return;
                }
            }
        } else {
            value = nullptr;
        }
    }
    
#if defined(__APPLE__)
    
#elif defined(__ANDROID__)
    
#else
    
    void LogV(const char * format, va_list va) {
        
        time_t now = time(NULL);
        
        char data[2048];
        
        struct tm * p = localtime(&now);
        
        size_t n = snprintf(data,sizeof(data), "[Ker] [%04d-%02d-%02d %02d:%02d:%02d] ",1900 + p->tm_year,p->tm_mon + 1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
        
        n += vsnprintf(data + n, sizeof(data) - n ,format, va);
        
        printf("%s\n",data);
        
    }
    
#endif
    
    
    void Log(const char * format, ...) {
        va_list va;
        va_start(va, format);
        LogV(format, va);
        va_end(va);
    }
    
    Boolean CStringHasPrefix(CString string,CString prefix) {
        
        if(string == prefix) {
            return true;
        }
        
        if(prefix == NULL) {
            return true;
        }
        
        if(string == NULL) {
            return false;
        }
        
        size_t n1 = strlen(string);
        size_t n2 = strlen(prefix);
        
        return n1 >= n2 && strncmp(string, prefix, n2) == 0;
    }

    String CStringPathNormalize(CString path) {
        std::vector<kk::String> vs;
        CStringSplit(path, "/", vs);
        auto i = vs.begin();
        while(i != vs.end()) {
            auto & v = * i;
            if(kk::CStringEqual(v.c_str(), ".")) {
                i = vs.erase(i);
                continue;
            } else if(kk::CStringEqual(v.c_str(), "..")) {
                i = vs.erase(i);
                if(i != vs.begin()) {
                    i = vs.erase(i - 1);
                }
                continue;
            } else if(kk::CStringEqual(v.c_str(), "") && i != vs.begin()) {
                i = vs.erase(i);
                continue;
            }
            i ++;
        }
        return CStringJoin(vs, "/");
    }
    
    String CStringPathDirname(CString path) {
        String p;
        
        if(path == nullptr) {
            return p;
        }
        
        p.append(path);
        
        auto i = p.find_last_of("/");
        
        if(i != std::string::npos) {
            return p.substr(0,i);
        }
        
        return "";
    }
    
    CString CStringPathBasename(CString path) {
        
        if(path == nullptr) {
            return nullptr;
        }
        char * b = (char *) path;
        char * p = b;
        while(*p) {
            if(*p == '/') {
                b = p + 1;
            }
            p ++;
        }
        return b;
    }
    
    Boolean CStringHasSubstring(CString string,CString substr) {

        if(string == substr) {
            return true;
        }

        if(substr == nullptr) {
            return true;
        }

        if(string == nullptr) {
            return true;
        }

        size_t n1 = strlen(string);
        size_t n2 = strlen(substr);

        if(n1 < n2) {
            return false;
        }

        char * p = (char *) string;

        while(p && *p) {
            char * b = (char *) substr;
            char * i = p;
            while(*b && *i && *b == *i) {
                b ++;
                i ++;
            }
            if(*b == 0) {
                return true;
            }
            p ++;
        }

        return false;
    }
    
    Boolean CStringHasSuffix(CString string,CString suffix) {
        
        if(string == suffix) {
            return true;
        }
        
        if(suffix == NULL) {
            return true;
        }
        
        if(string == NULL) {
            return false;
        }
        
        size_t n1 = strlen(string);
        size_t n2 = strlen(suffix);
        
        return n1 >= n2 && strncmp(string + n1 - n2, suffix, n2) == 0;
    }
    
    size_t CStringLength(CString string){
        if(string == NULL) {
            return 0;
        }
        return strlen(string);
    }
    
    Boolean CStringEqual(CString string,CString value) {
        
        if(string == value) {
            return true;
        }
        
        if(value == NULL || string == NULL ) {
            return false;
        }
        
        return strcmp(string, value) == 0;
    }
    
    void CStringSplit(CString string,CString delim, std::vector<String>& items) {
        
        if(string == NULL || delim == NULL) {
            return ;
        }
        
        char * p = (char *) string;
        char * b = p;
        size_t n= strlen(delim);
        
        while(*b != 0) {
            if(strncmp(b, delim, n) == 0) {
                items.push_back(String(p,b-p));
                b += n;
                p = b;
            } else {
                b ++;
            }
        }
        
        if(b != p) {
            items.push_back(String(p,b-p));
        }
        
    }
    
    void CStringSplit(CString string,CString delim, std::set<String>& items) {
        
        if(string == NULL || delim == NULL) {
            return ;
        }
        
        char * p = (char *) string;
        char * b = p;
        size_t n= strlen(delim);
        
        while(*b != 0) {
            if(strncmp(b, delim, n) == 0) {
                items.insert(String(p,b-p));
                b += n;
                p = b;
            } else {
                b ++;
            }
        }
        
        if(b != p) {
            items.insert(String(p,b-p));
        }
        
    }
    
    
    String CStringJoin(std::vector<String>& items,CString delim){
        String ss;
        std::vector<String>::iterator i = items.begin();
        
        while(i !=items.end()){
            if(i !=items.begin()){
                ss.append(delim);
            }
            ss.append(*i);
            i ++;
        }
        
        return ss;
    }
    
    String CStringJoin(std::set<String>& items,CString delim){
        String ss;
        std::set<String>::iterator i = items.begin();
        
        while(i !=items.end()){
            if(i !=items.begin()){
                ss.append(delim);
            }
            ss.append(*i);
            i ++;
        }
        
        return ss;
    }
    
    
    String& CStringTrim(String& s){
        
        if (s.empty())
        {
            return s;
        }
        
        s.erase(0,s.find_first_not_of(" "));
        s.erase(s.find_last_not_of(" ") + 1);
        return s;
    }
    
    String CStringPathAppend(CString basePath,CString path) {
        
        if(basePath == nullptr) {
            return path;
        }
        
        size_t n = strlen(basePath);
        
        if(n == 0) {
            return path;
        }
        
        if(basePath[n - 1] == '/') {
            return String(basePath) + path;
        }
        
        return String(basePath) + "/" + path;
    }
    
    String CStringPathDeleteLast(CString path) {
        
        if(path == nullptr) {
            return "";
        }
        kk::String v = path;
        
        size_t n = v.find_last_of("/");
        
        if(n == kk::String::npos) {
            return v;
        }
        
        return v.substr(0,n + 1);
        
    }
    
    String CStringPathDeleteExtension(CString path) {
        
        if(path == nullptr) {
            return "";
        }
        
        kk::String v = path;
        
        size_t n = v.find_last_of(".");
        
        if(n == kk::String::npos) {
            return v;
        }
        
        return v.substr(0,n);
    }
    
    kk::Int32 NativeObject::intValue(Native * object,kk::CString key,kk::Int32 defaultValue) {
        
        if(object == nullptr || key == nullptr) {
            return defaultValue;
        }
        
        kk::Strong<NativeObject> v = NativeObject::get(object, key);
        
        if(v == nullptr) {
            return defaultValue;
        }
        
        return NativeObject::intValue(v->native());
    }
    
    kk::Int64 NativeObject::int64Value(Native * object,kk::CString key,kk::Int64 defaultValue) {
        
        if(object == nullptr || key == nullptr) {
            return defaultValue;
        }
        
        kk::Strong<NativeObject> v = NativeObject::get(object, key);
        
        if(v == nullptr) {
            return defaultValue;
        }
        
        return NativeObject::int64Value(v->native());
    }
    
    kk::Double NativeObject::doubleValue(Native * object,kk::CString key,kk::Double defaultValue) {
        
        if(object == nullptr || key == nullptr) {
            return defaultValue;
        }
        
        kk::Strong<NativeObject> v = NativeObject::get(object, key);
        
        if(v == nullptr) {
            return defaultValue;
        }
        
        return NativeObject::doubleValue(v->native());
    }
    
    kk::Boolean NativeObject::booleanValue(Native * object,kk::CString key,kk::Boolean defaultValue) {
        if(object == nullptr || key == nullptr) {
            return defaultValue;
        }
        
        kk::Strong<NativeObject> v = NativeObject::get(object, key);
        
        if(v == nullptr) {
            return defaultValue;
        }
        
        return NativeObject::booleanValue(v->native());
    }
    
    kk::String NativeObject::stringValue(Native * object,kk::CString key,kk::CString defaultValue) {
        
        if(object && key) {
            
            kk::Strong<NativeObject> v = NativeObject::get(object, key);
            
            if(v != nullptr) {
                return NativeObject::stringValue(v->native());
            }
            
        }
        
        if(defaultValue != nullptr) {
            return defaultValue;
        }
        
        return kk::String();
    }
    
    NativeValue::NativeValue(Native * native):NativeObject(native) {
        
    }
    
    PointerObject::PointerObject(void * pointer,std::function<void(void*)> && release):_pointer(pointer),_release(release) {
        
    }
    
    PointerObject::~PointerObject() {
        if(_release != nullptr) {
            _release(_pointer);
        }
    }
    
    void * PointerObject::pointer() {
        return _pointer;
    }
    
}

