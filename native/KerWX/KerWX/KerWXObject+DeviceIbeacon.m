//
//  KerWXObject+DeviceIbeacon.m
//  KerWX
//
//  Created by zuowu on 2018/12/18.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+DeviceIbeacon.h"
#import <CoreLocation/CoreLocation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import <objc/runtime.h>

#define WXIBeaconKey "WXIBeaconKey"

@implementation WXIBeaconRes

-(instancetype) initWithErrMsg:(NSString *)errMsg errCode:(int)errCode{
    if (self = [super init]) {
        self.errMsg = errMsg;
        self.errCode = errCode;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXIBeaconRes] [dealloc]");
}

@end

@implementation WXOnBeaconServiceChangeRes

-(instancetype)initWithAvailable:(NSNumber *)available Discovering:(NSNumber *)discovering{
    if (self = [super init]) {
        self.available = [available boolValue];
        self.discovering = [discovering boolValue];
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXOnBeaconServiceChangeRes] [dealloc]");
}

@end

@implementation WXIBeaconInfo

-(instancetype)initWithBeacon:(CLBeacon *)beacon{
    if (self = [super init]) {
        self.uuid = beacon.proximityUUID.UUIDString;
        self.major = beacon.major;
        self.minor = beacon.minor;
        self.proximity = beacon.proximity;
        self.accuracy = beacon.accuracy;
        self.rssi = beacon.rssi;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXIBeaconInfo] [dealloc]");
}

@end

@implementation WXGetBeaconsRes

-(instancetype)initWithBeacons:(NSArray *)beacons ErrMsg:(NSString *)errMsg ErrCode:(int)errCode{
    if (self = [super init]) {
        self.beacons = beacons;
        self.errMsg = errMsg;
        self.errCode = errCode;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXGetBeaconsRes] [dealloc]");
}

@end

@implementation WXOnBeaconUpdateRes

-(instancetype)initWithBeacons:(NSArray *)beacons{
    if (self == [super init]) {
        self.beacons = beacons;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXOnBeaconUpdateRes] [dealloc]");
}

@end



@interface WXIBeacon : NSObject <CLLocationManagerDelegate, CBPeripheralManagerDelegate>

@property (nonatomic, strong, readonly) CLLocationManager * locationManager;
@property (nonatomic, strong, readonly) CBPeripheralManager * peripheralManager;
@property (nonatomic, strong, readonly) NSMutableArray<CLBeaconRegion *> * beaconRegionArray;
@property (nonatomic, strong, readonly) NSMutableDictionary<NSString *, NSArray<CLBeacon *> *> * beaconArrayDic;

@property (nonatomic, strong) KerJSObject * startBeaconDiscoveryObject;

@property (nonatomic, copy) NSNumber * beaconAvailable;
@property (nonatomic, copy) NSNumber * beaconDiscovering;

@property (nonatomic, strong) KerJSObject * onBeaconServiceChang;
@property (nonatomic, strong) KerJSObject * onBeaconUpdate;

@end

@implementation WXIBeacon

@synthesize locationManager = _locationManager;
@synthesize peripheralManager = _peripheralManager;

@synthesize beaconRegionArray = _beaconRegionArray;
@synthesize beaconArrayDic = _beaconArrayDic;

-(CLLocationManager *)locationManager{
    
    if (_locationManager == nil) {
        _locationManager = [[CLLocationManager alloc] init];
        [_locationManager requestAlwaysAuthorization];
        _locationManager.delegate = self;
        _locationManager.desiredAccuracy = kCLLocationAccuracyBest;
        _locationManager.distanceFilter = kCLDistanceFilterNone;
    }
    return _locationManager;
}

-(NSMutableArray *)beaconRegionArray{
    if (!_beaconRegionArray) {
        _beaconRegionArray = [[NSMutableArray alloc] init];
    }
    return _beaconRegionArray;
}

