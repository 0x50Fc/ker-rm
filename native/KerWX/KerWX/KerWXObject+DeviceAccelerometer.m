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

-(void) dealloc {
    NSLog(@"[WXOnAccelerometerChangeRes] [dealloc]");
}

@end


@implementation WXOnGyroscopeChangeRes

@synthesize x = _x;
@synthesize y = _y;
@synthesize z = _z;

-(instancetype)initWithCMGyroData:(CMGyroData *) data {
    if (self = [super init]) {
        self.x = data.rotationRate.x;
        self.y = data.rotationRate.y;
        self.z = data.rotationRate.z;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXOnGyroscopeChangeRes] [dealloc]");
}

@end

@implementation WXOnDeviceMotionChangeRes

@synthesize alpha = _alpha;
@synthesize beta = _beta;
@synthesize gamma = _gamma;
@synthesize testStr = _testStr;

-(instancetype)initWithCMDeviceMotion:(CMDeviceMotion *) motion{
    if (self = [super init]) {
        /*还不会计算 回头研究下*/
        self.alpha = 0;
        self.beta = 0;
        self.gamma = 0;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXOnDeviceMotionChangeRes] [dealloc]");
}

@end



@interface WXAccelerometer : NSObject

@property (nonatomic, strong, readonly) CMMotionManager * motionManager;
@property (nonatomic, strong) KerJSObject * onAccelerometerChange;
@property (nonatomic, strong) KerJSObject * onGyroscopeChange;
@property (nonatomic, strong) KerJSObject * onDeviceMotionChange;

@end

@implementation WXAccelerometer : NSObject

@synthesize motionManager = _motionManager;
@synthesize onAccelerometerChange = _onAccelerometerChange;
@synthesize onGyroscopeChange = _onGyroscopeChange;
@synthesize onDeviceMotionChange = _onDeviceMotionChange;

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

-(void) startGyroscope:(KerJSObject *) object {
    
    id<WXStartGyroscopeObject> v = [object implementProtocol:@protocol(WXStartGyroscopeObject)];
    
    if (!self.motionManager.isGyroAvailable) {
        
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"startGyroscope:err - gyroscope is not active"];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });
        
    }else{
        
        self.motionManager.gyroUpdateInterval = [v.interval ker_toInterval];
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"startGyroscope:ok"];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v success:res];
            [v complete:res];
        });

        [self.motionManager startGyroUpdatesToQueue: [NSOperationQueue mainQueue] withHandler:^(CMGyroData * _Nullable gyroData, NSError * _Nullable error) {
            if (self.onGyroscopeChange) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [self.onGyroscopeChange callWithArguments:@[[[WXOnGyroscopeChangeRes alloc] initWithCMGyroData:gyroData]]];
                });
            }
        }];
    }
    
}

-(void) stopGyroscope:(KerJSObject *) object{
    
    [self.motionManager stopGyroUpdates];
    
    id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];
    WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"stopGyroscope:ok"];
    dispatch_async(dispatch_get_main_queue(), ^{
        [v success:res];
        [v complete:res];
    });
}

-(void) startDeviceMotionListening:(KerJSObject *) object{
    
    id<WXStartDeviceMotionListeningObject> v = [object implementProtocol:@protocol(WXStartDeviceMotionListeningObject)];
    
    if (!self.motionManager.isDeviceMotionAvailable) {
        
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"startDeviceMotionListening:fail DeviceMotion 不可用"];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });
        
    }else {
        
        self.motionManager.deviceMotionUpdateInterval = [v.interval ker_toInterval];
        [self.motionManager startDeviceMotionUpdatesToQueue:[NSOperationQueue mainQueue] withHandler:^(CMDeviceMotion * _Nullable motion, NSError * _Nullable error) {
            if (self.onDeviceMotionChange) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    //还不会计算 三个值
                    [self.onDeviceMotionChange callWithArguments:@[[[WXOnDeviceMotionChangeRes alloc] initWithCMDeviceMotion:motion]]];
                });
            }
        }];
        
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"startDeviceMotionListening:ok"];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v success:res];
            [v complete:res];
        });
    }
    
}

-(void) stopDeviceMotionListening:(KerJSObject *) object{
    
    id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];
    [self.motionManager stopDeviceMotionUpdates];
    WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"stopDeviceMotionListening:ok"];
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

-(void) startGyroscope:(KerJSObject *) object {
    [self.WXAccelerometer startGyroscope:object];
}

-(void) stopGyroscope:(KerJSObject *) object{
    [self.WXAccelerometer stopGyroscope:object];
}

-(void) onGyroscopeChange:(KerJSObject *) object{
    self.WXAccelerometer.onGyroscopeChange = object;
}

-(void) startDeviceMotionListening:(KerJSObject *) object{
    
}
-(void) stopDeviceMotionListening:(KerJSObject *) object{
    
}
-(void) onDeviceMotionChange:(KerJSObject *) object{
    
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
