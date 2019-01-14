//
//  KerWXObject+Location.m
//  KerWX
//
//  Created by hailong11 on 2018/11/15.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+Location.h"
#import <objc/runtime.h>

#define WXLocationKey "WXLocationKey"

@implementation WXGetLocationRes

@synthesize latitude = _latitude;
@synthesize longitude = _longitude;
@synthesize speed = _speed;
@synthesize accuracy = _accuracy;
@synthesize altitude = _altitude;
@synthesize verticalAccuracy = _verticalAccuracy;
@synthesize horizontalAccuracy = _horizontalAccuracy;
@synthesize errMsg = _errMsg;

-(instancetype)initWithCLLocation:(CLLocation *)location type:(NSString *)type errMsg:(NSString *)errMsg{
    if (self = [super init]) {
        CLLocationCoordinate2D coordinate = location.coordinate;
        //默认为wgs84 如果设置gcj02 则计算相应系统坐标
        if ([type isEqualToString:@"gcj02"]) {
            coordinate = [location ker_generateGCJ02Coordinate];
        }
        self.latitude = coordinate.latitude;
        self.longitude = coordinate.longitude;
        self.speed = location.speed;
        self.accuracy = kCLLocationAccuracyBest;
        self.altitude = location.altitude;
        self.verticalAccuracy = location.verticalAccuracy;
        self.horizontalAccuracy = location.horizontalAccuracy;
        
        self.errMsg = errMsg;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXGetLocationRes] [dealloc]");
}

@end

@implementation WXOnCompassChageRes

@synthesize direction = _direction;
@synthesize accuracy = _accuracy;

-(instancetype)initWithHeading:(CLHeading *)heading{
    if (self = [super init]) {
        self.accuracy = heading.headingAccuracy;
        self.direction = heading.trueHeading;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXOnCompassChageRes] [dealloc]");
}

@end

@interface WXLocation : NSObject <CLLocationManagerDelegate>

@property (nonatomic, strong, readonly) CLLocationManager * locationManager;
@property (nonatomic, strong) KerJSObject * getLocationObject;

@property (nonatomic, strong) KerJSObject * onCompassChange;

@end

@implementation WXLocation

@synthesize locationManager = _locationManager;
@synthesize getLocationObject = _getLocationObject;
@synthesize onCompassChange = _onCompassChange;


-(CLLocationManager *)locationManager{
    
    if (_locationManager == nil) {
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            self->_locationManager = [[CLLocationManager alloc] init];
            [self->_locationManager requestAlwaysAuthorization];
            self->_locationManager.delegate = self;
            self->_locationManager.desiredAccuracy = kCLLocationAccuracyBest;
            self->_locationManager.distanceFilter = kCLDistanceFilterNone;
        });
        return _locationManager;
    }else {
        return _locationManager;
    }

}

-(void) getLocation:(KerJSObject *) object {
    
    self.getLocationObject = object;
    
    if ([CLLocationManager authorizationStatus] == kCLAuthorizationStatusDenied) {
        //未开启定位
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"getLocation:fail auth deny"];
        id<WXGetLocationObject> v = [self.getLocationObject implementProtocol:@protocol(WXGetLocationObject)];
        
        [v fail:res];
        [v complete:res];

    }else {
        //开始定位
        [self.locationManager startUpdatingLocation];
    }
    
}

-(void) startCompass:(KerJSObject *) object{
    
    [self.locationManager startUpdatingHeading];
    
    WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"startCompass:ok"];
    id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];
    
    [v success:res];
    [v complete:res];
    
}

-(void) stopCompass:(KerJSObject *) object{
    
    [self.locationManager stopUpdatingHeading];
    
    WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"stopCompass:ok"];
    id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];
    
    [v success:res];
    [v complete:res];
    
}

#pragma mark -- CLLocationManager Protocol

-(void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray<CLLocation *> *)locations{
    
    if (self.getLocationObject) {
        //getLocation:fail auth deny
        id<WXGetLocationObject> v = [self.getLocationObject implementProtocol:@protocol(WXGetLocationObject)];
        WXGetLocationRes * res = [[WXGetLocationRes alloc] initWithCLLocation:locations.lastObject type:v.type errMsg:@"getLocation:ok"];
        
        [v success:res];
        [v complete:res];
    }
    
    [self.locationManager stopUpdatingLocation];
}

-(void)locationManager:(CLLocationManager *)manager didChangeAuthorizationStatus:(CLAuthorizationStatus)status{
    
    if (status == kCLAuthorizationStatusDenied && self.getLocationObject) {
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"getLocation:fail auth deny"];
        id<WXGetLocationObject> v = [self.getLocationObject implementProtocol:@protocol(WXGetLocationObject)];
        
        [v fail:res];
        [v complete:res];
        
    }
    
}

-(void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error{
    
    //未知错误
    id<WXGetLocationObject> v = [self.getLocationObject implementProtocol:@protocol(WXGetLocationObject)];
    WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"getLocation:fail"];
    
    [v fail:res];
    [v complete:res];
    
    [self.locationManager stopUpdatingLocation];
}

