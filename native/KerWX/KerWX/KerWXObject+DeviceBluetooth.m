//
//  KerWXObject+DeviceBluetooth.m
//  KerWX
//
//  Created by zuowu on 2018/12/14.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+DeviceBluetooth.h"
#import <CoreBluetooth/CoreBluetooth.h>
#import <objc/runtime.h>

#define WXBluetoothKey "WXBluetoothKey"

@implementation WXBluetoothRes

-(instancetype)initWithErrMsg:(NSString *)errMsg errCode:(int)errCode{
    if (self = [super init]) {
        self.errMsg = errMsg;
        self.errCode = errCode;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXBluetoothRes] [dealloc]");
}

@end


@implementation WXBOpenBluetoothAdapterRes

-(instancetype)initWithErrMsg:(NSString *)errMsg errCode:(int)errCode state:(int)state{
    if (self = [super init]) {
        self.errMsg = errMsg;
        self.errCode = errCode;
        self.state = state;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXBOpenBluetoothAdapterRes] [dealloc]");
}

@end


@implementation WXGetBluetoothAdapterStateRes

-(instancetype)initWithErrMsg:(NSString *)msg errCode:(int)code discovering:(BOOL) discovering available:(BOOL) available{
    if (self = [super init]) {
        self.errMsg = msg;
        self.errCode = code;
        self.discovering = discovering;
        self.available = available;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXGetBluetoothAdapterStateRes] [dealloc]");
}

@end

@implementation WXOnBluetoothAdapterStateChangeRes

-(instancetype)initWithDiscovering:(BOOL) discovering available:(BOOL) available{
    if (self = [super init]) {
        self.discovering = discovering;
        self.available = available;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXOnBluetoothAdapterStateChangeRes] [dealloc]");
}

@end

@implementation WXStartBluetoothDevicesDiscoveryRes

-(instancetype)initWithErrMsg:(NSString *)errMsg ErrCode:(int)errCode isDiscovering:(BOOL) isDiscovering{
    if (self = [super init]) {
        self.errCode = errCode;
        self.errMsg = errMsg;
        self.isDiscovering = isDiscovering;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXStartBluetoothDevicesDiscoveryRes] [dealloc]");
}

@end

@implementation WXOnBluetoothDeviceFoundPeripheral

@synthesize name = _name;
@synthesize deviceId = _deviceId;
@synthesize RSSI = _RSSI;
@synthesize advertisData = _advertisData;
@synthesize advertisServiceUUIDs = _advertisServiceUUIDs;
@synthesize localName = _localName;
@synthesize serviceData = _serviceData;

-(instancetype) initWithDiscoverPeripheral:(CBPeripheral *) peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI {
    if (self = [super init]) {
        self.name = peripheral.name;
        self.deviceId = peripheral.identifier.UUIDString;
        self.RSSI = RSSI;
        self.advertisData = [advertisementData objectForKey:@"kCBAdvDataManufacturerData"];
        self.advertisServiceUUIDs = [advertisementData objectForKey:@"kCBAdvDataServiceUUIDs"];
        self.localName = [advertisementData objectForKey:@"kCBAdvDataLocalName"];
        self.serviceData = [advertisementData objectForKey:@"kCBAdvDataServiceData"];
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXOnBluetoothDeviceFoundPeripheral] [dealloc]");
}

@end


@implementation WXOnBluetoothDeviceFoundRes

@synthesize devices = _devices;

-(instancetype)initWithDevices:(NSArray<WXOnBluetoothDeviceFoundPeripheral *> *)devices{
    if (self = [super init]) {
        self.devices = devices;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXOnBluetoothDeviceFoundRes] [dealloc]");
}

@end

@implementation WXGetBluetoothDevicesRes

@synthesize devices = _devices;

-(instancetype)initWithErrMsg:(NSString *)errMsg ErrCode:(int)errCode devices:(NSArray<WXOnBluetoothDeviceFoundPeripheral *> *) devices{
    if (self = [super init]) {
        self.errMsg = errMsg;
        self.errCode = errCode;
        self.devices = devices;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXGetBluetoothDevicesRes] [dealloc]");
}

@end

@implementation WXConnectedBluetoothDevices

@synthesize name = _name;
@synthesize deviceId = _deviceId;

-(instancetype)initWithCBPeripheral:(CBPeripheral *)peripheral {
    if (self = [super init]) {
        self.name = peripheral.name;
        self.deviceId = peripheral.identifier.UUIDString;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXConnectedBluetoothDevices] [dealloc]");
}

@end