-(NSMutableDictionary *)beaconArrayDic{
    if (!_beaconArrayDic) {
        _beaconArrayDic = [[NSMutableDictionary alloc] init];
    }
    return _beaconArrayDic;
}


-(void)startRangingBeacons:(KerJSObject *)object{
    
    id<WXStartBeaconDiscoveryObject> v = [object implementProtocol:@protocol(WXStartBeaconDiscoveryObject)];
    
    BOOL location = [CLLocationManager locationServicesEnabled] && ([CLLocationManager authorizationStatus] == kCLAuthorizationStatusAuthorizedAlways || [CLLocationManager authorizationStatus] == kCLAuthorizationStatusAuthorizedWhenInUse);

    if (location) {

        for (int i = 0 ; i < v.uuids.count; i++) {
            NSString * strID = [v.uuids objectAtIndex:i];
            NSUUID * uuid = [[NSUUID alloc] initWithUUIDString:strID];
            NSString * identifier = [NSString stringWithFormat:@"WX_%d",i];
            CLBeaconRegion * region = [[CLBeaconRegion alloc] initWithProximityUUID:uuid identifier:identifier];
            [self.beaconRegionArray addObject:region];
            [self.locationManager startRangingBeaconsInRegion:region];
        }
        WXIBeaconRes * res = [[WXIBeaconRes alloc] initWithErrMsg:@"startBeaconDiscovery:ok" errCode:0];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v success:res];
            [v complete:res];
        });
        
        self.beaconDiscovering = @YES;
        
        dispatch_async(dispatch_get_main_queue(), ^{
            WXOnBeaconServiceChangeRes * changeRes = [[WXOnBeaconServiceChangeRes alloc] initWithAvailable:self.beaconAvailable Discovering:self.beaconDiscovering];
            [self.onBeaconServiceChang callWithArguments:@[changeRes]];
        });

    }else {
        
        WXIBeaconRes * res = [[WXIBeaconRes alloc] initWithErrMsg:@"startBeaconDiscovery:fail fail: location service unavailable" errCode:11002];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });
        
        dispatch_async(dispatch_get_main_queue(), ^{
            WXOnBeaconServiceChangeRes * changeRes = [[WXOnBeaconServiceChangeRes alloc] initWithAvailable:self.beaconAvailable Discovering:self.beaconDiscovering];
            [self.onBeaconServiceChang callWithArguments:@[changeRes]];
        });
        
    }
    
}

-(void) startBeaconDiscovery: (KerJSObject *) object{
    
    //_peripheralManager 用来判断蓝牙是否开启
    if (_peripheralManager == nil) {
        _peripheralManager = [[CBPeripheralManager alloc] initWithDelegate:self queue:nil];
    }
    
    id<WXStartBeaconDiscoveryObject> v = [object implementProtocol:@protocol(WXStartBeaconDiscoveryObject)];
    if (v.ignoreBluetoothAvailable) {
        
        self.startBeaconDiscoveryObject = object;
        
    }else {
        
        [self startRangingBeacons:object];
        
    }
    
}

-(void)stopBeaconDiscovery:(KerJSObject *) object{
    
    id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];
    
    for (CLBeaconRegion * region in self.beaconRegionArray) {
        [self.locationManager stopRangingBeaconsInRegion:region];
    }
    [self.beaconRegionArray removeAllObjects];
    [self.beaconArrayDic removeAllObjects];
    
    WXIBeaconRes * res = [[WXIBeaconRes alloc] initWithErrMsg:@"stopBeaconDiscovery:ok" errCode:0];
    dispatch_async(dispatch_get_main_queue(), ^{
        [v success:res];
        [v complete:res];
    });
    
    self.beaconDiscovering =  @NO;
    dispatch_async(dispatch_get_main_queue(), ^{
        WXOnBeaconServiceChangeRes * res = [[WXOnBeaconServiceChangeRes alloc] initWithAvailable:self.beaconAvailable Discovering:self.beaconDiscovering];
        [self.onBeaconServiceChang callWithArguments:@[res]];
    });

}

