//
//  KerWXObject+DeviceAccelerometer.h
//  KerWX
//
//  Created by zuowu on 2018/12/12.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>
#import <CoreMotion/CoreMotion.h>

@protocol WXStartAccelerometerObject <WXCallbackFunction>

@property(nonatomic, copy) NSString * interval;

@end

@protocol WXStopAccelerometerObject <WXCallbackFunction>

@end

@protocol WXStartGyroscopeObject <WXCallbackFunction>

@property (nonatomic, copy) NSString * interval;

@end

@protocol WXStartDeviceMotionListeningObject <WXCallbackFunction>

@property (nonatomic, copy) NSString * interval;

@end


@interface WXOnAccelerometerChangeRes : NSObject

@property(nonatomic,assign) double x;
@property(nonatomic,assign) double y;
@property(nonatomic,assign) double z;

@end

@interface WXOnGyroscopeChangeRes : NSObject

@property (nonatomic, assign) double x;
@property (nonatomic, assign) double y;
@property (nonatomic, assign) double z;

@end

@interface WXOnDeviceMotionChangeRes : NSObject

@property (nonatomic, assign) double alpha;
@property (nonatomic, assign) double beta;
@property (nonatomic, assign) double gamma;
@property (nonatomic, copy) NSString * testStr;

@end


@interface KerWXObject (DeviceAccelerometer)

-(void) startAccelerometer:(KerJSObject *) object;
-(void) stopAccelerometer:(KerJSObject *) object;
-(void) onAccelerometerChange:(KerJSObject *) object;

-(void) startGyroscope:(KerJSObject *) object;
-(void) stopGyroscope:(KerJSObject *) object;
-(void) onGyroscopeChange:(KerJSObject *) object;

-(void) startDeviceMotionListening:(KerJSObject *) object;
-(void) stopDeviceMotionListening:(KerJSObject *) object;
-(void) onDeviceMotionChange:(KerJSObject *) object;

@end

@interface NSString (DeviceAccelerometer)

-(NSTimeInterval) ker_toInterval;

@end


