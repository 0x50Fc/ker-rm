//
//  KerWXObject+DeviceClipboard.h
//  KerWX
//
//  Created by zuowu on 2018/12/12.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>

@interface WXGetClipboardRes : WXCallbackRes

@property (nonatomic, copy) NSString * data;   /*getClipboardRes*/

@end

@protocol WXGetClipboardDataObject <WXCallbackFunction>

@end

@protocol WXSetClipboardDataObject <WXCallbackFunction>

@property (nonatomic, strong) NSString * data;

@end

@interface KerWXObject (DeviceClipboard)

-(void) setClipboardData:(KerJSObject *)object;
-(void) getClipboardData:(KerJSObject *)object;

@end

