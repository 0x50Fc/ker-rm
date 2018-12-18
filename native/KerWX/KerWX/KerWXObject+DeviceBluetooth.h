//
//  KerWXObject+DeviceBluetooth.h
//  KerWX
//
//  Created by zuowu on 2018/12/14.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>
#import <CoreBluetooth/CoreBluetooth.h>

/*
 err type
 0        ok                    正常
 10000    not init              未初始化蓝牙适配器
 10001    not available         当前蓝牙适配器不可用
 10002    no device             没有找到指定设备
 10003    connection fail       连接失败
 10004    no service            没有找到指定服务
 10005    no characteristic     没有找到指定特征值
 10006    no connection         当前连接已断开
 10007    property not support  当前特征值不支持此操作
 10008    system error          其余所有系统上报的异常
 10009    system not support    Android 系统特有，系统版本低于 4.3 不支持 BLE
 */

@interface WXBluetoothRes : WXCallbackRes
@property (nonatomic, assign) int errCode;
@end

@interface WXBOpenBluetoothAdapterRes : WXBluetoothRes
@property (nonatomic, assign) int state;
@end

@interface WXGetBluetoothAdapterStateRes : WXBluetoothRes
@property(nonatomic, assign) BOOL discovering;
@property(nonatomic, assign) BOOL available;
@end

@interface WXOnBluetoothAdapterStateChangeRes : NSObject
@property(nonatomic, assign) BOOL discovering;
@property(nonatomic, assign) BOOL available;
@end

@interface WXStartBluetoothDevicesDiscoveryRes : WXBluetoothRes
@property (nonatomic, assign) BOOL isDiscovering;
@end

@interface WXOnBluetoothDeviceFoundPeripheral : NSObject
@property (nonatomic, copy) NSString * name;                              //蓝牙设备名称，某些设备可能没有
@property (nonatomic, copy) NSString * deviceId;                          //用于区分设备的 id
@property (nonatomic, copy) NSNumber * RSSI;                              //当前蓝牙设备的信号强度
@property (nonatomic, copy) NSData * advertisData;                        //当前蓝牙设备的广播数据段中的 ManufacturerData 数据段
@property (nonatomic, copy) NSArray<NSString *> * advertisServiceUUIDs;   //当前蓝牙设备的广播数据段中的 ServiceUUIDs 数据段
@property (nonatomic, copy) NSString * localName;                         //当前蓝牙设备的广播数据段中的 LocalName 数据段
@property (nonatomic, copy) NSDictionary * serviceData;                   //当前蓝牙设备的广播数据段中的 ServiceData 数据段
@end

@interface WXOnBluetoothDeviceFoundRes : NSObject
@property (nonatomic, copy) NSArray<WXOnBluetoothDeviceFoundPeripheral *> * devices;
@end

@interface WXGetBluetoothDevicesRes : WXBluetoothRes
@property (nonatomic, copy) NSArray<WXOnBluetoothDeviceFoundPeripheral *> * devices;
@end


@interface WXConnectedBluetoothDevices : NSObject
@property (nonatomic, copy) NSString * name;                              //蓝牙设备名称，某些设备可能没有
@property (nonatomic, copy) NSString * deviceId;                          //用于区分设备的 id
@end

@interface WXGetConnectedBluetoothDevicesRes : WXBluetoothRes
@property (nonatomic, copy) NSArray<WXConnectedBluetoothDevices *> * devices;
@end


@interface  WXOnBLEConnectionStateChangeRes : WXBluetoothRes
@property (nonatomic, copy) NSString * deviceId;
@property (nonatomic, assign) BOOL connected;
@end

@interface WXBLEDeviceService : NSObject
@property (nonatomic, copy) NSString * uuid;
@property (nonatomic, assign) BOOL isPrimary;
@end

@interface WXGetBLEDeviceServicesRes : WXBluetoothRes
@property (nonatomic, copy) NSString * deviceId;
@property (nonatomic, copy) NSArray<WXBLEDeviceService *> * services;
@end

@interface WXBLEDeviceCharacteristicPropertie : NSObject
@property (nonatomic, assign) BOOL read;                 //1<<1 CBCharacteristicPropertyRead
@property (nonatomic, assign) BOOL write;                //1<<3 CBCharacteristicPropertyWrite
@property (nonatomic, assign) BOOL notify;               //1<<4 CBCharacteristicPropertyNotify
@property (nonatomic, assign) BOOL indicate;             //1<<5 CBCharacteristicPropertyIndicate
@end

@interface WXBLEDeviceCharacteristic : NSObject
@property (nonatomic, strong) WXBLEDeviceCharacteristicPropertie * properties;
@property (nonatomic, copy) NSString * uuid;
@end

