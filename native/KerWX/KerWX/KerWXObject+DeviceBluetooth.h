//
//  KerWXObject+DeviceBluetooth.h
//  KerWX
//
//  Created by zuowu on 2018/12/14.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>

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

@protocol WXStartBluetoothDevicesDiscoveryObject <WXCallbackFunction>
@property (nonatomic, copy) NSArray<NSString*> * services;
@property (nonatomic, assign) BOOL allowDuplicatesKey;
@property (nonatomic, assign) long interval;
@end

@protocol WXGetConnectedBluetoothDevicesObject <WXCallbackFunction>
@property (nonatomic, copy) NSArray<NSString*> * services;
@end


@interface KerWXObject (DeviceBluetooth)

-(void) getBluetoothAdapterState:(KerJSObject *) object;
-(void) onBluetoothAdapterStateChange:(KerJSObject *) object;

-(void) openBluetoothAdapter:(KerJSObject *) object;
-(void) closeBluetoothAdapter:(KerJSObject *) object;

-(void) startBluetoothDevicesDiscovery:(KerJSObject *) object;
-(void) stopBluetoothDevicesDiscovery:(KerJSObject *) object;

-(void) onBluetoothDeviceFound:(KerJSObject *) object;

-(void) getBluetoothDevices:(KerJSObject *) object;
-(void) getConnectedBluetoothDevices:(KerJSObject *) object;

@end


typedef BOOL (^ArrayCompareFunc)(id objA, id objB);

@interface NSMutableArray (WXBOpenBluetooth)

-(BOOL) ker_alreadySaveObject:(id)object method:(ArrayCompareFunc)func ;

@end

