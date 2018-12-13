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

void ker_push_NSObject(duk_context * ctx, id object);
id ker_to_NSObject(duk_context * ctx,duk_idx_t idx);

kk::Any KerObjectToAny(id object);
id KerObjectFromAny(kk::Any & v);

namespace kk {
    
    namespace objc {
        
        void PushInterface(duk_context * ctx, ::Class isa);
        
        void PushProtocol(duk_context * ctx, Protocol * protocol);
        
    }
    
}


typedef kk::JSObject * KerJSObjectRef;

#else

typedef void * KerJSObjectRef;

#endif

@interface KerJSONDataObject : NSObject

-(instancetype) initWithData:(NSData *) data;

@property(nonatomic,strong,readonly) NSData * data;

@end

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

#ifdef __cplusplus
extern "C" {
#endif
    
    void KerAddOpenlibInterface(Class isa);
    void KerAddOpenlibProtocol(Protocol * protocol);
    
    typedef void (^KerOpenlibSetLibrary)(NSString * name,id object);
    
    typedef void (^KerAddOpenlibFunction)(NSString * basePath,NSString * appkey,KerOpenlibSetLibrary setLibrary);
    
    void KerAddPageOpenlib(KerAddOpenlibFunction func);
    
    void KerAddAppOpenlib(KerAddOpenlibFunction func);
    
#ifdef __cplusplus
}
#endif

