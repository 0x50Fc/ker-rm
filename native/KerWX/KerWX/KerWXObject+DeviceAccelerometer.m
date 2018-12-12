//
//  KerWXObject+DeviceAccelerometer.m
//  KerWX
//
//  Created by zuowu on 2018/12/12.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+DeviceAccelerometer.h"
#include <objc/runtime.h>

#define WXAccelerometerKey "WXAccelerometerKey"

@implementation WXOnAccelerometerChangeRes

@synthesize x = _x;
@synthesize y = _y;
@synthesize z = _z;

-(instancetype)initWithCMAccelerometerData:(CMAccelerometerData *) data{
    if (self = [super init]) {
        self.x = data.acceleration.x;
        self.y = data.acceleration.y;
        self.z = data.acceleration.z;
    }
    return self;
}

@end



@interface WXAccelerometer : NSObject

@property (nonatomic, strong, readonly) CMMotionManager * motionManager;
@property (nonatomic, strong) KerJSObject * onAccelerometerChange;

//-(void) startAccelerometer:(KerJSObject *) object;
//-(void) stopAccelerometer:(KerJSObject *) object;

@end

@implementation WXAccelerometer : NSObject

@synthesize motionManager = _motionManager;
@synthesize onAccelerometerChange = _onAccelerometerChange;

-(CMMotionManager *)motionManager{
    if (_motionManager == nil) {
        _motionManager = [[CMMotionManager alloc] init];
    }
    return _motionManager;
}

-(void) startAccelerometer:(KerJSObject *) object{
    
    id<WXStartAccelerometerObject> v = [object implementProtocol:@protocol(WXStartAccelerometerObject)];
    
    if (!self.motionManager.isAccelerometerAvailable) {
        
        // Accelerometer 不可用
        dispatch_async(dispatch_get_main_queue(), ^{
            WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"startAccelerometer:err - magnetometer is not active"];
            [v fail:res];
            [v complete:res];
        });
        
        
    } else {
        
        //成功回调
        self.motionManager.magnetometerUpdateInterval = [v.interval ker_toInterval];
        dispatch_async(dispatch_get_main_queue(), ^{
            WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"startAccelerometer:ok"];
            [v success:res];
            [v complete:res];
        });

        [self.motionManager startAccelerometerUpdatesToQueue: [NSOperationQueue mainQueue] withHandler:^(CMAccelerometerData * _Nullable accelerometerData, NSError * _Nullable error) {

            if (self.onAccelerometerChange) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [self.onAccelerometerChange callWithArguments:@[[[WXOnAccelerometerChangeRes alloc] initWithCMAccelerometerData:accelerometerData]]];
                });
            }
        }];
        
    }
}

-(void)stopAccelerometer:(KerJSObject *) object{
    
    [self.motionManager stopAccelerometerUpdates];
    
    WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"stopAccelerometer:ok"];
    id<WXStopAccelerometerObject> v = [object implementProtocol:@protocol(WXStopAccelerometerObject)];
    dispatch_async(dispatch_get_main_queue(), ^{
        [v success:res];
        [v complete:res];
    });

}

@end




@implementation KerWXObject (DeviceAccelerometer)

-(WXAccelerometer *)WXAccelerometer{
    WXAccelerometer * v = objc_getAssociatedObject(self, WXAccelerometerKey);
    if(v == nil) {
        v = [[WXAccelerometer alloc]init];
        objc_setAssociatedObject(self, WXAccelerometerKey, v, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return v;
}

-(void) startAccelerometer:(KerJSObject *) object{
    
    [self.WXAccelerometer startAccelerometer:object];

}

-(void) stopAccelerometer:(KerJSObject *) object{
    
    [self.WXAccelerometer stopAccelerometer:object];
}

-(void) onAccelerometerChange:(KerJSObject *) object {
    
    self.WXAccelerometer.onAccelerometerChange = object;

}

@end


@implementation NSString (DeviceAccelerometer)

-(NSTimeInterval) ker_toInterval{
    NSTimeInterval result = 0.2;
    if ([self isEqualToString:@"game"]) {
        result = 0.02;
    }else if ([self isEqualToString:@"ui"]){
        result = 0.06;
    }else if ([self isEqualToString:@"normal"]){
        result = 0.6;
    }
    return result;
}

@end