-(void) locationManager:(CLLocationManager *)manager didUpdateHeading:(CLHeading *)newHeading{
    
    // 罗盘输出回调
    if (self.onCompassChange) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.onCompassChange callWithArguments:@[[[WXOnCompassChageRes alloc] initWithHeading:newHeading]]];
        });
    }
}

@end


@implementation KerWXObject (Location)

-(WXLocation *)WXLocation{
    WXLocation * location = objc_getAssociatedObject(self, WXLocationKey);
    if (location == nil) {
        location = [[WXLocation alloc]init];
        objc_setAssociatedObject(self, WXLocationKey, location, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return location;
}

-(void) getLocation:(KerJSObject *) object {
    [self.WXLocation getLocation:object];
}

-(void) openLocation:(KerJSObject *) object {
    
}

-(void) startCompass:(KerJSObject *) object{
    [self.WXLocation startCompass:object];
}

-(void) stopCompass:(KerJSObject *) object{
    [self.WXLocation stopCompass:object];
}

-(void) onCompassChange:(KerJSObject *) object{
    self.WXLocation.onCompassChange = object;
}


@end

@implementation CLLocation (WXLocation)

#define LAT_OFFSET_0(x,y) -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(fabs(x))
#define LAT_OFFSET_1 (20.0 * sin(6.0 * x * M_PI) + 20.0 * sin(2.0 * x * M_PI)) * 2.0 / 3.0
#define LAT_OFFSET_2 (20.0 * sin(y * M_PI) + 40.0 * sin(y / 3.0 * M_PI)) * 2.0 / 3.0
#define LAT_OFFSET_3 (160.0 * sin(y / 12.0 * M_PI) + 320 * sin(y * M_PI / 30.0)) * 2.0 / 3.0

#define LON_OFFSET_0(x,y) 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(fabs(x))
#define LON_OFFSET_1 (20.0 * sin(6.0 * x * M_PI) + 20.0 * sin(2.0 * x * M_PI)) * 2.0 / 3.0
#define LON_OFFSET_2 (20.0 * sin(x * M_PI) + 40.0 * sin(x / 3.0 * M_PI)) * 2.0 / 3.0
#define LON_OFFSET_3 (150.0 * sin(x / 12.0 * M_PI) + 300.0 * sin(x / 30.0 * M_PI)) * 2.0 / 3.0

#define RANGE_LON_MAX 137.8347
#define RANGE_LON_MIN 72.004
#define RANGE_LAT_MAX 55.8271
#define RANGE_LAT_MIN 0.8293

#define jzA 6378245.0
#define jzEE 0.00669342162296594323

- (BOOL)outOfChina:(double)lat bdLon:(double)lon{
    if (lon < RANGE_LON_MIN || lon > RANGE_LON_MAX)
        return true;
    if (lat < RANGE_LAT_MIN || lat > RANGE_LAT_MAX)
        return true;
    return false;
}
- (double)transformLat:(double)x bdLon:(double)y{
    double ret = LAT_OFFSET_0(x, y);
    ret += LAT_OFFSET_1;
    ret += LAT_OFFSET_2;
    ret += LAT_OFFSET_3;
    return ret;
}

- (double)transformLon:(double)x bdLon:(double)y{
    double ret = LON_OFFSET_0(x, y);
    ret += LON_OFFSET_1;
    ret += LON_OFFSET_2;
    ret += LON_OFFSET_3;
    return ret;
}

- (CLLocationCoordinate2D)gcj02Encrypt:(double)ggLat bdLon:(double)ggLon{
    CLLocationCoordinate2D resPoint;
    double mgLat;
    double mgLon;
    if ([self outOfChina:ggLat bdLon:ggLon]) {
        resPoint.latitude = ggLat;
        resPoint.longitude = ggLon;
        return resPoint;
    }
    double dLat = [self transformLat:(ggLon - 105.0)bdLon:(ggLat - 35.0)];
    double dLon = [self transformLon:(ggLon - 105.0) bdLon:(ggLat - 35.0)];
    double radLat = ggLat / 180.0 * M_PI;
    double magic = sin(radLat);
    magic = 1 - jzEE * magic * magic;
    double sqrtMagic = sqrt(magic);
    dLat = (dLat * 180.0) / ((jzA * (1 - jzEE)) / (magic * sqrtMagic) * M_PI);
    dLon = (dLon * 180.0) / (jzA / sqrtMagic * cos(radLat) * M_PI);
    mgLat = ggLat + dLat;
    mgLon = ggLon + dLon;
    
    resPoint.latitude = mgLat;
    resPoint.longitude = mgLon;
    return resPoint;
}

-(CLLocationCoordinate2D)ker_generateGCJ02Coordinate{
    return [self gcj02Encrypt:self.coordinate.latitude bdLon:self.coordinate.longitude];
}
@end
