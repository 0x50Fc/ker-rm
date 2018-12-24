//
//  jit.cpp
//  kk
//
//  Created by zhanghailong on 2018/10/24.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <core/jit.h>
#include <thread>

namespace kk {
    
    JITContext::JITContext() {
        
    }
    
    JITContext::~JITContext() {
        Log("[JITContext] [dealloc]");
    }
    
    void JITContext::strong(duk_context * ctx, void * heapptr, Object * object) {
        
        auto i = _strongObjects.find(heapptr);
        
        if(i == _strongObjects.end()) {
            
            _strongObjects[heapptr] = object;
            
            auto n = _objectsWithObject.find(object);
            
            if(n == _objectsWithObject.end()) {
                _objectsWithObject[object] = {};
                n = _objectsWithObject.find(object);
            }
            
            n->second[ctx] = heapptr;
            
        }
        
    }
    
    void JITContext::weak(duk_context * ctx, void * heapptr, Object * object) {
        
        auto i = _weakObjects.find(heapptr);
        
        if(i == _weakObjects.end()) {
            
            _weakObjects[heapptr] = object;
            
            auto n = _objectsWithObject.find(object);
            
            if(n == _objectsWithObject.end()) {
                _objectsWithObject[object] = {};
                n = _objectsWithObject.find(object);
            }
            
            n->second[ctx] = heapptr;
            
        }
        
    }
    
    void JITContext::remove(void * heapptr) {
        
        {
            auto i = _weakObjects.find(heapptr);
            
            if(i != _weakObjects.end()) {
                Object * v = i->second;
                auto n = _objectsWithObject.find(v);
                if(n != _objectsWithObject.end()) {
                    _objectsWithObject.erase(n);
                }
                _weakObjects.erase(i);
            }
        }
        
        {
            auto i = _strongObjects.find(heapptr);
            
            if(i != _strongObjects.end()) {
                Object * v = i->second;
                auto n = _objectsWithObject.find(v);
                if(n != _objectsWithObject.end()) {
                    _objectsWithObject.erase(n);
                }
                _strongObjects.erase(i);
            }
        }
    }
    
    void JITContext::remove(duk_context * ctx) {
        
        auto i = _objectsWithObject.begin();
        
        while(i != _objectsWithObject.end()) {
            
            auto & m = i->second;
            auto n = m.find(ctx);
            if (n != m.end()) {
                void * v = n->second;
                {
                    auto j = _weakObjects.find(v);
                    if(j != _weakObjects.end()) {
                        _weakObjects.erase(j);
                    }
                }
                {
                    auto j = _strongObjects.find(v);
                    if(j != _strongObjects.end()) {
                        _strongObjects.erase(j);
                    }
                }
                m.erase(n);
            }
            
            i ++;
        }
        
    }
    
    void JITContext::remove(Object * object) {
        
        auto i = _objectsWithObject.find(object);
        
        if(i != _objectsWithObject.end()) {
            
            auto & m = i->second;
            auto n = m.begin();
            while (n != m.end()) {
                void * v = n->second;
                {
                    auto j = _weakObjects.find(v);
                    if(j != _weakObjects.end()) {
                        _weakObjects.erase(j);
                    }
                }
                {
                    auto j = _strongObjects.find(v);
                    if(j != _strongObjects.end()) {
                        _strongObjects.erase(j);
                    }
                }
                
                n ++;
            }
            
            _objectsWithObject.erase(i);
        }
        
    }
    
    Object * JITContext::get(void * heapptr) {
        
        {
            auto j = _weakObjects.find(heapptr);
            if(j != _weakObjects.end()) {
                return j->second;
            }
        }
        {
            auto j = _strongObjects.find(heapptr);
            if(j != _strongObjects.end()) {
                return j->second;
            }
        }
        
        return nullptr;
    }
    
