//
//  KerWXObject+DeviceBluetooth.m
//  KerWX
//
//  Created by zuowu on 2018/12/14.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+DeviceBluetooth.h"
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
         /*
         * @"kCBAdvDataServiceData" 数据段中的内容 key 可能为 CBUUID 类型 而不是 NSString 会导致转换js对象崩溃
         * ker_mutableDeepCopy 方法复制该数据 会检测 key的类型 如果为 CBUUID 会转换成NSString类型
         * 但是 如果是除了CBUUID 和 NSString 其他的类型会不拷贝相对应的 value, 防止崩溃
         */
        self.serviceData = [[advertisementData objectForKey:@"kCBAdvDataServiceData"] ker_mutableDeepCopy];

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

@implementation WXOnBLEConnectionStateChangeRes

-(instancetype)initWithPeripheral:(CBPeripheral *)peripheral Connected:(BOOL)connected ErrMsg:(NSString *)errMsg ErrCode:(int)errCode{
    if (self = [super init]) {
        self.deviceId = peripheral.identifier.UUIDString;
        self.connected = connected;
        self.errMsg = errMsg;
        self.errCode = errCode;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXOnBLEConnectionStateChangeRes] [dealloc]");
}

@end

@implementation WXBLEDeviceService

-(instancetype)initWithCBService:(CBService *)service{
    if (self = [super init]) {
        self.uuid = service.UUID.UUIDString;
        self.isPrimary = service.isPrimary;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXBLEDeviceService] [dealloc]");
}

@end

@implementation WXGetBLEDeviceServicesRes

-(instancetype)initWithErrMsg:(NSString *)errMsg errCode:(int)errCode peripheral:(CBPeripheral *)peripheral{
    if (self = [super init]) {
        self.errMsg = errMsg;
        self.errCode = errCode;
        NSMutableArray * array = [[NSMutableArray alloc] init];
        if (peripheral) {
            
            self.deviceId = peripheral.identifier.UUIDString;
            for (CBService * service in peripheral.services) {
                [array addObject:[[WXBLEDeviceService alloc] initWithCBService:service]];
                self.services = array;
            }
        }
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXGetBLEDeviceServicesRes] [dealloc]");
}

@end

@implementation WXBLEDeviceCharacteristicPropertie

-(instancetype)initWithProperties:(CBCharacteristicProperties)properties{
    if (self = [super init]) {
        self.read = properties & CBCharacteristicPropertyRead ? YES : NO;
        self.write = properties & CBCharacteristicPropertyWrite ? YES : NO;
        self.notify = properties & CBCharacteristicPropertyNotify ? YES : NO;
        self.indicate = properties & CBCharacteristicPropertyIndicate ? YES : NO;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXBLEDeviceCharacteristicPropertie] [dealloc]");
}

@end

@implementation WXBLEDeviceCharacteristic

-(instancetype)initWithCharacteristic:(CBCharacteristic *)characteristic{
    if (self = [super init]) {
        self.uuid = characteristic.UUID.UUIDString;
        self.properties = [[WXBLEDeviceCharacteristicPropertie alloc] initWithProperties:characteristic.properties];
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXBLEDeviceCharacteristic] [dealloc]");
}

@end

@implementation WXGetBLEDeviceCharacteristicsRes

-(instancetype)initWithService:(CBService *)service ErrMsg:(NSString *)errMsg ErrCode:(int) errCode{
    if (self = [super init]) {
        if (service == nil) {
            self.characteristics = @[];
        }else {
            NSMutableArray * arr = [[NSMutableArray alloc] init];
            for (CBCharacteristic * characteristic in service.characteristics) {
                [arr addObject:[[WXBLEDeviceCharacteristic alloc] initWithCharacteristic:characteristic]];
            }
            self.characteristics = [arr copy];
        }
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXGetBLEDeviceCharacteristicsRes] [dealloc]");
}

@end

@implementation WXOnBLECharacteristicValueChangeRes

