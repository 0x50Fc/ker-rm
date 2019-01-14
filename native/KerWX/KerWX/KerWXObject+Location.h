//
//  KerWXObject+Location.h
//  KerWX
//
//  Created by hailong11 on 2018/11/15.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWXObject.h>
#import <Ker/Ker.h>
#import <CoreLocation/CoreLocation.h>


@interface WXGetLocationRes : WXCallbackRes

@property(nonatomic,assign) double latitude;
@property(nonatomic,assign) double longitude;
@property(nonatomic,assign) double speed;
@property(nonatomic,assign) double accuracy;
@property(nonatomic,assign) double altitude;
@property(nonatomic,assign) double verticalAccuracy;
@property(nonatomic,assign) double horizontalAccuracy;

@end


@interface WXOnCompassChageRes : NSObject

@property (nonatomic, assign) double direction;
@property (nonatomic, assign) double accuracy;

@end


@protocol WXGetLocationObject <WXCallbackFunction>

@property(nonatomic,strong) NSString * type;
@property(nonatomic,assign,readonly) BOOL altitude;

@end


@interface KerWXObject (Location)

-(void) getLocation:(KerJSObject *) object;
-(void) openLocation:(KerJSObject *) object;

-(void) startCompass:(KerJSObject *) object;
-(void) stopCompass:(KerJSObject *) object;

-(void) onCompassChange:(KerJSObject *) object;

@end


@interface CLLocation (WXLocation)
/*CoreLocation 获取的地址为wgs84 这个方法可以生成一个转换成 gcj02 标准的坐标*/
-(CLLocationCoordinate2D)ker_generateGCJ02Coordinate;

@end