    void * JITContext::get(Object * object,duk_context * ctx) {
        
        auto i = _objectsWithObject.find(object);
        
        if(i != _objectsWithObject.end()) {
            auto & m = i->second;
            auto n = m.find(ctx);
            if(n != m.end()) {
                return n->second;
            }
        }
        
        return nullptr;
    }
    
    void JITContext::forEach(Object * object,std::function<void(duk_context * ,void *)> && func) {
        auto i = _objectsWithObject.find(object);
        if(i != _objectsWithObject.end()) {
            auto & m = i->second;
            auto j = m.begin();
            while(j != m.end()) {
                func(j->first,j->second);
                j ++;
            }
        }
    }
    
    static pthread_key_t gJITContextKey = 0;
    
    static void gJITContextKeyDealloc(void * p) {
        JITContext * v = (JITContext *) p;
        v->release();
    }
    
    JITContext * JITContext::current(){
        if(gJITContextKey == 0) {
            pthread_key_create(&gJITContextKey, gJITContextKeyDealloc);
        }
        JITContext * v = (JITContext *) pthread_getspecific(gJITContextKey);
        if(v == nullptr) {
            v = new JITContext();
            v->retain();
            pthread_setspecific(gJITContextKey, v);
        }
        return v;
    }
    
    
    static duk_ret_t Object_dealloc(duk_context * ctx) {
        
        void * heapptr = duk_get_heapptr(ctx, -1);
        
        JITContext::current()->remove(heapptr);
        
        return 0;
    }
    
    void SetObject(duk_context * ctx, duk_idx_t idx, Object * object) {
        
        if(duk_is_object(ctx, idx) || duk_is_function(ctx, idx)) {
            
            duk_get_finalizer(ctx, idx);
            
            if(duk_is_function(ctx, -1)) {
                duk_pop(ctx);
                return;
            } else {
                duk_pop(ctx);
            }
            
            duk_push_c_function(ctx, Object_dealloc, 1);
            duk_set_finalizer(ctx, idx - 1);
            
            JITContext::current()->strong(ctx, duk_get_heapptr(ctx, idx), object);
            
        }
        
    }
    
    void SetWeakObject(duk_context * ctx, duk_idx_t idx, Object * object) {
        
        if(duk_is_object(ctx, idx) || duk_is_function(ctx, idx)) {
            
            duk_get_finalizer(ctx, idx);
            
            if(duk_is_function(ctx, -1)) {
                duk_pop(ctx);
                return;
            } else {
                duk_pop(ctx);
            }
            
            duk_push_c_function(ctx, Object_dealloc, 1);
            duk_set_finalizer(ctx, idx - 1);
            
            JITContext::current()->weak(ctx, duk_get_heapptr(ctx, idx), object);
            
        }
        
    }
    
    void SetPrototype(duk_context * ctx, duk_idx_t idx, const Class * isa) {
        SetPrototype(ctx, idx, isa->name);
    }
    
    void SetPrototype(duk_context * ctx, duk_idx_t idx, kk::CString name) {
        duk_get_global_string(ctx, name);
        
        if(duk_is_function(ctx, -1)) {
            duk_get_prototype(ctx, -1);
            duk_set_prototype(ctx, idx - 2);
        }
        
        duk_pop(ctx);
    }
    
    kk::Boolean HasPrototype(duk_context * ctx, kk::CString name) {
        
        duk_get_global_string(ctx, name);
        
        if(duk_is_function(ctx, -1)) {
            duk_get_prototype(ctx, -1);
            if(duk_is_object(ctx, -1)) {
                duk_pop_2(ctx);
                return true;
            }
            duk_pop(ctx);
        }
        
        duk_pop(ctx);
        
        return false;
    }
    