-(instancetype)initWithPeripheral:(CBPeripheral *)peripheral Characteristic:(CBCharacteristic *)characteristic{
    if (self = [super init]) {
        self.deviceId = peripheral.identifier.UUIDString;
        self.serviceId = characteristic.service.UUID.UUIDString;
        self.characteristicId = characteristic.UUID.UUIDString;
        self.value = characteristic.value;
    }
    return self;
}

-(void) dealloc {
    NSLog(@"[WXOnBLECharacteristicValueChangeRes] [dealloc]");
}

@end





@interface WXBluetooth : NSObject <CBCentralManagerDelegate, CBPeripheralDelegate>

@property (nonatomic, strong) CBCentralManager * centralManager;
@property (nonatomic, strong) KerJSObject * openBluetoothAdapterObject;
@property (nonatomic, strong) KerJSObject * sartBluetoothDevicesDiscoveryObject;

@property (nonatomic, strong) KerJSObject * onBluetoothAdapterStateChange;
@property (nonatomic, strong) KerJSObject * onBluetoothDeviceFound;

@property (nonatomic, strong) NSNumber * starScanTime;
@property (nonatomic, strong, readonly) NSMutableArray<WXOnBluetoothDeviceFoundPeripheral *> * peripherArray;
@property (nonatomic, strong, readonly) NSMutableArray<WXOnBluetoothDeviceFoundPeripheral *> * discoveredPeripherArray;


@property (nonatomic, strong, readonly) NSMutableArray<CBPeripheral *> * connectedPeripherals;

@property (nonatomic, strong) KerJSObject * createBLEConnectionObject;
@property (nonatomic, strong) KerJSObject * closeBLEConnectionObject;
@property (nonatomic, strong) KerJSObject * getBLEDeviceServicesObject;
@property (nonatomic, strong) KerJSObject * getBLEDeviceCharacteristicsObject;
@property (nonatomic, strong) KerJSObject * notifyBLECharacteristicValueChangeObject;
@property (nonatomic, strong) KerJSObject * readBLECharacteristicValueObject;
@property (nonatomic, strong) KerJSObject * writeBLECharacteristicValueObject;

@property (nonatomic, strong) KerJSObject * onBLEConnectionStateChange;
@property (nonatomic, strong) KerJSObject * onBLECharacteristicValueChange;

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
    WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"closeBluetoothAdapter:ok" errCode:0];
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


-(void) createBLEConnection:(KerJSObject *) object{
    
    id<WXCreateBLEConnectionObject> v = [object implementProtocol:@protocol(WXCreateBLEConnectionObject)];
    
    if (self.centralManager == nil) {
        
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"createBLEConnection:fail createBLEConnection error 10000." errCode:10000];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });

    }else{
        
        CBPeripheral * peripheral = [self findPeripheral:v.deviceId];

        if (peripheral) {

            BOOL saved = [self.connectedPeripherals ker_alreadySaveObject:peripheral method:^BOOL(id objA, id objB) {return objA == objB ? YES : NO;}];

            if (!saved) {
                [self.connectedPeripherals addObject:peripheral];
            }

            self.createBLEConnectionObject = object;
            //NSLog(@"connect peripheral id = %@",peripheral.identifier);
            [self.centralManager connectPeripheral:peripheral options:nil];

            if (v.timeout && v.timeout > 0) {
                //设定超时
                [NSTimer scheduledTimerWithTimeInterval:v.timeout/1000 target:self selector:@selector(onConnectTimeOutTimer:) userInfo:peripheral repeats:NO];
            }
        
        }else {
            
            WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"createBLEConnection:fail createBLEConnection error 10002 device not found" errCode:10002];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });
            
        }
    }
}

-(void) closeBLEConnection:(KerJSObject *) object{
    
    id<WXCloseBLEConnectionObject> v = [object implementProtocol:@protocol(WXCloseBLEConnectionObject)];
    
    if (self.centralManager == nil) {
        
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"closeBLEConnection:fail closeBLEConnection error 10000" errCode:10000];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });
        
    }else{
        
        CBPeripheral * peripheral = [self findConnectedPeripheral:v.deviceId];
        if (peripheral) {
            
            self.closeBLEConnectionObject = object;
            [self.centralManager cancelPeripheralConnection:peripheral];
            
        }else{
            
            WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"closeBLEConnection:fail closeBLEConnection error 10002 device not find" errCode:10002];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });
            
        }
    }
    
}

