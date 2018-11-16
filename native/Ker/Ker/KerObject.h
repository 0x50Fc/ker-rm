//
//  KerObject.h
//  KK
//
//  Created by zhanghailong on 2018/11/2.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>

#if defined(__cplusplus)

#include <core/kk.h>
#include <core/jit.h>

#include <objc/runtime.h>

void duk_push_NSObject(duk_context * ctx, id object);
id duk_to_NSObject(duk_context * ctx,duk_idx_t idx);

kk::Any KerObjectToAny(id object);
id KerObjectFromAny(kk::Any & v);

namespace kk {
    
    namespace objc {
        
        void PushInterface(duk_context * ctx,kk::CString name,kk::CString base, std::function<void(duk_context *)> && func);
        
        
        template<typename TReturn,typename ... TArgs>
        void PutMethod(duk_context * ctx,duk_idx_t idx,::Class isa,kk::CString name,SEL selector,typename std::enable_if<std::is_void<TReturn>::value>::type* = 0) {
            
            ::Method m = class_getInstanceMethod(isa, selector);
            
            duk_push_string(ctx, name);
            duk_push_c_function(ctx, [](duk_context *ctx)->duk_ret_t{
                
                duk_push_this(ctx);
                
                kk::NativeObject * v = dynamic_cast<kk::NativeObject *>(kk::GetObject(ctx, -1));
                
                duk_pop(ctx);
                
                duk_push_current_function(ctx);
                
                duk_get_prop_string(ctx, -1, "__method");
                
                ::Method m = (::Method) duk_to_pointer(ctx, -1);
                
                duk_pop_2(ctx);
                
                if(v != nullptr && m != nullptr) {
                    
                    std::function<TReturn(TArgs...)> fn([v,m](TArgs ... args)->TReturn{
                        id (*func)(id,SEL,TArgs ...) = (id (*)(id,SEL,TArgs ...)) method_getImplementation(m);
                        @autoreleasepool {
                            (*func)((__bridge id) v->native(),method_getName(m),args...);
                        }
                    });
                    
                    kk::Call<TReturn,TArgs...>(std::move(fn), ctx);
                    
                }
                
                return 0;
                
            }, sizeof...(TArgs));
            
            duk_push_pointer(ctx, (void *)m);
            duk_put_prop_string(ctx, -2, "__method");
            
            duk_def_prop(ctx, idx - 2, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_WRITABLE | DUK_DEFPROP_CLEAR_CONFIGURABLE);
            
        }
        
        
        template<typename TReturn,typename ... TArgs>
        void PutMethod(duk_context * ctx,duk_idx_t idx,::Class isa,kk::CString name,SEL selector,typename std::enable_if<!std::is_void<TReturn>::value>::type* = 0) {
            
            ::Method m = class_getInstanceMethod(isa, selector);
            
            duk_push_string(ctx, name);
            duk_push_c_function(ctx, [](duk_context *ctx)->duk_ret_t{
                
                duk_push_this(ctx);
                
                kk::NativeObject * v = dynamic_cast<kk::NativeObject *>(kk::GetObject(ctx, -1));
                
                duk_pop(ctx);
                
                duk_push_current_function(ctx);
                
                duk_get_prop_string(ctx, -1, "__method");
                
                ::Method m = (::Method) duk_to_pointer(ctx, -1);
                
                duk_pop_2(ctx);
                
                if(v != nullptr && m != nullptr) {
                    
                    std::function<TReturn(TArgs...)> fn([v,m](TArgs ... args)->TReturn{
                        id (*func)(id,SEL,TArgs ...) = (id (*)(id,SEL,TArgs ...)) method_getImplementation(m);
                        @autoreleasepool {
                            kk::Any r((__bridge kk::Native *) (*func)((__bridge id) v->native(),method_getName(m),args...));
                            return (TReturn) r;
                        }
                        
                    });
                    
                    return kk::Call<TReturn,TArgs...>(std::move(fn), ctx);
                    
                }
                
                return 0;
                
            }, sizeof...(TArgs));
            
            duk_push_pointer(ctx, (void *)m);
            duk_put_prop_string(ctx, -2, "__method");
            
            duk_def_prop(ctx, idx - 2, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_WRITABLE | DUK_DEFPROP_CLEAR_CONFIGURABLE);
            
        }
        
