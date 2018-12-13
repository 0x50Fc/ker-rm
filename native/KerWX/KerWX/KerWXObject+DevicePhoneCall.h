//
//  KerWXObject+DevicePhoneCall.h
//  KerWX
//
//  Created by zuowu on 2018/12/13.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>


@protocol WXMakePhoneCallObject <WXCallbackFunction>

@property (nonatomic, assign) NSString * phoneNumber;

@end


@interface KerWXObject (DevicePhoneCall)

-(void)makePhoneCall:(KerJSObject *) object;

@end