    void PushObject(duk_context * ctx, Object * object) {
        
        {
            _Array * v = dynamic_cast<_Array *>(object);
            if(v != nullptr) {
                duk_push_array(ctx);
                duk_uarridx_t i = 0;
                v->forEach([&i,ctx](Any & item) -> void {
                    PushAny(ctx, item);
                    duk_put_prop_index(ctx, -2, i);
                    i ++;
                });
                return;
            }
        }
        
        {
            _TObject * v = dynamic_cast<_TObject *>(object);
            if(v != nullptr) {
                duk_push_object(ctx);
                v->forEach([ctx](Any & value,Any & key) -> void {
                    CString sKey = key;
                    if(sKey) {
                        PushAny(ctx, value);
                        duk_put_prop_string(ctx, -2, sKey);
                    }
                });
                return;
            }
        }
        
        {
            ArrayBuffer * v = dynamic_cast<ArrayBuffer *>(object);
            if(v != nullptr) {
                void * data = duk_push_fixed_buffer(ctx, v->byteLength());
                memcpy(data, v->data(), v->byteLength());
                duk_push_buffer_object(ctx, -1, 0, v->byteLength(), DUK_BUFOBJ_ARRAYBUFFER);
                duk_remove(ctx, -2);
                return;
            }
        }
        
        {
            NativeValue * v = dynamic_cast<NativeValue *>(object);
            if(v != nullptr) {
                PushNative(ctx, v->native());
                return;
            }
        }
        
        void * heapptr = JITContext::current()->get(object, ctx);
        
        if(heapptr != nullptr) {
            duk_push_heapptr(ctx, heapptr);
            return;
        }
        
        duk_push_object(ctx);
        SetObject(ctx, -1, object);
        
        {
            NativeObject * v = dynamic_cast<NativeObject *>(object);
            if(v != nullptr) {
                kk::String s = NativeObject::getPrototype(v->native());
                if(s != "") {
                    SetPrototype(ctx, -1, s.c_str());
                    return;
                }
            }
        }
        
        {
            const Class * isa = object->isa();
            if(isa != nullptr) {
                SetPrototype(ctx, -1, isa);
            }
        }
        
    }
    
    void PushWeakObject(duk_context * ctx, Object * object) {
        
        {
            _Array * v = dynamic_cast<_Array *>(object);
            if(v != nullptr) {
                duk_push_array(ctx);
                duk_uarridx_t i = 0;
                v->forEach([&i,ctx](Any & item) -> void {
                    PushAny(ctx, item);
                    duk_put_prop_index(ctx, -2, i);
                    i ++;
                });
                return;
            }
        }
        
        {
            _TObject * v = dynamic_cast<_TObject *>(object);
            if(v != nullptr) {
                duk_push_object(ctx);
                v->forEach([ctx](Any & value,Any & key) -> void {
                    CString sKey = key;
                    if(sKey) {
                        PushAny(ctx, value);
                        duk_put_prop_string(ctx, -2, sKey);
                    }
                });
                return;
            }
        }
        
        {
            NativeValue * v = dynamic_cast<NativeValue *>(object);
            if(v != nullptr) {
                PushNative(ctx, v->native());
                return;
            }
        }
        
        void * heapptr = JITContext::current()->get(object, ctx);
        
        if(heapptr != nullptr) {
            duk_push_heapptr(ctx, heapptr);
            return;
        }
        
        duk_push_object(ctx);
        SetWeakObject(ctx, -1, object);
        
        {
            const Class * isa = object->isa();
            if(isa != nullptr) {
                SetPrototype(ctx, -1, isa);
            }
        }
        
    }
    
    
    Object * GetObject(duk_context * ctx, duk_idx_t idx) {
        
        if(duk_is_object(ctx, idx) || duk_is_function(ctx, idx)) {
            void * heapptr = duk_get_heapptr(ctx, idx);
            return JITContext::current()->get(heapptr);
        }
        
        return nullptr;
    }
    
