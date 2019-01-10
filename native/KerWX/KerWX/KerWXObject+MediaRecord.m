//
//  KerWXObject+MediaRecord.m
//  KerWX
//
//  Created by zuowu on 2018/12/29.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+MediaRecord.h"


@interface KerWXRecorderManager : NSObject<KerWXRecorderManager>



@end

@implementation KerWXRecorderManager


-(void) dealloc {
    NSLog(@"[KerWXRecorderManager] [dealloc]");
}

-(void) start:(KerJSObject *)object{
    NSLog(@"KerWXRecorderManager star");
}

@end


@implementation KerWXObject (MediaRecord)

-(void) startRecord:(KerJSObject *) object{
    
}

-(void) stopRecord:(KerJSObject *) object{
    
}

-(id<KerWXRecorderManager>) getRecorderManager{
    return [[KerWXRecorderManager alloc] init];
}

@end