-(void)getBeacons:(KerJSObject *) object{
    
    id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];
    WXGetBeaconsRes * res = [[WXGetBeaconsRes alloc] initWithBeacons:[self collectBeacons] ErrMsg:@"getBeacons:ok" ErrCode:0];
    dispatch_async(dispatch_get_main_queue(), ^{
        [v success:res];
        [v complete:res];
    });

}

#pragma mark -- tools

-(NSArray<WXIBeaconInfo *> *)collectBeacons{
    NSMutableArray<WXIBeaconInfo *> * arr = [[NSMutableArray alloc]init];
    if (self.beaconArrayDic) {
        for (NSString * key in self.beaconArrayDic.allKeys) {
            NSArray * beacons = [self.beaconArrayDic objectForKey:key];
            for (CLBeacon * beacon in beacons) {
                WXIBeaconInfo * info = [[WXIBeaconInfo alloc]initWithBeacon:beacon];
                [arr addObject:info];
            }
        }
    }
    return arr;
}

#pragma mark -- CLLocationManagerDelegate

-(void)locationManager:(CLLocationManager *)manager didRangeBeacons:(NSArray<CLBeacon *> *)beacons inRegion:(CLBeaconRegion *)region{
    
    //NSLog(@"didRangeBeacons");
    [self.beaconArrayDic setObject:beacons forKey:region.proximityUUID.UUIDString];
    WXOnBeaconUpdateRes * res = [[WXOnBeaconUpdateRes alloc] initWithBeacons:[self collectBeacons]];
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.onBeaconUpdate callWithArguments:@[res]];
    });
    
}

#pragma mark -- CBPeripheralManagerDelegate

- (void)peripheralManagerDidUpdateState:(nonnull CBPeripheralManager *)peripheral {
    
    if (self.startBeaconDiscoveryObject) {
        
        id<WXStartBeaconDiscoveryObject> v = [self.startBeaconDiscoveryObject implementProtocol:@protocol(WXStartBeaconDiscoveryObject)];
        if (peripheral.state == CBCentralManagerStatePoweredOn) {
            
            [self startRangingBeacons:self.startBeaconDiscoveryObject];
            
        }else{
            
            WXIBeaconRes * res = [[WXIBeaconRes alloc] initWithErrMsg:@"startBeaconDiscovery:fail fail:bluetooth power off" errCode:11000];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });
            
        }
        
        self.startBeaconDiscoveryObject = nil;
    }
    
    self.beaconAvailable = peripheral.state == CBCentralManagerStatePoweredOn ? @YES : @NO;
    
    dispatch_async(dispatch_get_main_queue(), ^{
        WXOnBeaconServiceChangeRes * res = [[WXOnBeaconServiceChangeRes alloc] initWithAvailable:self.beaconAvailable Discovering:self.beaconDiscovering];
        [self.onBeaconServiceChang callWithArguments:@[res]];
    });

}

@end

@implementation KerWXObject (DeviceIbeacon)

-(WXIBeacon *)WXIBeacon{
    WXIBeacon * iBeacon = objc_getAssociatedObject(self, WXIBeaconKey);
    if (iBeacon == nil) {
        iBeacon = [[WXIBeacon alloc]init];
        objc_setAssociatedObject(self, WXIBeaconKey, iBeacon, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return iBeacon;
}

-(void) startBeaconDiscovery: (KerJSObject *) object{
    [self.WXIBeacon startBeaconDiscovery:object];
}

-(void) stopBeaconDiscovery: (KerJSObject *) object{
    [self.WXIBeacon stopBeaconDiscovery:object];
}

-(void)getBeacons:(KerJSObject *) object{
    [self.WXIBeacon getBeacons:object];
}

-(void) onBeaconServiceChange:(KerJSObject *) object{
    self.WXIBeacon.onBeaconServiceChang = object;
}

-(void) onBeaconUpdate:(KerJSObject *) object{
    self.WXIBeacon.onBeaconUpdate = object;
}

@end