    static duk_ret_t JSObject_dealloc(duk_context * ctx) {
        
        duk_push_current_function(ctx);
        
        duk_get_prop_string(ctx, -1, "__object");
        
        if(duk_is_pointer(ctx, -1)) {
            JSObject * v = (JSObject *) duk_to_pointer(ctx, -1);
            if(v && JITContext::current()->get(v, ctx) != nullptr) {
                v->recycle();
            }
        }
        
        duk_pop_2(ctx);
        
        return 0;
    }
    
    JSObject::JSObject(duk_context * ctx, void * heapptr):_ctx(ctx) {
        
        duk_push_heap_stash(ctx);
        
        duk_push_sprintf(ctx, "0x%x",(long) heapptr);
        duk_push_heapptr(ctx, heapptr);
        {
            duk_push_c_function(ctx, JSObject_dealloc, 1);
            {
                duk_push_pointer(ctx, this);
                duk_put_prop_string(ctx, -2, "__object");
            }
            duk_set_finalizer(ctx, -2);
        }
        
        duk_put_prop(ctx, -3);
        
        duk_pop(ctx);
        
        JITContext::current()->weak(ctx, heapptr, this);
        
        _queue = getCurrentDispatchQueue();
        
        if(_queue.get() != mainDispatchQueue()) {
            kk::Log("");
        }
    }
    
    void Error(duk_context * ctx, duk_idx_t idx, kk::CString prefix) {
        if(duk_is_error(ctx, idx)) {
            duk_get_prop_string(ctx, idx, "lineNumber");
            int lineNumber = duk_to_int(ctx, -1);
            duk_pop(ctx);
            duk_get_prop_string(ctx, idx, "stack");
            const char * error = duk_to_string(ctx, -1);
            duk_pop(ctx);
            duk_get_prop_string(ctx, idx, "fileName");
            const char * fileName = duk_to_string(ctx, -1);
            duk_pop(ctx);
            kk::Log("%s %s(%d): %s",prefix,fileName,lineNumber,error);
        } else {
            kk::Log("%s %s",prefix,duk_to_string(ctx, idx));
        }
    }
    
    DispatchQueue * JSObject::queue() {
        return _queue;
    }
    
    JSObject::~JSObject() {
        
        duk_context * ctx = _ctx;
        Object * object = this;
        
        if(ctx) {
            
            _queue->sync([ctx,object]()->void{
                
                void * heapptr = JITContext::current()->get(object, ctx);
                
                if(heapptr) {
                    
                    duk_push_heapptr(ctx, heapptr);
                    duk_push_undefined(ctx);
                    duk_set_finalizer(ctx, -2);
                    duk_pop(ctx);
                    
                    duk_push_heap_stash(ctx);
                    duk_push_sprintf(ctx, "0x%x",(long) heapptr);
                    duk_del_prop(ctx, -2);
                    duk_pop(ctx);
                    
                }
                
                JITContext::current()->remove(object);
                
            });
            
        }

    }
    
    duk_context * JSObject::jsContext() {
        return _ctx;
    }
    
    void * JSObject::heapptr() {
        return JITContext::current()->get(this, _ctx);
    }
    
    void JSObject::recycle() {
        
        JITContext::current()->remove(this);
        _ctx = nullptr;
    }
    
    JSObject::operator kk::Strong<TObject<kk::String,kk::Any>>() {
        
        kk::Strong<TObject<kk::String,kk::Any>> v = new TObject<kk::String,kk::Any>();
        
        if(_ctx) {
            
            void * heapptr = JITContext::current()->get(this, _ctx);
            
            if(heapptr) {
                
                duk_push_heapptr(_ctx, heapptr);
                
                if(duk_is_object(_ctx, -1)) {
                    
                    duk_enum(_ctx, -1, DUK_ENUM_INCLUDE_SYMBOLS);
                    
                    kk::Any key;
                    kk::Any value;
                    
                    while(duk_next(_ctx, -1, 1)) {
                        
                        GetAny(_ctx, -2, key);
                        GetAny(_ctx, -1, value);
                        
                        (*v)[key] = value;
                        
                    }
                    
                    duk_pop(_ctx);
                    
                }
                
                duk_pop(_ctx);
            }
        }
        
        return v;
    }
    