@implementation WXGetConnectedBluetoothDevicesRes

-(instancetype)initWithDevices:(NSArray *)devices errMsg:(NSString *)errMsg errCode:(int)errCode{
    if (self = [super init]) {
        self.devices = devices;
        self.errMsg = errMsg;
        self.errCode = errCode;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXGetConnectedBluetoothDevicesRes] [dealloc]");
}

@end


@interface WXBluetooth : NSObject <CBCentralManagerDelegate>

@property (nonatomic, strong) CBCentralManager * centralManager;
@property (nonatomic, strong) KerJSObject * openBluetoothAdapterObject;
@property (nonatomic, strong) KerJSObject * sartBluetoothDevicesDiscoveryObject;

@property (nonatomic, strong) KerJSObject * onBluetoothAdapterStateChange;
@property (nonatomic, strong) KerJSObject * onBluetoothDeviceFound;

@property (nonatomic, strong) NSNumber * starScanTime;
@property (nonatomic, strong, readonly) NSMutableArray<WXOnBluetoothDeviceFoundPeripheral *> * peripherArray;
@property (nonatomic, strong, readonly) NSMutableArray<WXOnBluetoothDeviceFoundPeripheral *> * discoveredPeripherArray;

@end

@implementation WXBluetooth

@synthesize peripherArray = _peripherArray;
@synthesize discoveredPeripherArray = _discoveredPeripherArray;

-(NSMutableArray<WXOnBluetoothDeviceFoundPeripheral *> *)peripherArray{
    if (!_peripherArray) {
        _peripherArray = [[NSMutableArray alloc] init];
    }
    return _peripherArray;
}

-(NSMutableArray<WXOnBluetoothDeviceFoundPeripheral *> *)discoveredPeripherArray{
    if (!_discoveredPeripherArray) {
        _discoveredPeripherArray = [[NSMutableArray alloc] init];
    }
    return _discoveredPeripherArray;
}

-(void)getBluetoothAdapterState:(KerJSObject *) object{
    
    id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];

    if (self.centralManager == nil) {

        WXGetBluetoothAdapterStateRes * res = [[WXGetBluetoothAdapterStateRes alloc] initWithErrMsg: @"getBluetoothAdapterState:fail ble adapter need open first. need open bluetooth adapter" errCode:10000 discovering:NO available:NO];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });

    }else {

        if (self.centralManager.state == CBCentralManagerStatePoweredOn) {

            WXGetBluetoothAdapterStateRes * res = [[WXGetBluetoothAdapterStateRes alloc] initWithErrMsg: @"getBluetoothAdapterState:ok" errCode:0 discovering:self.centralManager.isScanning available:YES];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v success:res];
                [v complete:res];
            });

        }else {

            WXGetBluetoothAdapterStateRes * res = [[WXGetBluetoothAdapterStateRes alloc] initWithErrMsg: @"getBluetoothAdapterState:fail ble adapter need open first. bluetooth state err" errCode:10008 discovering:NO available:NO];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });

        }
    }
}

-(void)openBluetoothAdapter:(KerJSObject *) object {
    
    self.openBluetoothAdapterObject = object;
    id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];
    
    if (self.centralManager) {
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"openBluetoothAdapter:fail bluetooth already started" errCode:10008];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });
    }else {
        self.centralManager = [[CBCentralManager alloc]initWithDelegate:self queue:nil];
    }
    
}

-(void)closeBluetoothAdapter:(KerJSObject *) object{
    
    self.centralManager = nil;
    self.openBluetoothAdapterObject = nil;
    [self.peripherArray removeAllObjects];
    WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"closeBluetoothAdapter:fail" errCode:0];
    if (object) {
        id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v success:res];
            [v complete:res];
        });
    }

}

