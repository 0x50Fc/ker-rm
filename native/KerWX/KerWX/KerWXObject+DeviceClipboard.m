//
//  KerWXObject+DeviceClipboard.m
//  KerWX
//
//  Created by zuowu on 2018/12/12.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+DeviceClipboard.h"

@implementation WXGetClipboardRes

@synthesize errMsg = _errMsg;
@synthesize data = _data;

-(instancetype) initWithData:(NSString *)data errMsg:(NSString *)msg{
    if (self = [super init]) {
        self.data = data;
        self.errMsg = msg;
    }
    return self;
}

@end


@implementation KerWXObject (DeviceClipboard)

-(void) setClipboardData:(KerJSObject *)object{
    
    id<WXSetClipboardDataObject> v = [object implementProtocol:@protocol(WXSetClipboardDataObject)];
    [UIPasteboard generalPasteboard].string = v.data;
    
    WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"setClipboardData:ok"];
    dispatch_async(dispatch_get_main_queue(), ^{
        [v success:res];
        [v complete:res];
    });
    
}

-(void) getClipboardData:(KerJSObject *)object{
    
    NSString * data = [UIPasteboard generalPasteboard].string;
    WXGetClipboardRes * res = [[WXGetClipboardRes alloc] initWithData:data errMsg:@"getClipboardData:ok"];
    
    id<WXGetClipboardDataObject> v = [object implementProtocol:@protocol(WXGetClipboardDataObject)];
    dispatch_async(dispatch_get_main_queue(), ^{
        [v success:res];
        [v complete:res];
    });
    
}

@end
