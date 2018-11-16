//
//  KerObject.m
//  KK
//
//  Created by zhanghailong on 2018/11/2.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerObject.h"
#include <objc/runtime.h>

@interface KerJSObject()

-(instancetype) initWithJSObject:(kk::JSObject *) object;

@end

namespace kk {

    
    NativeObject::NativeObject(Native * native):_native(native) {
        if(_native){
            CFRetain((CFTypeRef) _native);
        }
    }
    
    kk::CString NativeObject::getPrototype(Native * native) {
        
        ::Class isa = object_getClass((__bridge id) native);
        
        unsigned int n = 0, i= 0;
        
        Protocol * __unsafe_unretained  *  p = class_copyProtocolList(isa, &n);
        
        for(;i<n;i++) {
            
            if(p[i] == @protocol(KerJSExport)) {
                free(p);
                return class_getName(isa);
            }
            
        }
    
        for(i = 0;i<n;i++) {
            
            if(protocol_conformsToProtocol(p[i], @protocol(KerJSExport))) {
                free(p);
                return protocol_getName(p[i]);
            }
            
        }
        
        free(p);
        
        return nullptr;
    }
    
    NativeObject::~NativeObject() {
        if(_native != nullptr) {
            CFRelease((CFTypeRef) _native);
        }
    }
    
    Native * NativeObject::native() {
        return _native;
    }
    
    Any::operator Native*() {
        
        if(type == TypeObject) {
            {
                NativeObject * v = dynamic_cast<NativeObject *>(objectValue.get());
                if(v != nullptr) {
                    return v->native();
                }
            }
            {
                JSObject * v = dynamic_cast<JSObject *>(objectValue.get());
                
                if(v != nullptr) {
                    KerJSObject * object = [[KerJSObject alloc] initWithJSObject:v];
                    NativeObject * native = new kk::NativeObject((__bridge Native *) object);
                    objectValue = native;
                    return native->native();
                }
                
            }
        }
        
        return nullptr;
    }
    
    namespace objc {
        
        void PushInterface(duk_context * ctx,kk::CString name,kk::CString base, std::function<void(duk_context *)> && func) {
            
            duk_push_c_function(ctx, [](duk_context * ctx)->duk_ret_t{
                return 0;
            }, 0);
            
            duk_push_object(ctx);
            
            if(base) {
                SetPrototype(ctx, -1, base);
            }
            
            if(func != nullptr) {
                func(ctx);
            }
            
            duk_set_prototype(ctx, -2);
            
            duk_put_global_string(ctx, name);
            
        }
        
    }
    
}


void duk_push_NSObject(duk_context * ctx, id object) {
    
    if(object == nil) {
        duk_push_undefined(ctx);
        return;
    }
    
    if([object isKindOfClass:[NSString class]]) {
        duk_push_string(ctx, [object UTF8String]);
        return;
    }
    
    if([object isKindOfClass:[NSNumber class]]) {
        if(strcmp( [object objCType],@encode(BOOL)) == 0) {
            duk_push_boolean(ctx, [object boolValue]);
        } else {
            duk_push_number(ctx, [object doubleValue]);
        }
        return ;
    }
    
    if([object isKindOfClass:[NSValue class]]) {
        duk_push_string(ctx, [[object description] UTF8String]);
        return;
    }
    
    if([object isKindOfClass:[NSArray class]]) {
        duk_push_array(ctx);
        duk_uarridx_t i = 0;
        for(id v in object) {
            duk_push_NSObject(ctx, v);
            duk_put_prop_index(ctx, -2, i);
            i++;
        }
        return;
    }
    
    if([object isKindOfClass:[NSDictionary class]]) {
        duk_push_object(ctx);
        NSEnumerator * keyEnum = [object keyEnumerator];
        id key;
        while((key = [keyEnum nextObject])) {
            id v = [object objectForKey:key];
            duk_push_string(ctx, [key UTF8String]);
            duk_push_NSObject(ctx, v);
            duk_put_prop(ctx, -3);
        }
        return;
    }
    
    if([object isKindOfClass:[KerJSObject class]]) {
        kk::PushObject(ctx, (kk::Object *) [(KerJSObject *) object JSObject]);
        return;
    }
    
    {

        kk::CString name = kk::NativeObject::getPrototype((__bridge kk::Native *) object);
        
        if(name != nullptr) {
            kk::Strong<kk::NativeObject> v = new kk::NativeObject((__bridge kk::Native *) object);
            kk::PushObject(ctx, v.get());
        }
        
    }
    
    duk_push_object(ctx);
    
    Class isa = [object class];
    
    while(isa && isa != [NSObject class]) {
        
        unsigned int n;
        objc_property_t * p = class_copyPropertyList(isa, &n);
        
        for(int i=0;i<n;i++) {
            
            NSString * name = [NSString stringWithCString:property_getName(p[i]) encoding:NSUTF8StringEncoding];
            id v = [object valueForKey:name];
            
            if(v != nil) {
                duk_push_string(ctx,[name UTF8String]);
                duk_push_NSObject(ctx, v);
                duk_put_prop(ctx, -3);
            }
            
        }
        
        free(p);
        
        isa = class_getSuperclass(isa);
    }
    
    return;
    
}

