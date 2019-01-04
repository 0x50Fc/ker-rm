//
//  KerStrong.h
//  Ker
//
//  Created by hailong11 on 2019/1/3.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>


#if defined(__cplusplus)

#include <core/kk.h>
#include <core/dispatch.h>

namespace kk {
    class Object;
    class DispatchQueue;
}

typedef kk::Object * KerObjectRef;
typedef kk::DispatchQueue * KerDispatchQueueRef;

#else

typedef void * KerObjectRef;
typedef void * KerDispatchQueueRef;

#endif

@interface KerStrong : NSObject

@property(nonatomic,readonly,assign) KerObjectRef object;
@property(nonatomic,readonly,assign) KerDispatchQueueRef queue;

-(instancetype) initWithObject:(KerObjectRef) object queue:(KerDispatchQueueRef) queue;

-(void) recycle;

@end