        template<typename TReturn,typename ... TArgs>
        void PutMethod(duk_context * ctx,duk_idx_t idx,kk::CString name,SEL selector,typename std::enable_if<std::is_void<TReturn>::value>::type* = 0) {
            
            duk_push_string(ctx, name);
            duk_push_c_function(ctx, [](duk_context *ctx)->duk_ret_t{
                
                duk_push_this(ctx);
                
                kk::NativeObject * v = dynamic_cast<kk::NativeObject *>(kk::GetObject(ctx, -1));
                
                duk_pop(ctx);
                
                duk_push_current_function(ctx);
                
                duk_get_prop_string(ctx, -1, "__selector");
                
                SEL selector = sel_registerName(duk_to_string(ctx, -1));
                
                duk_pop_2(ctx);
                
                
                if(v != nullptr && selector != nullptr) {
                    
                    ::Class isa = object_getClass((__bridge id) v->native());
                    ::Method m = class_getInstanceMethod(isa, selector);
                    
                    std::function<TReturn(TArgs...)> fn([v,m](TArgs ... args)->TReturn{
                        id (*func)(id,SEL,TArgs ...) = (id (*)(id,SEL,TArgs ...)) method_getImplementation(m);
                        @autoreleasepool {
                            (*func)((__bridge id) v->native(),method_getName(m),args...);
                        }
                    });
                    
                    kk::Call<TReturn,TArgs...>(std::move(fn), ctx);
                    
                }
                
                return 0;
                
            }, sizeof...(TArgs));
            
            duk_push_string(ctx, sel_getName(selector));
            duk_put_prop_string(ctx, -2, "__selector");
            
            duk_def_prop(ctx, idx - 2, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_WRITABLE | DUK_DEFPROP_CLEAR_CONFIGURABLE);
            
        }
        
        template<typename TReturn,typename ... TArgs>
        void PutMethod(duk_context * ctx,duk_idx_t idx,kk::CString name,SEL selector,typename std::enable_if<!std::is_void<TReturn>::value>::type* = 0) {
            
            duk_push_string(ctx, name);
            duk_push_c_function(ctx, [](duk_context *ctx)->duk_ret_t{
                
                duk_push_this(ctx);
                
                kk::NativeObject * v = dynamic_cast<kk::NativeObject *>(kk::GetObject(ctx, -1));
                
                duk_pop(ctx);
                
                duk_push_current_function(ctx);
                
                duk_get_prop_string(ctx, -1, "__selector");
                
                SEL selector = sel_registerName(duk_to_string(ctx, -1));
                
                duk_pop_2(ctx);
                
                
                if(v != nullptr && selector != nullptr) {
                    
                    ::Class isa = object_getClass((__bridge id) v->native());
                    ::Method m = class_getInstanceMethod(isa, selector);
                    
                    std::function<TReturn(TArgs...)> fn([v,m](TArgs ... args)->TReturn{
                        id (*func)(id,SEL,TArgs ...) = (id (*)(id,SEL,TArgs ...)) method_getImplementation(m);
                        @autoreleasepool {
                            return (*func)((__bridge id) v->native(),method_getName(m),args...);
                        }
                    });
                    
                    return kk::Call<TReturn,TArgs...>(std::move(fn), ctx);
                    
                }
                
                return 0;
                
            }, sizeof...(TArgs));
            
            duk_push_string(ctx, sel_getName(selector));
            duk_put_prop_string(ctx, -2, "__selector");
            
            duk_def_prop(ctx, idx - 2, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_WRITABLE | DUK_DEFPROP_CLEAR_CONFIGURABLE);
            
        }
        
    }
    
}


typedef kk::JSObject * KerJSObjectRef;

#else

typedef void * KerJSObjectRef;

#endif

@interface KerJSObject : NSObject

@property(nonatomic,readonly,assign) KerJSObjectRef JSObject;

-(id) propertyForKey:(NSString *) key;

-(void) setProperty:(id) value forKey:(NSString *) key;

-(id) invoke:(NSString *) name arguments:(NSArray *) arguments;

-(id) callWithArguments:(NSArray *) arguments;

-(id) implementProtocol:(Protocol *) protocol;

@end


@interface NSObject(KerObject)

-(id) ker_getValue:(NSString *) key;

-(NSString *) ker_getString:(NSString *) key;

-(NSString *) ker_stringValue;

@end

@protocol KerJSExport

@end

