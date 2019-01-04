//
//  KerStrong.m
//  Ker
//
//  Created by hailong11 on 2019/1/3.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#import "KerStrong.h"

@implementation KerStrong

@synthesize object = _object;
@synthesize queue = _queue;

-(instancetype) initWithObject:(KerObjectRef) object queue:(KerDispatchQueueRef) queue {
    if((self = [super init])) {
        _object = object;
        _queue = queue;
        if(object) {
            object->retain();
        }
        if(queue) {
            queue->retain();
        }
    }
    return self;
}

-(void) recycle {
    if(_object) {
        if(_queue) {
            kk::Object * object = _object;
            kk::DispatchQueue * queue = _queue;
            _queue->async([object,queue]()->void{
                object->release();
                queue->release();
            });
        } else {
            _object->release();
        }
    }
    _object = nullptr;
    _queue = nullptr;
}

-(void) dealloc {
    if(_object) {
        if(_queue) {
            kk::Object * object = _object;
            kk::DispatchQueue * queue = _queue;
            _queue->async([object,queue]()->void{
                object->release();
                queue->release();
            });
        } else {
            _object->release();
        }
    }
}

@end
