//
//  KerWXObject+Http.m
//  KerWX
//
//  Created by hailong11 on 2018/11/16.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+Http.h"

@interface KerWXRequestTask : NSObject<KerWXRequestTask>

@end

@implementation KerWXRequestTask

-(void) dealloc {
    NSLog(@"[KerWXRequestTask] [dealloc]");
}

-(void) abort {
    NSLog(@"");
}

@end

@implementation KerWXObject (Http)

-(id<KerWXRequestTask>) request:(id<KerWXRequestObject>) object {
    return [[KerWXRequestTask alloc] init];
}

@end