@interface WXGetBLEDeviceCharacteristicsRes : WXBluetoothRes
@property (nonatomic, copy) NSArray<WXBLEDeviceCharacteristic *> * characteristics;
@end

@interface WXOnBLECharacteristicValueChangeRes : NSObject
@property (nonatomic, copy) NSString * deviceId;
@property (nonatomic, copy) NSString * serviceId;
@property (nonatomic, copy) NSString * characteristicId;
@property (nonatomic, copy) NSData * value;
@end






@protocol WXStartBluetoothDevicesDiscoveryObject <WXCallbackFunction>
@property (nonatomic, copy) NSArray<NSString*> * services;
@property (nonatomic, assign) BOOL allowDuplicatesKey;
@property (nonatomic, assign) long interval;
@end

@protocol WXGetConnectedBluetoothDevicesObject <WXCallbackFunction>
@property (nonatomic, copy) NSArray<NSString*> * services;
@end

@protocol WXCreateBLEConnectionObject <WXCallbackFunction>
@property (nonatomic, strong) NSString * deviceId;
@property (nonatomic, assign) double timeout;
@end

@protocol WXCloseBLEConnectionObject <WXCallbackFunction>
@property (nonatomic, strong) NSString * deviceId;
@end

@protocol WXGetBLEDeviceServicesObject <WXCallbackFunction>
@property (nonatomic, strong) NSString * deviceId;
@end

@protocol WXBGetLEDeviceCharacteristicsObject <WXCallbackFunction>
@property (nonatomic, copy) NSString * deviceId;
@property (nonatomic, copy) NSString * serviceId;
@end

@protocol WXNotifyBLECharacteristicValueChangeObject <WXCallbackFunction>
@property (nonatomic, copy) NSString * deviceId;
@property (nonatomic, copy) NSString * serviceId;
@property (nonatomic, copy) NSString * characteristicId;
@property (nonatomic, assign) BOOL state;
@end

@protocol WXReadBLECharacteristicValueObject <WXCallbackFunction>
@property (nonatomic, copy) NSString * deviceId;
@property (nonatomic, copy) NSString * serviceId;
@property (nonatomic, copy) NSString * characteristicId;
@end

@protocol WXWriteBLECharacteristicValueObject <WXCallbackFunction>
@property (nonatomic, copy) NSString * deviceId;
@property (nonatomic, copy) NSString * serviceId;
@property (nonatomic, copy) NSString * characteristicId;
@property (nonatomic, copy) NSData * value;
@end


@interface KerWXObject (DeviceBluetooth)

/*bluetooth*/

-(void) getBluetoothAdapterState:(KerJSObject *) object;
-(void) onBluetoothAdapterStateChange:(KerJSObject *) object;

-(void) openBluetoothAdapter:(KerJSObject *) object;
-(void) closeBluetoothAdapter:(KerJSObject *) object;

-(void) startBluetoothDevicesDiscovery:(KerJSObject *) object;
-(void) stopBluetoothDevicesDiscovery:(KerJSObject *) object;

-(void) onBluetoothDeviceFound:(KerJSObject *) object;

-(void) getBluetoothDevices:(KerJSObject *) object;
-(void) getConnectedBluetoothDevices:(KerJSObject *) object;

/*BLE*/

-(void) createBLEConnection:(KerJSObject *) object;
-(void) closeBLEConnection:(KerJSObject *)object;
-(void) onBLEConnectionStateChange:(KerJSObject *) object;

-(void) getBLEDeviceServices:(KerJSObject *) object;
-(void) getBLEDeviceCharacteristics:(KerJSObject *) object;
-(void) notifyBLECharacteristicValueChange:(KerJSObject *) object;

-(void) readBLECharacteristicValue:(KerJSObject *) object;
-(void) writeBLECharacteristicValue:(KerJSObject *) object;

-(void) onBLECharacteristicValueChange:(KerJSObject *) object;

@end


typedef BOOL (^ArrayCompareFunc)(id objA, id objB);

@interface NSMutableArray (DeviceBluetooth)

-(BOOL) ker_alreadySaveObject:(id)object method:(ArrayCompareFunc)func ;

@end

@interface CBPeripheral(DeviceBluetooth)

-(CBService *)ker_findServiceByID:(NSString *)ID;

@end

@interface CBService(DeviceBluetooth)

-(CBCharacteristic *)ker_findCharacteristicByID:(NSString *)ID;

@end

@interface NSDictionary (DeviceBluetooth)

- (NSMutableDictionary *)ker_mutableDeepCopy;

@end