-(void) getBLEDeviceServices:(KerJSObject *) object{
    
    id<WXGetBLEDeviceServicesObject> v = [object implementProtocol:@protocol(WXGetBLEDeviceServicesObject)];
    
    if (self.centralManager == nil) {
        
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"getBLEDeviceServices:fail closeBLEConnection error 10000" errCode:10000];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });
        
    }else {
        
        CBPeripheral * peripheral = [self findConnectedPeripheral:v.deviceId];
        if (peripheral) {
            self.getBLEDeviceServicesObject = object;
            //扫描服务
            [peripheral discoverServices:nil];
        }else{
            //没找到设备
            WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"getBLEDeviceServices:fail getBLEDeviceServices error 10002" errCode:10002];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });

        }
    }
}

-(void) getBLEDeviceCharacteristics:(KerJSObject *) object{
    
    id<WXBGetLEDeviceCharacteristicsObject> v = [object implementProtocol:@protocol(WXBGetLEDeviceCharacteristicsObject)];
    if (self.centralManager == nil) {
        //未初始化
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"getBLEDeviceCharacteristics:fail getBLEDeviceCharacteristics error 10000" errCode:10000];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });
        
    }else{
        
        CBPeripheral * peripheral = [self findConnectedPeripheral:v.deviceId];
        if (peripheral) {
            
            CBService * service = [peripheral ker_findServiceByID:v.serviceId];
            if (service) {
                //获取特征值
                self.getBLEDeviceCharacteristicsObject = object;
                [peripheral discoverCharacteristics:nil forService:service];
            }else{
                //服务未找到
                WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"getBLEDeviceCharacteristics:fail getBLEDeviceCharacteristics error 10004 need find services" errCode:10004];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [v fail:res];
                    [v complete:res];
                });
            }
            
        }else{
            
            //设备未找到
            WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"getBLEDeviceCharacteristics:fail getBLEDeviceCharacteristics error 10002 need connect device" errCode:10002];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });
            
        }
    }
}

-(void) notifyBLECharacteristicValueChange:(KerJSObject *) object{
    
    id<WXNotifyBLECharacteristicValueChangeObject> v = [object implementProtocol:@protocol(WXNotifyBLECharacteristicValueChangeObject)];
    if (self.centralManager == nil) {
        //未初始化
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"notifyBLECharacteristicValueChange:fail setNotifyOnCharacteristics error 10000" errCode:10000];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });
        
    }else {
        
        CBPeripheral * peripheral = [self findConnectedPeripheral:v.deviceId];
        if (peripheral) {
            CBService * service = [peripheral ker_findServiceByID:v.serviceId];
            if (service) {
                CBCharacteristic * characteristic = [service ker_findCharacteristicByID:v.characteristicId];
                if (characteristic) {
                    //开始设定特征值notify
                    self.notifyBLECharacteristicValueChangeObject = object;
                    [peripheral setNotifyValue:v.state forCharacteristic:characteristic];
                }else{
                    //特征值未找到
                    WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"notifyBLECharacteristicValueChange:fail setNotifyOnCharacteristics error 10005 charcteristics not found" errCode:10005];
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [v fail:res];
                        [v complete:res];
                    });
                }
            }else {
                //服务未找到
                WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"notifyBLECharacteristicValueChange:fail setNotifyOnCharacteristics error 10004 service not found" errCode:10004];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [v fail:res];
                    [v complete:res];
                });
            }
        }else{
            //设备未找到
            WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"notifyBLECharacteristicValueChange:fail setNotifyOnCharacteristics error 10002 device not found" errCode:10002];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });
        }
    }
}