id duk_to_NSObject(duk_context * ctx,duk_idx_t idx) {
    
    duk_int_t type = duk_get_type(ctx, idx);
    
    switch (type) {
        case DUK_TYPE_STRING:
        return [NSString stringWithCString:duk_to_string(ctx, idx) encoding:NSUTF8StringEncoding];
        case DUK_TYPE_NUMBER:
        return [NSNumber numberWithDouble:duk_to_number(ctx, idx)];
        case DUK_TYPE_BOOLEAN:
        return [NSNumber numberWithBool:duk_to_boolean(ctx, idx)];
        case DUK_TYPE_OBJECT:
        if(duk_is_array(ctx, idx)) {
            NSMutableArray * vs = [NSMutableArray arrayWithCapacity:4];
            duk_enum(ctx, idx, DUK_ENUM_ARRAY_INDICES_ONLY);
            while(duk_next(ctx, -1, 1)) {
                id v = duk_to_NSObject(ctx, -1);
                if(v) {
                    [vs addObject:v];
                }
                duk_pop_2(ctx);
            }
            duk_pop(ctx);
            return vs;
        } else {
            {
                kk::Object * v = kk::GetObject(ctx, idx);
                if(v != nullptr) {
                    {
                        kk::_TObject * o = dynamic_cast<kk::_TObject *>(v);
                        if(o) {
                            NSMutableDictionary * data = [NSMutableDictionary dictionaryWithCapacity:4];
                            o->forEach([data](kk::Any & key,kk::Any & value)->void{
                                id k = KerObjectFromAny(key);
                                id v = KerObjectFromAny(value);
                                [data setObject:v forKey:k];
                            });
                            return data;
                        }
                    }
                    {
                        kk::_Array * o = dynamic_cast<kk::_Array *>(v);
                        if(o) {
                            NSMutableArray * data = [NSMutableArray arrayWithCapacity:4];
                            o->forEach([data](kk::Any & item)->void{
                                id v = KerObjectFromAny(item);
                                if(v) {
                                    [data addObject:v];
                                }
                            });
                            return data;
                        }
                    }
                    {
                        kk::JSObject * o = dynamic_cast<kk::JSObject *>(v);
                        if(o) {
                            return [[KerJSObject alloc] initWithJSObject:o];
                        }
                    }
                }
            }
            NSMutableDictionary * data = [NSMutableDictionary dictionaryWithCapacity:4];
            duk_enum(ctx, idx, DUK_ENUM_INCLUDE_SYMBOLS);
            while(duk_next(ctx, -1, 1)) {
                id key = duk_to_NSObject(ctx, -2);
                id v = duk_to_NSObject(ctx, -1);
                if(key && v) {
                    [data setObject:v forKey:key];
                }
                duk_pop_2(ctx);
            }
            duk_pop(ctx);
            return data;
        }
        case DUK_TYPE_BUFFER:
        if(duk_is_buffer_data(ctx, idx)) {
            duk_size_t size = 0;
            void * bytes = duk_get_buffer_data(ctx, idx, &size);
            return [NSData dataWithBytes:bytes length:size];
        } else {
            duk_size_t size = 0;
            void * bytes = duk_get_buffer(ctx, idx, &size);
            return [NSData dataWithBytes:bytes length:size];
        }
        case DUK_TYPE_LIGHTFUNC:
        {
            kk::JSObject * v = dynamic_cast<kk::JSObject *>(kk::GetObject(ctx, idx));
            if(v) {
                return [[KerJSObject alloc] initWithJSObject:v];
            }
        }
            return nil;
        default:
        return nil;
    }
    
}

