//
//  KerWXObject+Location.m
//  KerWX
//
//  Created by hailong11 on 2018/11/15.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+Location.h"

@implementation WXGetLocationRes

-(void) dealloc {
    NSLog(@"[WXGetLocationRes] [dealloc]");
}

@end

@implementation KerWXObject (Location)

-(void) getLocation:(KerJSObject *) object {
    
    id<WXGetLocationObject> v = [object implementProtocol:@protocol(WXGetLocationObject)];
    
    v.type = @"OK";
    
    NSLog(@"%@",v.type);
    
    WXGetLocationRes * r = [[WXGetLocationRes alloc] init];
    
    [v success:r errmsg:@"OK"];
//
//    dispatch_async(dispatch_get_main_queue(), ^{
//        WXGetLocationRes * r = [[WXGetLocationRes alloc] init];
//        [v success:r errmsg:@"OK"];
//    });
    
}

@end
