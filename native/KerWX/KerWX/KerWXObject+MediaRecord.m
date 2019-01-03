//
//  KerWXObject+MediaRecord.m
//  KerWX
//
//  Created by zuowu on 2018/12/29.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+MediaRecord.h"

@implementation RecorderManager : KerWXObject

-(void) start{
    NSLog(@"123");
}

@end

@implementation KerWXObject (MediaRecord)

-(void) startRecord:(KerJSObject *) object{
    
}

-(void) stopRecord:(KerJSObject *) object{
    
}

-(KerWXObject *) getRecorderManager{
    return [[RecorderManager alloc] init];
}

@end