-(void) readBLECharacteristicValue:(KerJSObject *) object{
    
    id<WXReadBLECharacteristicValueObject> v = [object implementProtocol:@protocol(WXReadBLECharacteristicValueObject)];
    if (self.centralManager == nil) {
        //未初始化
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"readBLECharacteristicValue:fail errCode 10000" errCode:10000];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });
        
    }else {
        
        CBPeripheral * peripheral = [self findConnectedPeripheral:v.deviceId];
        if (peripheral) {
            CBService * service = [peripheral ker_findServiceByID:v.serviceId];
            if (service) {
                
                CBCharacteristic * characteristic = [service ker_findCharacteristicByID:v.characteristicId];
                if (characteristic) {
                    //开始读
                    self.readBLECharacteristicValueObject = object;
                    [peripheral readValueForCharacteristic:characteristic];
                }else {
                    //特征值未找到
                    WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"readBLECharacteristicValue:fail errCode 10005" errCode:10005];
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [v fail:res];
                        [v complete:res];
                    });
                }
            }else {
                //服务未找到
                WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"readBLECharacteristicValue:fail errCode 10004" errCode:10004];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [v fail:res];
                    [v complete:res];
                });
            }
            
        }else {
            
            //设备未找到
            WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"readBLECharacteristicValue:fail errCode 10002" errCode:10002];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });
            
        }
    }
    
}

-(void) writeBLECharacteristicValue:(KerJSObject *) object{
    
    id<WXWriteBLECharacteristicValueObject> v = [object implementProtocol:@protocol(WXWriteBLECharacteristicValueObject)];
    if (self.centralManager == nil) {
        
        //未初始化
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"writeBLECharacteristicValue:fail errCode 10000" errCode:10000];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });

    }else {
        
        CBPeripheral * peripheral = [self findConnectedPeripheral:v.deviceId];
        if (peripheral) {
            CBService * service = [peripheral ker_findServiceByID:v.serviceId];
            if (service) {
                
                CBCharacteristic * characteristic = [service ker_findCharacteristicByID:v.characteristicId];
                if (characteristic) {
                    //开始写
                    self.writeBLECharacteristicValueObject = object;
                    [peripheral writeValue:v.value forCharacteristic:characteristic type:CBCharacteristicWriteWithResponse];
                }else {
                    //特征值未找到
                    WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"writeBLECharacteristicValue:fail errCode 10005" errCode:10005];
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [v fail:res];
                        [v complete:res];
                    });
                }
                
            }else {
                
                //服务未找到
                WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"writeBLECharacteristicValue:fail errCode 10004" errCode:10004];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [v fail:res];
                    [v complete:res];
                });
            }
            
        }else {
            
            //设备未找到
            WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"writeBLECharacteristicValue:fail errCode 10002" errCode:10002];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });
            
        }
    }
    
}



#pragma mark -- tools

-(CBPeripheral *) findPeripheral:(NSString *)uuid{
    
    NSArray * arr = [self.centralManager retrievePeripheralsWithIdentifiers:@[[[NSUUID alloc]initWithUUIDString:uuid]]];
    if (arr && arr.count > 0) {
        return arr[0];
    }else{
        return nil;
    }
    
}

-(void)onConnectTimeOutTimer:(NSTimer *)timer{
    
    CBPeripheral * peripheral = timer.userInfo;
    if (peripheral.state != CBPeripheralStateConnected && self.createBLEConnectionObject) {
        
        id<WXCreateBLEConnectionObject> v = [self.createBLEConnectionObject implementProtocol:@protocol(WXCreateBLEConnectionObject)];
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"createBLEConnection:fail connect time out." errCode:10003];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
            self.createBLEConnectionObject = nil;
        });
        
    }
}

