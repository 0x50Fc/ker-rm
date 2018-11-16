//
//  kk.cc
//  kk
//
//  Created by zhanghailong on 2018/10/24.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <core/kk.h>
#include <typeinfo>

namespace kk {
    
    Object::Object(): _retainCount(0) {
    }
    
    Object::~Object(){
        
        Atomic * a = Atomic::current();
        
        if(a != nullptr) {
            a->lock();
        }
        
        std::set<Object **>::iterator i =_weakObjects.begin();
        
        while(i != _weakObjects.end()) {
            Object ** v = * i;
            if(v) {
                *v = NULL;
            }
            i ++;
        }
        
        if(a != nullptr) {
            a->unlock();
        }
     
    }
    
    String Object::toString() {
        return String(typeid(this).name());
    }
    
    void Object::release() {
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
        Atomic * a = Atomic::current();
        if(a != nullptr) {
            a->lock();
        }
        _weakObjects.insert(ptr);
        if(a != nullptr) {
            a->unlock();
        }
    }
    
    void Object::unWeak(Object ** ptr) {
        Atomic * a = Atomic::current();
        if(a != nullptr) {
            a->lock();
        }
        std::set<Object **>::iterator i = _weakObjects.find(ptr);
        if(i != _weakObjects.end()) {
            _weakObjects.erase(i);
        }
        if(a != nullptr) {
            a->unlock();
        }
    }
    
    Atomic::Atomic(){
        
    }
    
    
    Atomic::~Atomic() {
        
    }
    
    void Atomic::lock() {
        _lock.lock();
    }
    
    void Atomic::unlock() {
        
        _lock.unlock();
        
        Object * v = nullptr;
        
        do {
            
            _objectLock.lock();
            
            if(_objects.empty()) {
                v = nullptr;
            } else {
                v = _objects.front();
                _objects.pop();
            }
            
            _objectLock.unlock();
            
            if(v != nullptr && v->retainCount() == 0) {
                delete v;
            }
            
        } while (v);
    }
    
    void Atomic::addObject(Object * object) {
        _objectLock.lock();
        _objects.push(object);
        _objectLock.unlock();
    }
   
    
    Atomic * Atomic::current() {
        static Atomic * a = nullptr;
        if(a == nullptr) {
            a = new Atomic();
        }
        return a;
    }
    
    ArrayBuffer::ArrayBuffer(void * data,kk::Uint size):_data(nullptr),_size(0) {
        if(size > 0) {
            _size = size;
            _data = malloc(size);
            memcpy(_data, data, size);
        }
    }
    
    ArrayBuffer::ArrayBuffer(kk::Uint size):_data(nullptr),_size(0) {
        if(size > 0) {
            _size = size;
            _data = malloc(size);
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
    
    void * ArrayBuffer::data() {
        return _data;
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
            default:
                return objectValue.get() != nullptr;
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
        size_t n = 255;
        
        do {
            
            va_start(va,format);
            
            if(_size == 0) {
                _size = n;
                _data = malloc(_size);
            } else if(_size < n) {
                _size = n;
                _data = realloc(_data, _size);
            }
            
            n = vsnprintf((char *)_data, _size, format, va);
            
            va_end(va);
            
        } while(n > _size);
        
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
            } else if(_size < length + 1) {
                _size = len + 1;
                _data = realloc(_data, _size);
            }
            
            memcpy(_data, string, len + 1);
            
            stringValue = (CString) _data;
            length = len;
            
        } else if(_size > 0){
            * (char *) _data = 0;
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
    
    void LogV(const char * format, va_list va) {
        
        time_t now = time(NULL);
        
        char data[2048];
        
        struct tm * p = localtime(&now);
        
        size_t n = snprintf(data,sizeof(data), "[KK] [%04d-%02d-%02d %02d:%02d:%02d] ",1900 + p->tm_year,p->tm_mon + 1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
        
        n += vsnprintf(data + n, sizeof(data) - n ,format, va);
        
        printf("%s\n",data);
        
    }
    
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
}