    JSObject::operator kk::Strong<Array<kk::Any>>() {
        kk::Strong<Array<kk::Any>> v = new Array<kk::Any>();
        
        if(_ctx) {
            
            void * heapptr = JITContext::current()->get(this, _ctx);
            
            if(heapptr) {
            
                duk_push_heapptr(_ctx, heapptr);
                
                if(duk_is_array(_ctx, -1)) {
                    
                    duk_enum(_ctx, -1, DUK_ENUM_ARRAY_INDICES_ONLY);
                    
                    kk::Any value;
                    
                    while(duk_next(_ctx, -1, 1)) {
                        
                        GetAny(_ctx, -1, value);
                        
                        v->push(value);
                        
                    }
                    
                    duk_pop(_ctx);
                    
                }
                
                duk_pop(_ctx);
            }
        }
        
        return v;
    }
    
    void PushAny(duk_context * ctx, Any & any) {
        switch (any.type) {
            case TypeInt8:
                duk_push_int(ctx, any.int8Value);
                break;
            case TypeUint8:
                duk_push_uint(ctx, any.uint8Value);
                break;
            case TypeInt16:
                duk_push_int(ctx, any.int16Value);
                break;
            case TypeUint16:
                duk_push_uint(ctx, any.uint16Value);
                break;
            case TypeInt32:
                duk_push_int(ctx, any.int32Value);
                break;
            case TypeUint32:
                duk_push_uint(ctx, any.uint32Value);
                break;
            case TypeInt64:
                duk_push_sprintf(ctx, "%lld",any.int64Value);
                break;
            case TypeUint64:
                duk_push_sprintf(ctx, "%llu",any.uint64Value);
                break;
            case TypeBoolean:
                duk_push_boolean(ctx, any.booleanValue);
                break;
            case TypeFloat32:
                duk_push_number(ctx, any.float32Value);
                break;
            case TypeFloat64:
                duk_push_number(ctx, any.float64Value);
                break;
            case TypeObject:
            case TypeFunction:
                if(any.objectValue.get() == nullptr) {
                    duk_push_undefined(ctx);
                } else {
                    PushObject(ctx, any.objectValue.get());
                }
                break;
            case TypeString:
                duk_push_lstring(ctx, any.stringValue,any.length);
                break;
            default:
                duk_push_undefined(ctx);
                break;
        }
    }
    
    std::shared_ptr<Any> GetAnyPtr(duk_context * ctx, duk_idx_t idx) {
        Any * vv = new Any();
        GetAny(ctx,idx,* vv);
        return std::shared_ptr<Any>(vv);
    }
    
    void GetAny(duk_context * ctx, duk_idx_t idx,Any & v) {
    
        switch (duk_get_type(ctx, idx)) {
            case DUK_TYPE_NUMBER:
            {
                Double dv = duk_to_number(ctx, idx);
                Int64 iv = (Int64) dv;
                Int32 iiv = (Int32) dv;
                if((Double) iv == dv) {
                    if(iv == (Int64) iiv) {
                        v = iiv;
                    } else {
                        v = iv;
                    }
                } else {
                    v = dv;
                }
            }
                break;
            case DUK_TYPE_BOOLEAN:
                v = (Boolean) duk_to_boolean(ctx, idx);
                break;
            case DUK_TYPE_STRING:
                v = (CString) duk_to_string(ctx, idx);
                break;
            case DUK_TYPE_OBJECT:
            case DUK_TYPE_LIGHTFUNC:
            {
                Object * a =  GetObject(ctx, idx);
                if(a == nullptr) {
                    a = new JSObject(ctx,duk_get_heapptr(ctx, idx));
                }
                v = a;
            }
                break;
            case DUK_TYPE_BUFFER:
            {
                if(duk_is_buffer_data(ctx, idx)) {
                    size_t n;
                    void * data = duk_get_buffer_data(ctx, idx, &n);
                    v = new ArrayBuffer(data,(kk::Uint) n);
                } else {
                    size_t n;
                    void * data = duk_get_buffer(ctx, idx, &n);
                    v = new ArrayBuffer(data,(kk::Uint) n);
                }
            }
                break;
            default:
                break;
        }
        
    }
    