-(CBPeripheral *)findConnectedPeripheral:(NSString *)uuid{
    
    CBPeripheral * peripheral = [self  findPeripheral:uuid];
    if (peripheral) {
        BOOL saved = [self.connectedPeripherals ker_alreadySaveObject:peripheral method:^BOOL(id objA, id objB) { return objA == objB ? YES : NO; }];
        if (saved) {
            return peripheral;
        }else {
            return nil;
        }
    }else{
        return nil;
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
            
            dispatch_async(dispatch_get_main_queue(), ^{
                WXOnBluetoothDeviceFoundRes * res = [[WXOnBluetoothDeviceFoundRes alloc] initWithDevices:@[p]];
                [self.onBluetoothDeviceFound callWithArguments:@[res]];
            });
            
        }else {

            [self.discoveredPeripherArray addObject:p];
            long nowTime = [[NSDate date] timeIntervalSince1970] * 1000;
            if (nowTime - [self.starScanTime longLongValue] > v.interval) {
                
                dispatch_async(dispatch_get_main_queue(), ^{
                    [self.onBluetoothDeviceFound callWithArguments:@[[[WXOnBluetoothDeviceFoundRes alloc] initWithDevices:self.discoveredPeripherArray]]];
                    [self.discoveredPeripherArray removeAllObjects];
                    self.starScanTime = [NSNumber numberWithLong:nowTime];
                });
                
            }
        }
    }
    
}

-(void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral{
    
    //NSLog(@"连接成功 %@", peripheral);
    peripheral.delegate = self;
    if (self.createBLEConnectionObject) {
        
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"createBLEConnection:ok" errCode:0];
        id<WXCreateBLEConnectionObject> v = [self.createBLEConnectionObject implementProtocol:@protocol(WXCreateBLEConnectionObject)];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v success:res];
            [v complete:res];
            self.createBLEConnectionObject = nil;
        });
        
    }
    
    if (self.onBLEConnectionStateChange) {
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.onBLEConnectionStateChange callWithArguments:@[[[WXOnBLEConnectionStateChangeRes alloc]initWithPeripheral:peripheral Connected:YES ErrMsg:@"connect success" ErrCode:0]]];
        });
        
    }
    
}

-(void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
    
    //NSLog(@"断开连接 %@", peripheral);
    
    if (self.closeBLEConnectionObject) {
        
        //主动断开连接
        id<WXCloseBLEConnectionObject> v = [self.closeBLEConnectionObject implementProtocol:@protocol(WXCloseBLEConnectionObject)];
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"closeBLEConnectionObject:ok" errCode:0];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v success:res];
            [v complete:res];
            self.closeBLEConnectionObject = nil;
        });

        //主动断开
        if (self.onBLEConnectionStateChange) {
            dispatch_async(dispatch_get_main_queue(), ^{
            [self.onBLEConnectionStateChange callWithArguments:@[[[WXOnBLEConnectionStateChangeRes alloc]initWithPeripheral:peripheral Connected:NO ErrMsg:@"disconnect success" ErrCode:0]]];
            });
        }
        
    }else {
        
        //被动断开
        if (self.onBLEConnectionStateChange) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [self.onBLEConnectionStateChange callWithArguments:@[[[WXOnBLEConnectionStateChangeRes alloc]initWithPeripheral:peripheral Connected:NO ErrMsg:@"disconnect 10006" ErrCode:10006]]];
            });
        }
        
    }
}

-(void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
    
    //NSLog(@"连接失败 %@", peripheral);
    if (self.createBLEConnectionObject) {
        
        id<WXCreateBLEConnectionObject> v = [self.createBLEConnectionObject implementProtocol:@protocol(WXCreateBLEConnectionObject)];
        WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"createBLEConnection:fail errCode 10003 连接失败" errCode:10003];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v fail:res];
            [v complete:res];
        });

    }

    if (self.onBLEConnectionStateChange) {
        [self.onBLEConnectionStateChange callWithArguments:@[[[WXOnBLEConnectionStateChangeRes alloc]initWithPeripheral:peripheral Connected:NO ErrMsg:@"connect fail 10003" ErrCode:10003]]];
    }
}

#pragma mark -- CBPeripheralDelegate