-(void)startBluetoothDevicesDiscovery:(KerJSObject *) object{
    
    id<WXStartBluetoothDevicesDiscoveryObject> v = [object implementProtocol:@protocol(WXStartBluetoothDevicesDiscoveryObject)];

    if (self.centralManager == nil) {

        WXStartBluetoothDevicesDiscoveryRes * res = [[WXStartBluetoothDevicesDiscoveryRes alloc] initWithErrMsg:@"startBluetoothDevicesDiscovery:fail need openBluetoothAdapter" ErrCode:10000 isDiscovering:NO];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });

    }else {

        if (self.centralManager.state == CBCentralManagerStatePoweredOn) {

            self.sartBluetoothDevicesDiscoveryObject = object;
            self.starScanTime = [NSNumber numberWithInteger:([[NSDate date] timeIntervalSince1970] * 1000)];
            NSMutableArray * services = nil;
            if (v.services != nil) {
                services = [[NSMutableArray alloc] init];
                for (int i = 0 ; i < v.services.count; i++) {
                    [services addObject:[CBUUID UUIDWithString:[v.services objectAtIndex:i]]];
                }
            }
            NSMutableDictionary * options = [[NSMutableDictionary alloc] init];
            [options setObject:[NSNumber numberWithBool:v.allowDuplicatesKey] forKey:CBCentralManagerScanOptionAllowDuplicatesKey];
            [self.centralManager scanForPeripheralsWithServices:services options:options];

            WXStartBluetoothDevicesDiscoveryRes * res = [[WXStartBluetoothDevicesDiscoveryRes alloc] initWithErrMsg:@"startBluetoothDevicesDiscovery:ok" ErrCode:0 isDiscovering:YES];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v success:res];
                [v complete:res];
            });

        }else {

            WXStartBluetoothDevicesDiscoveryRes * res = [[WXStartBluetoothDevicesDiscoveryRes alloc] initWithErrMsg:[NSString stringWithFormat:@"startBluetoothDevicesDiscovery:fail openBluetoothAdapter state = %ld", (long)self.centralManager.state] ErrCode:10000 isDiscovering:NO];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });

        }
    }

}

-(void)stopBluetoothDevicesDiscovery:(KerJSObject *) object {

    id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];
    
    if (self.centralManager == nil || self.centralManager.state != CBCentralManagerStatePoweredOn) {

        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"stopBluetoothDevicesDiscovery:fail ble adapter hans't been opened or ble is unavailable." errCode:10000];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });

    }else {

        [self.centralManager stopScan];
        self.sartBluetoothDevicesDiscoveryObject = nil;
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"stopBluetoothDevicesDiscovery:ok" errCode:0];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });

    }

}

-(void)getBluetoothDevices:(KerJSObject *) object{
    
    id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];

    if (self.centralManager == nil) {

        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"getBluetoothDevices:fail ble adapter hans't been opened or ble is unavailable." errCode:10000];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });

    }else {

        WXGetBluetoothDevicesRes * res = [[WXGetBluetoothDevicesRes alloc] initWithErrMsg:@"getBluetoothDevices:ok" ErrCode:0 devices:self.peripherArray];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v success:res];
            [v complete:res];
        });

    }

}

-(void)getConnectedBluetoothDevices:(KerJSObject *) object{
    
    id<WXGetConnectedBluetoothDevicesObject> v = [object implementProtocol:@protocol(WXGetConnectedBluetoothDevicesObject)];
    
    if (self.centralManager == nil) {
        
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"getConnectedBluetoothDevices:fail ble adapter hans't been opened or ble is unavailable." errCode:10000];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });

    }else {

        NSMutableArray<CBUUID *> * arr = [[NSMutableArray alloc]init];
        for (NSString * identifier in v.services) {
            [arr addObject:[CBUUID UUIDWithString:identifier]];
        }
        NSArray<CBPeripheral *> * peripherals = [self.centralManager retrieveConnectedPeripheralsWithServices:arr];
        NSMutableArray<WXConnectedBluetoothDevices *> * devices = [[NSMutableArray alloc]init];
        for (CBPeripheral * peripheral in peripherals) {
            [devices addObject:[[WXConnectedBluetoothDevices alloc] initWithCBPeripheral:peripheral]];
        }
        
        WXGetConnectedBluetoothDevicesRes * res = [[WXGetConnectedBluetoothDevicesRes alloc] initWithDevices:devices errMsg:@"getConnectedBluetoothDevices:ok" errCode:0];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v success:res];
            [v complete:res];
        });

    }
}

#pragma mark -- CBCentralManagerDelegate