    void addOpenlib(std::function<void(duk_context *)> && func) {
        kk::Openlib<>::add(std::move(func));
    }
    
    void OpenBaselib(){
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            duk_push_c_function(ctx, [](duk_context * ctx)->duk_ret_t{
                
                duk_idx_t top = duk_get_top(ctx);
                
                Any v;
                
                for(duk_idx_t i = - top ;i < 0; i++) {
                    switch (duk_get_type(ctx, i)) {
                        case DUK_TYPE_NULL:
                        kk::Log("null");
                        break;
                        case DUK_TYPE_UNDEFINED:
                        kk::Log("undefined");
                        break;
                        case DUK_TYPE_BOOLEAN:
                        kk::Log("%s",duk_to_boolean(ctx, i) ? "true":"false");
                        break;
                        case DUK_TYPE_NUMBER:
                        kk::Log("%g",duk_to_number(ctx, i));
                        break;
                        case DUK_TYPE_STRING:
                        kk::Log("%s",duk_to_string(ctx, i));
                        break;
                        case DUK_TYPE_BUFFER:
                        {
                            size_t n;
                            void * data = duk_is_buffer_data(ctx, i) ?  duk_get_buffer_data(ctx, i, &n) : duk_get_buffer(ctx, i, &n);
                            kk::Log("<0x%x:%d>",data,n);
                        }
                        break;
                        case DUK_TYPE_LIGHTFUNC:
                        kk::Log("[function]");
                        break;
                        default:
                        kk::Log("[object]");
                        break;
                    }
                }
                
                return 0;
                
            }, DUK_VARARGS);
            
            duk_put_global_string(ctx, "print");
            
            duk_push_c_function(ctx, [](duk_context * ctx)->duk_ret_t{
                
                if(duk_is_string(ctx, -1) && duk_is_string(ctx, -2)) {
                    
                    kk::CString code = duk_to_string(ctx, -2);
                    kk::CString path = duk_to_string(ctx, -1);
                    
                    duk_push_string(ctx, path);
                    duk_compile_string_filename(ctx, 0, code);
                    return 1;
                    
                }
                
                return 0;
                
            }, 2);
            
            duk_put_global_string(ctx, "compile");
            
        });
    }
    
    
    void duk_push_Signature(duk_context * ctx,Signature & v) {
        
        if(v.to != nullptr) {
            (*v.to)(&v,ctx);
            return;
        }
        
        switch (v.type) {
            case SignatureTypeInt8:
                duk_push_int(ctx, v.int8Value);
                break;
            case SignatureTypeUint8:
                duk_push_uint(ctx, v.uint8Value);
                break;
            case SignatureTypeInt16:
                duk_push_int(ctx, v.int16Value);
                break;
            case SignatureTypeUint16:
                duk_push_uint(ctx, v.uint16Value);
                break;
            case SignatureTypeInt32:
                duk_push_int(ctx, v.int32Value);
                break;
            case SignatureTypeUint32:
                duk_push_uint(ctx, v.uint32Value);
                break;
            case SignatureTypeInt64:
                duk_push_sprintf(ctx, "%lld",v.int64Value);
                break;
            case SignatureTypeUint64:
                duk_push_sprintf(ctx, "%llu",v.uint64Value);
                break;
            case SignatureTypeFloat32:
                duk_push_number(ctx, v.float32Value);
                break;
            case SignatureTypeFloat64:
                duk_push_number(ctx, v.float64Value);
                break;
            case SignatureTypeBoolean:
                duk_push_boolean(ctx, v.booleanValue);
                break;
            case SignatureTypeCString:
                if(v.cstringValue != nullptr) {
                    duk_push_string(ctx, v.cstringValue);
                } else {
                    duk_push_undefined(ctx);
                }
                break;
            case SignatureTypeObject:
                if(v.objectValue != nullptr) {
                    PushObject(ctx, v.objectValue);
                } else {
                    duk_push_undefined(ctx);
                }
                break;
            case SignatureTypeNative:
                if(v.nativeValue != nullptr) {
                    PushObject(ctx, new NativeObject(v.nativeValue));
                } else {
                    duk_push_undefined(ctx);
                }
                break;
            default:
                duk_push_undefined(ctx);
                break;
        }
    }
    
    void duk_get_Signature(duk_context * ctx,duk_idx_t idx,Signature & v) {
        
        if(v.from != nullptr) {
            (*v.from)(&v,ctx,idx);
            return;
        }
        
        switch (duk_get_type(ctx, idx)) {
            case DUK_TYPE_NUMBER:
            {
                switch (v.type) {
                    case SignatureTypeInt8:
                        v.int8Value = duk_to_int(ctx, idx);
                        break;
                    case SignatureTypeUint8:
                        v.uint8Value = duk_to_uint(ctx, idx);
                        break;
                    case SignatureTypeInt16:
                        v.int16Value = duk_to_int(ctx, idx);
                        break;
                    case SignatureTypeUint16:
                        v.uint16Value = duk_to_uint(ctx, idx);
                        break;
                    case SignatureTypeInt32:
                        v.int32Value = duk_to_int(ctx, idx);
                        break;
                    case SignatureTypeUint32:
                        v.uint32Value = duk_to_uint(ctx, idx);
                        break;
                    case SignatureTypeInt64:
                        v.int64Value = duk_to_int(ctx, idx);
                        break;
                    case SignatureTypeUint64:
                        v.uint64Value = duk_to_uint(ctx, idx);
                        break;
                    case SignatureTypeFloat32:
                        v.float32Value = duk_to_number(ctx, idx);
                        break;
                    case SignatureTypeFloat64:
                        v.float64Value = duk_to_number(ctx, idx);
                        break;
                    case SignatureTypeBoolean:
                        v.booleanValue = duk_to_number(ctx, idx) != 0;
                        break;
                    default:
                        break;
                }
            }
                break;
            case DUK_TYPE_BOOLEAN:
            {
                switch (v.type) {
                    case SignatureTypeInt8:
                        v.int8Value = duk_to_boolean(ctx, idx);
                        break;
                    case SignatureTypeUint8:
                        v.uint8Value = duk_to_boolean(ctx, idx);
                        break;
                    case SignatureTypeInt16:
                        v.int16Value = duk_to_boolean(ctx, idx);
                        break;
                    case SignatureTypeUint16:
                        v.uint16Value = duk_to_boolean(ctx, idx);
                        break;
                    case SignatureTypeInt32:
                        v.int32Value = duk_to_boolean(ctx, idx);
                        break;
                    case SignatureTypeUint32:
                        v.uint32Value = duk_to_boolean(ctx, idx);
                        break;
                    case SignatureTypeInt64:
                        v.int64Value = duk_to_boolean(ctx, idx);
                        break;
                    case SignatureTypeUint64:
                        v.uint64Value = duk_to_boolean(ctx, idx);
                        break;
                    case SignatureTypeFloat32:
                        v.float32Value = duk_to_boolean(ctx, idx);
                        break;
                    case SignatureTypeFloat64:
                        v.float64Value = duk_to_boolean(ctx, idx);
                        break;
                    case SignatureTypeBoolean:
                        v.booleanValue = duk_to_boolean(ctx, idx);
                        break;
                    case SignatureTypeCString:
                        v.cstringValue = duk_to_boolean(ctx, idx) ? "true" : "false";
                        break;
                    default:
                        break;
                }
            }
                break;
            case DUK_TYPE_STRING:
            {
                switch (v.type) {
                    case SignatureTypeInt8:
                        v.int8Value = atoi(duk_to_string(ctx, idx));
                        break;
                    case SignatureTypeUint8:
                        v.uint8Value = atoi(duk_to_string(ctx, idx));
                        break;
                    case SignatureTypeInt16:
                        v.int16Value = atoi(duk_to_string(ctx, idx));
                        break;
                    case SignatureTypeUint16:
                        v.uint16Value = atoi(duk_to_string(ctx, idx));
                        break;
                    case SignatureTypeInt32:
                        v.int32Value = atoi(duk_to_string(ctx, idx));
                        break;
                    case SignatureTypeUint32:
                        v.uint32Value = atoi(duk_to_string(ctx, idx));
                        break;
                    case SignatureTypeInt64:
                        v.int64Value = atoll(duk_to_string(ctx, idx));
                        break;
                    case SignatureTypeUint64:
                        v.uint64Value = atoll(duk_to_string(ctx, idx));
                        break;
                    case SignatureTypeFloat32:
                        v.float32Value = atof(duk_to_string(ctx, idx));
                        break;
                    case SignatureTypeFloat64:
                        v.float64Value = atof(duk_to_string(ctx, idx));
                        break;
                    case SignatureTypeBoolean:
                        v.booleanValue = kk::CStringEqual(duk_to_string(ctx, idx), "true");
                        break;
                    case SignatureTypeCString:
                        v.cstringValue = duk_to_string(ctx, idx);
                        break;
                    default:
                        break;
                }
            }
                break;
            case DUK_TYPE_LIGHTFUNC:
            case DUK_TYPE_OBJECT:
            {
                switch (v.type) {
                    case SignatureTypeObject:
                        v.objectValue = new JSObject(ctx,duk_get_heapptr(ctx, idx));
                        break;
                    default:
                        break;
                }
            }
                break;
            default:
                break;
        }
    }
    
    void duk_get_Arguments(duk_context * ctx,std::vector<Signature> & arguments) {
        duk_idx_t n = duk_get_top(ctx);
        auto i = arguments.begin();
        while(i != arguments.end() && n > 0) {
            duk_get_Signature(ctx, - n, * i);
            i ++;
            n --;
        }
    }
    
    void duk_pcall(duk_context * ctx,Signature returnSignature,std::vector<Signature> & arguments) {
        
        auto i = arguments.begin();
        while(i != arguments.end()) {
            duk_push_Signature(ctx, * i);
            i ++;
        }
        
        if(::duk_pcall(ctx, (duk_idx_t)arguments.size()) == DUK_EXEC_SUCCESS) {
            duk_get_Signature(ctx, -1, returnSignature);
        } else {
            kk::Error(ctx, -1, "[kk::duk_pcall]");
        }
        
        duk_pop(ctx);
        
    }
    
    void duk_pcall_method(duk_context * ctx,Signature returnSignature,std::vector<Signature> & arguments) {
        
        auto i = arguments.begin();
        while(i != arguments.end()) {
            duk_push_Signature(ctx, * i);
            i ++;
        }
        
        if(::duk_pcall_method(ctx, (duk_idx_t)arguments.size()) == DUK_EXEC_SUCCESS) {
            duk_get_Signature(ctx, -1, returnSignature);
        } else {
            kk::Error(ctx, -1, "[kk::duk_pcall]");
        }
        
        duk_pop(ctx);
        
    }
    
    static duk_ret_t duk_json_decode_func(duk_context *ctx, void *udata) {
        duk_json_decode(ctx, -1);
        return 1;
    }
    
    duk_ret_t duk_json_decode(duk_context * ctx,void * data,size_t size) {
        duk_push_lstring(ctx, (const char *) data, size);
        return duk_safe_call(ctx, duk_json_decode_func, nullptr, 1, 1);
    }
}
