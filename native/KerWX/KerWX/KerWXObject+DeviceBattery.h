//
//  KerWXObject+DeviceBattery.h
//  KerWX
//
//  Created by zuowu on 2018/12/12.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>

@interface WXGetBatteryInfoRes : WXCallbackRes

@property (nonatomic, copy) NSString * level;
@property (nonatomic, assign) BOOL isCharging;

@end


@protocol WXGetBatteryInfoObject <WXCallbackFunction>

@end

@interface KerWXObject (DeviceBattery)

-(void) getBatteryInfo:(KerJSObject *) object;
//-(void) getBatteryInfoSync:(KerJSObject *) object; iOS 不可用

@end