-(void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error{
    
    //NSLog(@"peripheral:didDiscoverServices %@", peripheral.services);
    if (self.getBLEDeviceServicesObject) {
        WXGetBLEDeviceServicesRes * res = [[WXGetBLEDeviceServicesRes alloc] initWithErrMsg:@"getBLEDeviceServices:ok" errCode:0 peripheral:peripheral];
        id<WXGetBLEDeviceServicesObject> v = [self.getBLEDeviceServicesObject implementProtocol:@protocol(WXGetBLEDeviceServicesObject)];
        dispatch_async(dispatch_get_main_queue(), ^{
            [v success:res];
            [v complete:res];
            self.getBLEDeviceServicesObject = nil;
        });
    }
}

-(void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error{
    
    //NSLog(@"peripheral = %@", peripheral);
    //NSLog(@"characteristics = %@",service.characteristics);
    if (self.getBLEDeviceCharacteristicsObject) {
        id<WXGetBLEDeviceServicesObject> v = [self.getBLEDeviceServicesObject implementProtocol:@protocol(WXGetBLEDeviceServicesObject)];
        if (error) {
            WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"getBLEDeviceCharacteristics:fail getBLEDeviceCharacteristics error 10008" errCode:10008];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
                self.getBLEDeviceCharacteristicsObject = nil;
            });
        }else{
            WXGetBLEDeviceCharacteristicsRes * res = [[WXGetBLEDeviceCharacteristicsRes alloc] initWithService:service ErrMsg:@"getBLEDeviceCharacteristics:ok" ErrCode:0];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v success:res];
                [v complete:res];
                self.getBLEDeviceCharacteristicsObject = nil;
            });
        }
    }
    
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error{
    
    //NSLog(@"error = %@", error);
    if (self.notifyBLECharacteristicValueChangeObject) {
        id<WXNotifyBLECharacteristicValueChangeObject> v = [self.notifyBLECharacteristicValueChangeObject implementProtocol:@protocol(WXNotifyBLECharacteristicValueChangeObject)];
        if (error) {
            if (error.code == 6) {
                //特征不支持 notify
                WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"notifyBLECharacteristicValueChange:fail setNotifyOnCharacteristics error 10007" errCode:10007];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [v fail:res];
                    [v complete:res];
                });
            }else {
                //其他错误
                WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"notifyBLECharacteristicValueChange:fail setNotifyOnCharacteristics error 10008" errCode:10008];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [v fail:res];
                    [v complete:res];
                });
            }
        }else {
            //成功
            WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"notifyBLECharacteristicValueChange:ok" errCode:0];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v success:res];
                [v complete:res];
            });
        }
        self.notifyBLECharacteristicValueChangeObject = nil;
    }
    
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error{
    
    //读反馈
    if (self.readBLECharacteristicValueObject) {
        id<WXReadBLECharacteristicValueObject> v = [self.readBLECharacteristicValueObject implementProtocol:@protocol(WXReadBLECharacteristicValueObject)];
        
        if (error == nil) {
            //成功
            WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"readBLECharacteristicValue:ok" errCode:0];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v success:res];
                [v complete:res];
            });

        }else {
            
            if (error.code == 2) {
                //特征不支持读取操作
                WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"readBLECharacteristicValue:fail characteristic not support read errCode 10007" errCode:10007];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [v fail:res];
                    [v complete:res];
                });
            }else {
                //其他错误
                WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"readBLECharacteristicValue:fail errCode 10008" errCode:10008];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [v fail:res];
                    [v complete:res];
                });
            }
        }
        
        self.readBLECharacteristicValueObject = nil;
        
    }

    if (self.onBLECharacteristicValueChange) {
        WXOnBLECharacteristicValueChangeRes * res = [[WXOnBLECharacteristicValueChangeRes alloc] initWithPeripheral:peripheral Characteristic:characteristic];
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.onBLECharacteristicValueChange callWithArguments:@[res]];
        });
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error{
    //写反馈
    if (self.writeBLECharacteristicValueObject) {
        
        id<WXWriteBLECharacteristicValueObject> v = [self.writeBLECharacteristicValueObject implementProtocol:@protocol(WXWriteBLECharacteristicValueObject)];
        if (error == nil) {
            
            //写成功
            WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"writeBLECharacteristicValue:ok" errCode:0];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v success:res];
                [v complete:res];
            });
            
        }else {
            
            if (error.code == 3) {
                //特征不支持写
                WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"writeBLECharacteristicValue:fail not support write errCode 10007" errCode:10007];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [v fail:res];
                    [v complete:res];
                });
            }else {
                //其他错误
                WXBluetoothRes * res = [[WXBluetoothRes alloc] initWithErrMsg:@"writeBLECharacteristicValue:fail errCode 10008" errCode:10008];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [v fail:res];
                    [v complete:res];
                });
            }
            
        }
        
        self.writeBLECharacteristicValueObject = nil;
        
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

