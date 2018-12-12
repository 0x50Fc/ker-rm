//
//  KerWXObject+DeviceBattery.m
//  KerWX
//
//  Created by zuowu on 2018/12/12.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+DeviceBattery.h"

@implementation WXGetBatteryInfoRes

@synthesize level = _level;
@synthesize isCharging = _isCharging;
@synthesize errMsg = _errMsg;

-(instancetype)initWithLevel:(float) level BatteryState:(UIDeviceBatteryState) state errMsg:(NSString *) msg {
    if (self = [super init]) {
        if (level >= 0.0f && level <= 1.0f) {
            int Il = level * 100;
            self.level = [NSString stringWithFormat:@"%d", Il];
        }else {
            self.level = @"100";
        }
        self.isCharging = state == UIDeviceBatteryStateCharging ? YES: NO;
        self.errMsg = msg;
    }
    return self;
}

@end

@implementation KerWXObject (DeviceBattery)

-(void) getBatteryInfo:(KerJSObject *) object{
    
    id<WXGetBatteryInfoObject> v = [object implementProtocol:@protocol(WXGetBatteryInfoObject)];
    UIDevice * device = [UIDevice currentDevice];
    device.batteryMonitoringEnabled = YES;
    
    WXGetBatteryInfoRes * res = [[WXGetBatteryInfoRes alloc] initWithLevel:device.batteryLevel BatteryState:device.batteryState errMsg:@"getBatteryInfo:ok"];
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [v success:res];
        [v complete:res];
    });
}

@end