kk::Any KerObjectToAny(id object) {
    kk::Any v;
    
    if([object isKindOfClass:[NSNumber class]]) {
        if(strcmp([object objCType], @encode(BOOL)) == 0) {
            v = (kk::Boolean) [object boolValue];
        } else if(strcmp([object objCType], @encode(double)) == 0) {
            v = (kk::Double) [object doubleValue];
        } else if(strcmp([object objCType], @encode(float)) == 0) {
            v = (kk::Float) [object floatValue];
        } else if(strcmp([object objCType], @encode(int)) == 0) {
            v = (kk::Int) [object intValue];
        } else if(strcmp([object objCType], @encode(unsigned int)) == 0) {
            v = (kk::Uint) [object unsignedIntValue];
        } else if(strcmp([object objCType], @encode(long long)) == 0) {
            v = (kk::Int64) [object longLongValue];
        } else if(strcmp([object objCType], @encode(unsigned long long)) == 0) {
            v = (kk::Uint64) [object unsignedLongLongValue];
        } else {
            v = (kk::Double) [object doubleValue];
        }
    } else if([object isKindOfClass:[NSString class]]) {
        v = (kk::CString) [object UTF8String];
    } else if([object isKindOfClass:[NSArray class]]) {
        
        kk::Array<kk::Any> * items = new kk::Array<kk::Any>();
        
        for(id item in object) {
            kk::Any i = KerObjectToAny(item);
            items->push(i);
        }
        
        v = (kk::Object *) items;
        
    } else if([object isKindOfClass:[NSDictionary class]]) {
        kk::TObject<kk::String, kk::Any> * m = new kk::TObject<kk::String, kk::Any>();
        
        NSEnumerator * keyEnum = [object keyEnumerator];
        NSString * key ;
        while((key = [keyEnum nextObject])) {
            id v = [object objectForKey:key];
            kk::CString skey = [key UTF8String];
            (*m)[skey] = KerObjectToAny(v);
        }
        
        v = (kk::Object *) m;
    } else if([object isKindOfClass:[KerJSObject class]]) {
        v = (kk::Object *) [(KerJSObject *) object JSObject];
    }
    
    return v;
}

id KerObjectFromAny(kk::Any & v) {
    switch (v.type) {
        case kk::TypeInt8:
        return [NSNumber numberWithInt:v.int8Value];
        case kk::TypeInt16:
        return [NSNumber numberWithInt:v.int16Value];
        case kk::TypeInt32:
        return [NSNumber numberWithInt:v.int32Value];
        case kk::TypeInt64:
        return [NSNumber numberWithLongLong:v.int64Value];
        case kk::TypeUint8:
        return [NSNumber numberWithUnsignedInt:v.uint8Value];
        case kk::TypeUint16:
        return [NSNumber numberWithUnsignedInt:v.uint16Value];
        case kk::TypeUint32:
        return [NSNumber numberWithUnsignedInt:v.uint32Value];
        case kk::TypeUint64:
        return [NSNumber numberWithUnsignedLongLong:v.uint64Value];
        case kk::TypeBoolean:
        return [NSNumber numberWithBool:v.booleanValue];
        case kk::TypeString:
        return [NSString stringWithCString:v.stringValue encoding:NSUTF8StringEncoding];
        case kk::TypeObject:
        {
            kk::_TObject * object = dynamic_cast<kk::_TObject *>(v.objectValue.get());
            if(object) {
                NSMutableDictionary * m = [NSMutableDictionary dictionaryWithCapacity:4];
                object->forEach([m](kk::Any & key,kk::Any & value)->void{
                    kk::CString skey = key;
                    id vv = KerObjectFromAny(value);
                    if(skey) {
                        [m setObject:vv forKey:[NSString stringWithCString:skey encoding:NSUTF8StringEncoding]];
                    }
                });
                return m;
            }
        }
        {
            kk::_Array * object = dynamic_cast<kk::_Array *>(v.objectValue.get());
            if(object) {
                NSMutableArray * m = [NSMutableArray arrayWithCapacity:4];
                object->forEach([m](kk::Any & value)->void{
                    id vv = KerObjectFromAny(value);
                    if(vv) {
                        [m addObject:vv];
                    }
                });
                return m;
            }
        }
        {
            kk::JSObject * object = dynamic_cast<kk::JSObject *>(v.objectValue.get());
            if(object) {
                return [[KerJSObject alloc] initWithJSObject:object];
            }
        }

        default:
        break;
    }
    return nil;
}


@implementation NSObject(KerObject)

-(id) ker_getValue:(NSString *) key {
    @try {
        return [self valueForKey:key];
    }
    @catch(NSException *ex) {
        
    }
    return nil;
}