#pragma mark -- bluetooth

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

#pragma mark -- BLE

-(void) createBLEConnection:(KerJSObject *) object{
    [self.WXBluetooth createBLEConnection:object];
}

-(void) closeBLEConnection:(KerJSObject *)object{
    [self.WXBluetooth closeBLEConnection:object];
}

-(void) onBLEConnectionStateChange:(KerJSObject *) object{
    self.WXBluetooth.onBLEConnectionStateChange = object;
}

-(void) getBLEDeviceServices:(KerJSObject *) object{
    [self.WXBluetooth getBLEDeviceServices:object];
}

-(void) getBLEDeviceCharacteristics:(KerJSObject *) object{
    [self.WXBluetooth getBLEDeviceCharacteristics:object];
}

-(void) notifyBLECharacteristicValueChange:(KerJSObject *) object{
    [self.WXBluetooth notifyBLECharacteristicValueChange:object];
}

-(void) readBLECharacteristicValue:(KerJSObject *) object{
    [self.WXBluetooth readBLECharacteristicValue:object];
}

-(void) writeBLECharacteristicValue:(KerJSObject *) object{
    
}

-(void) onBLECharacteristicValueChange:(KerJSObject *) object{
    self.WXBluetooth.onBLECharacteristicValueChange = object;
}

@end


@implementation NSMutableArray (DeviceBluetooth)

-(BOOL)ker_alreadySaveObject:(id)object method:(ArrayCompareFunc)func{
    for (id a in self) {
        if (func(object, a)) {
            return YES;
        }
    }
    return NO;
}

@end


@implementation CBPeripheral(DeviceBluetooth)
-(CBService *)ker_findServiceByID:(NSString *)ID{
    for (CBService * service in self.services) {
        if ([service.UUID.UUIDString isEqualToString:ID]) {
            return service;
        }
    }
    return nil;
}
@end

@implementation CBService(DeviceBluetooth)
-(CBCharacteristic *)ker_findCharacteristicByID:(NSString *)ID{
    for (CBCharacteristic * characteristic in self.characteristics) {
        if ([characteristic.UUID.UUIDString isEqualToString:ID]) {
            return characteristic;
        }
    }
    return nil;
}
@end


@implementation NSDictionary (DeviceBluetooth)
- (NSMutableDictionary *)ker_mutableDeepCopy
{
    NSMutableDictionary *dict = [[NSMutableDictionary alloc] initWithCapacity:[self count]];
    //新建一个NSMutableDictionary对象，大小为原NSDictionary对象的大小
    NSArray *keys = [self allKeys];
    for(id key in keys)
    {//循环读取复制每一个元素
        id value = [self objectForKey:key];
        id copyValue;
        if ([value respondsToSelector:@selector(ker_mutableDeepCopy)]) {
            //如果key对应的元素可以响应mutableDeepCopy方法(还是NSDictionary)，调用mutableDeepCopy方法复制
            copyValue = [value ker_mutableDeepCopy];
        } else if ([value respondsToSelector:@selector(mutableCopy)])
        {
            copyValue = [value mutableCopy];
        }
        if (copyValue == nil)
            copyValue = [value copy];
        if ([key isKindOfClass:[NSString class]]) {
            [dict setObject:copyValue forKey:key];
        }else if ([key isKindOfClass:[CBUUID class]]){
            CBUUID * uuid = key;
            [dict setObject:copyValue forKey:[uuid UUIDString]];
        }
        
        
    }
    return dict;
}
@end
