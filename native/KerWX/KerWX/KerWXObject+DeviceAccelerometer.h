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


@interface WXOnAccelerometerChangeRes : NSObject

@property(nonatomic,assign) double x;
@property(nonatomic,assign) double y;
@property(nonatomic,assign) double z;

-(instancetype)initWithCMAccelerometerData:(CMAccelerometerData *) data;

@end


@interface KerWXObject (DeviceAccelerometer)

-(void) startAccelerometer:(KerJSObject *) object;
-(void) stopAccelerometer:(KerJSObject *) object;
-(void) onAccelerometerChange:(KerJSObject *) object;

@end

@interface NSString (DeviceAccelerometer)

-(NSTimeInterval) ker_toInterval;

@end