-(NSString *) ker_getString:(NSString *) key {
    return [[self ker_getValue:key] ker_stringValue];
}

-(NSString *) ker_stringValue {
    if([self isKindOfClass:[NSString class]]) {
        return (NSString *) self;
    }
    return [self description];
}

@end


@interface KerJSObjectDynamicObject : NSProxy {
    KerJSObject * _object;
    Protocol * _protocol;
}

-(instancetype) initWithProtocol:(Protocol *) protocol object:(KerJSObject *) object;

@end

@implementation KerJSObjectDynamicObject

-(instancetype) initWithProtocol:(Protocol *) protocol object:(KerJSObject *) object {
    _protocol = protocol;
    _object = object;
    return self;
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector {
    objc_method_description m = protocol_getMethodDescription(_protocol, aSelector, YES, YES);
    return [NSMethodSignature signatureWithObjCTypes: m.types];
}

- (BOOL)respondsToSelector:(SEL)aSelector {
    return YES;
}


/*
 enum _NSObjCValueType {
 NSObjCNoType = 0,
 NSObjCVoidType = 'v',
 NSObjCCharType = 'c',
 NSObjCShortType = 's',
 NSObjCLongType = 'l',
 NSObjCLonglongType = 'q',
 NSObjCFloatType = 'f',
 NSObjCDoubleType = 'd',
 NSObjCBoolType = 'B',
 NSObjCSelectorType = ':',
 NSObjCObjectType = '@',
 NSObjCStructType = '{',
 NSObjCPointerType = '^',
 NSObjCStringType = '*',
 NSObjCArrayType = '[',
 NSObjCUnionType = '(',
 NSObjCBitfield = 'b'
 } API_DEPRECATED("Not supported", macos(10.0,10.5), ios(2.0,2.0), watchos(2.0,2.0), tvos(9.0,9.0));
 
 */


static void KerJSObjectDynamicObjectSetReturnValue(duk_context * ctx, duk_idx_t idx,NSInvocation * anInvocation) {
    
    switch (* [[anInvocation methodSignature] methodReturnType]) {
        case 'c':
        {
            char v = duk_to_int(ctx, idx);
            [anInvocation setReturnValue:&v];
        }
            break;
        case 's':
        {
            short v = duk_to_int(ctx, idx);
            [anInvocation setReturnValue:&v];
        }
            break;
        case 'l':
        {
            long v = duk_to_int(ctx, idx);
            [anInvocation setReturnValue:&v];
        }
            break;
        case 'q':
        {
            long long v = 0;
            if(duk_is_string(ctx, idx)) {
                v = atoll(duk_to_string(ctx, idx));
            } else {
                v = duk_to_int(ctx, idx);
            }
            [anInvocation setReturnValue:&v];
        }
            break;
        case 'f':
        {
            float v = 0;
            if(duk_is_string(ctx, idx)) {
                v = atof(duk_to_string(ctx, idx));
            } else {
                v = duk_to_number(ctx, idx);
            }
            [anInvocation setReturnValue:&v];
        }
            break;
        case 'd':
        {
            double v = 0;
            if(duk_is_string(ctx, idx)) {
                v = atof(duk_to_string(ctx, idx));
            } else {
                v = duk_to_number(ctx, idx);
            }
            [anInvocation setReturnValue:&v];
        }
            break;
        case 'B':
        {
            BOOL v = 0;
            if(duk_is_string(ctx, idx)) {
                v = kk::CStringEqual(duk_to_string(ctx, idx),"true");
            } else {
                v = duk_to_boolean(ctx, idx);
            }
            [anInvocation setReturnValue:&v];
        }
            break;
        case '*':
        {
            const char * v = nullptr;
            if(duk_is_string(ctx, idx)) {
                v = duk_to_string(ctx, idx);
            }
            [anInvocation setReturnValue:&v];
        }
            break;
        case ':':
        {
            SEL v = nullptr;
            if(duk_is_string(ctx, idx)) {
                v = NSSelectorFromString([NSString stringWithCString: duk_to_string(ctx, idx) encoding:NSUTF8StringEncoding]);
            }
            [anInvocation setReturnValue:&v];
        }
            break;
        case '@':
        {
            id v = duk_to_NSObject(ctx, idx);
            [anInvocation setReturnValue:&v];
        }
            break;
        default:
            break;
    }
}

static void KerJSObjectDynamicObjectPushValue(duk_context * ctx, NSInvocation * anInvocation,int idx) {
    NSMethodSignature * s = [anInvocation methodSignature];
    if(idx < [s numberOfArguments]) {
        switch (* [s getArgumentTypeAtIndex:idx]) {
            case 'c':
            {
                char v = 0;
                [anInvocation getArgument:&v atIndex:idx];
                duk_push_int(ctx, v);
            }
                break;
            case 's':
            {
                short v = 0;
                [anInvocation getArgument:&v atIndex:idx];
                duk_push_int(ctx, v);
            }
                break;
            case 'l':
            {
                long v = 0;
                [anInvocation getArgument:&v atIndex:idx];
                duk_push_int(ctx, (int) v);
            }
                break;
            case 'q':
            {
                long long v = 0;
                [anInvocation getArgument:&v atIndex:idx];
                duk_push_sprintf(ctx, "%lld",v);
            }
                break;
            case 'f':
            {
                float v = 0;
                [anInvocation getArgument:&v atIndex:idx];
                duk_push_number(ctx, v);
            }
                break;
            case 'd':
            {
                double v = 0;
                [anInvocation getArgument:&v atIndex:idx];
                duk_push_number(ctx, v);
            }
                break;
            case 'B':
            {
                BOOL v = 0;
                [anInvocation getArgument:&v atIndex:idx];
                duk_push_boolean(ctx, v);
            }
                break;
            case '*':
            case ':':
            {
                const char * v = nullptr;
                [anInvocation getArgument:&v atIndex:idx];
                if(v == nullptr) {
                    duk_push_undefined(ctx);
                } else {
                    duk_push_string(ctx, v);
                }
            }
                break;
            case '@':
            {
                __unsafe_unretained id v = nil;
                [anInvocation getArgument:&v atIndex:idx];
                duk_push_NSObject(ctx, v);
            }
                break;
            default:
                duk_push_undefined(ctx);
                break;
        }
        
    } else {
        duk_push_undefined(ctx);
    }
}

static void KerJSObjectDynamicObjectGetProperty(KerJSObject * object,NSInvocation * anInvocation,NSString * name) {
    
    kk::JSObject * v = [object JSObject];
    
    if(v) {
        
        duk_context * ctx = v->jsContext();
        void * heapptr = v->heapptr();
        
        if(ctx && heapptr) {
            
            duk_push_heapptr(ctx, heapptr);
            
            duk_get_prop_string(ctx, -1, [name UTF8String]);
            
            KerJSObjectDynamicObjectSetReturnValue(ctx,-1,anInvocation);
            
            duk_pop_2(ctx);
           
        }
    }
    
}

static void KerJSObjectDynamicObjectSetProperty(KerJSObject * object,NSInvocation * anInvocation,NSString * name) {
    
    kk::JSObject * v = [object JSObject];
    
    if(v) {
        
        duk_context * ctx = v->jsContext();
        void * heapptr = v->heapptr();
        
        if(ctx && heapptr) {
            
            duk_push_heapptr(ctx, heapptr);
            
            duk_push_string(ctx, [name UTF8String]);
            KerJSObjectDynamicObjectPushValue(ctx,anInvocation,2);
            duk_put_prop(ctx, -3);
            
            duk_pop(ctx);
            
            
        }
    }
    
}

static void KerJSObjectDynamicObjectInvoke(KerJSObject * object,NSInvocation * anInvocation,NSString * name) {
    
    kk::JSObject * v = [object JSObject];
    
    if(v) {
        
        duk_context * ctx = v->jsContext();
        void * heapptr = v->heapptr();
        
        if(ctx && heapptr) {
            
            duk_push_heapptr(ctx, heapptr);
            
            duk_get_prop_string(ctx, -1, [name UTF8String]);
            
            if(duk_is_function(ctx, -1)) {
                
                duk_push_heapptr(ctx, heapptr);
                
                NSUInteger n = [[anInvocation methodSignature] numberOfArguments];
                
                for(int i=2;i<n;i++) {
                    KerJSObjectDynamicObjectPushValue(ctx,anInvocation,i);
                }
                
                if(duk_pcall_method(ctx, (duk_idx_t) n - 2) != DUK_EXEC_SUCCESS) {
                    kk::Error(ctx, -1, "[KerJSObjectDynamicObjectInvoke]");
                } else {
                    KerJSObjectDynamicObjectSetReturnValue(ctx,-1,anInvocation);
                }
                
            }
            
            duk_pop_2(ctx);
            
        }
    }
    
}

- (void)forwardInvocation:(NSInvocation *)anInvocation {
    
    NSString * name = NSStringFromSelector(anInvocation.selector);
    NSRange r = [name rangeOfString:@":"];
    
    if(r.location == NSNotFound) {
        if(protocol_getProperty(_protocol, [name UTF8String], YES, YES) != nullptr) {
            KerJSObjectDynamicObjectGetProperty(_object,anInvocation,name);
            return;
        }
    } else if([name hasPrefix:@"set"] && [name hasSuffix:@":"]) {
        NSRange range = {3,1};
        NSString * n = [[name substringWithRange:range] lowercaseString];
        range.location = 4;
        range.length = [name length] - 5;
        n = [n stringByAppendingString:[name substringWithRange:range]];
        if(protocol_getProperty(_protocol, [n UTF8String], YES, YES) != nullptr) {
            KerJSObjectDynamicObjectSetProperty(_object,anInvocation,n);
            return;
        }
    }

    if(r.location != NSNotFound) {
        name = [name substringToIndex:r.location];
    }
    
    KerJSObjectDynamicObjectInvoke(_object,anInvocation,name);
}

@end

@implementation KerJSObject

@synthesize JSObject = _JSObject;

-(instancetype) initWithJSObject:(kk::JSObject *) object {
    if((self = [super init])) {
        _JSObject = object;
        if(_JSObject) {
            _JSObject->retain();
        }
    }
    return self;
}

-(void) dealloc {
    if(_JSObject) {
        _JSObject->release();
    }
}


-(id) propertyForKey:(NSString *) key {
    if(_JSObject) {
        duk_context * ctx = _JSObject->jsContext();
        void * heapptr = _JSObject->heapptr();
        if(ctx && heapptr) {
            duk_push_heapptr(ctx, heapptr);
            duk_get_prop_string(ctx, -1, [key UTF8String]);
            id v = duk_to_NSObject(ctx, -1);
            duk_pop_2(ctx);
            return v;
        }
    }
    return nil;
}

-(void) setProperty:(id) value forKey:(NSString *) key {
    if(_JSObject) {
        duk_context * ctx = _JSObject->jsContext();
        void * heapptr = _JSObject->heapptr();
        if(ctx && heapptr) {
            duk_push_heapptr(ctx, heapptr);
            duk_push_string(ctx, [key UTF8String]);
            duk_push_NSObject(ctx, value);
            duk_put_prop(ctx, -3);
            duk_pop(ctx);
        }
    }
}

-(id) invoke:(NSString *) name arguments:(NSArray *) arguments {
    
    if(_JSObject) {
        duk_context * ctx = _JSObject->jsContext();
        void * heapptr = _JSObject->heapptr();
        if(ctx && heapptr) {
            
            id v = nil;
            
            duk_push_heapptr(ctx, heapptr);
            
            duk_get_prop_string(ctx, -1, [name UTF8String]);
            
            if(duk_is_function(ctx, -1)) {
                
                duk_push_heapptr(ctx, heapptr);
                
                for(id v in arguments) {
                    duk_push_NSObject(ctx, v);
                }
                
                if(duk_pcall_method(ctx, (duk_idx_t) [arguments count]) != DUK_EXEC_SUCCESS) {
                    kk::Error(ctx, -1, "[KerJSObject] [invoke]");
                } else {
                    v = duk_to_NSObject(ctx, -1);
                }
                
            }
            
            duk_pop_2(ctx);
            
            return v;
        }
    }
    
    return nil;
}

-(id) callWithArguments:(NSArray *) arguments {
    
    if(_JSObject) {
        duk_context * ctx = _JSObject->jsContext();
        void * heapptr = _JSObject->heapptr();
        if(ctx && heapptr) {
            
            id v = nil;
            
            duk_push_heapptr(ctx, heapptr);

            if(duk_is_function(ctx, -1)) {
                
                for(id v in arguments) {
                    duk_push_NSObject(ctx, v);
                }
                
                if(duk_pcall(ctx, (duk_idx_t) [arguments count]) != DUK_EXEC_SUCCESS) {
                    kk::Error(ctx, -1, "[KerJSObject] [callWithArguments]");
                } else {
                    v = duk_to_NSObject(ctx, -1);
                }
                
            }
            
            duk_pop(ctx);
            
            return v;
        }
    }
    
    return nil;
    
}


-(id) implementProtocol:(Protocol *) protocol {
    return [[KerJSObjectDynamicObject alloc] initWithProtocol:protocol object:self];
}

@end