- (void)centralManagerDidUpdateState:(CBCentralManager *)central{
    
    BOOL available = NO;
    BOOL discovering = NO;
    if (central) {
        discovering = central.isScanning;
        available = central.state == CBCentralManagerStatePoweredOn ? YES: NO;
    }
    
    if (self.onBluetoothAdapterStateChange) {
        WXOnBluetoothAdapterStateChangeRes * res = [[WXOnBluetoothAdapterStateChangeRes alloc] initWithDiscovering:available available:discovering];
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.onBluetoothAdapterStateChange callWithArguments:@[res]];
        });
    }
    
    
    if (self.openBluetoothAdapterObject) {
        id<WXCallbackFunction> v = [self.openBluetoothAdapterObject implementProtocol:@protocol(WXCallbackFunction)];
        if (central && central.state == CBCentralManagerStatePoweredOn) {
            //成功
            WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"openBluetoothAdapter:ok" errCode:0];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v success:res];
                [v complete:res];
            });
        }else {
            //失败
            WXBOpenBluetoothAdapterRes * res = [[WXBOpenBluetoothAdapterRes alloc] initWithErrMsg:@"openBluetoothAdapter:fail" errCode:1001 state:central.state];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });
        }
    }

}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI {
    
    WXOnBluetoothDeviceFoundPeripheral * p = [[WXOnBluetoothDeviceFoundPeripheral alloc] initWithDiscoverPeripheral:peripheral advertisementData:advertisementData RSSI:RSSI];

    BOOL saved = [self.peripherArray ker_alreadySaveObject:p method:^BOOL(id objA, id objB) {
        WXOnBluetoothDeviceFoundPeripheral * pa = (WXOnBluetoothDeviceFoundPeripheral *)objA;
        WXOnBluetoothDeviceFoundPeripheral * pb = (WXOnBluetoothDeviceFoundPeripheral *)objB;
        if ([pa.deviceId isEqualToString:pb.deviceId]) {
            return YES;
        }
        return NO;
    }];
    if (!saved) {
        [self.peripherArray addObject:p];
    }

    if (self.sartBluetoothDevicesDiscoveryObject) {
        
        id<WXStartBluetoothDevicesDiscoveryObject> v = [self.sartBluetoothDevicesDiscoveryObject implementProtocol:@protocol(WXStartBluetoothDevicesDiscoveryObject)];

        if (v.interval == 0) {
            
            [self.discoveredPeripherArray addObject:p];
            [self.onBluetoothDeviceFound callWithArguments:@[[[WXOnBluetoothDeviceFoundRes alloc] initWithDevices:self.discoveredPeripherArray]]];
            [self.discoveredPeripherArray removeAllObjects];


        }else {

            [self.discoveredPeripherArray addObject:p];
            long nowTime = [[NSDate date] timeIntervalSince1970] * 1000;
            if (nowTime - [self.starScanTime longLongValue] > v.interval) {
                [self.onBluetoothDeviceFound callWithArguments:@[[[WXOnBluetoothDeviceFoundRes alloc] initWithDevices:self.discoveredPeripherArray]]];
                [self.discoveredPeripherArray removeAllObjects];
                self.starScanTime = [NSNumber numberWithLong:nowTime];
            }
        }
    }
}

@end


@implementation KerWXObject (DeviceBluetooth)

-(WXBluetooth *)WXBluetooth{
    WXBluetooth * bluetooth = objc_getAssociatedObject(self, WXBluetoothKey);
    if (bluetooth == nil) {
        bluetooth = [[WXBluetooth alloc]init];
        objc_setAssociatedObject(self, WXBluetoothKey, bluetooth, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return bluetooth;
}

-(void) openBluetoothAdapter:(KerJSObject *) object{
    [self.WXBluetooth openBluetoothAdapter:object];
}

-(void) onBluetoothAdapterStateChange:(KerJSObject *) object{
    self.WXBluetooth.onBluetoothAdapterStateChange = object;
}

-(void)closeBluetoothAdapter:(KerJSObject *) object{
    [self.WXBluetooth closeBluetoothAdapter:object];
}

-(void) getBluetoothAdapterState:(KerJSObject *) objec{
    [self.WXBluetooth getBluetoothAdapterState:objec];
}

-(void)startBluetoothDevicesDiscovery:(KerJSObject *) object{
    [self.WXBluetooth startBluetoothDevicesDiscovery:object];
}

-(void)stopBluetoothDevicesDiscovery:(KerJSObject *) object{
    [self.WXBluetooth stopBluetoothDevicesDiscovery:object];
}

-(void) onBluetoothDeviceFound:(KerJSObject *) object{
    self.WXBluetooth.onBluetoothDeviceFound = object;
}

-(void)getBluetoothDevices:(KerJSObject *) object{
    [self.WXBluetooth getBluetoothDevices:object];
}

-(void)getConnectedBluetoothDevices:(KerJSObject *) object{
    [self.WXBluetooth getConnectedBluetoothDevices:object];
}


@end


@implementation NSMutableArray (WXBOpenBluetooth)

-(BOOL)ker_alreadySaveObject:(id)object method:(ArrayCompareFunc)func{
    for (id a in self) {
        if (func(object, a)) {
            return YES;
        }
    }
    return NO;
}

@end
