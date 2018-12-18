//
//  KerWXObject+DeviceIbeacon.h
//  KerWX
//
//  Created by zuowu on 2018/12/18.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>

@interface WXIBeaconRes : WXCallbackRes
@property (nonatomic, assign) int errCode;
@end

@interface WXOnBeaconServiceChangeRes : NSObject
@property (nonatomic, assign) BOOL available;
@property (nonatomic, assign) BOOL discovering;
@end

@interface WXIBeaconInfo : NSObject
@property (nonatomic, copy) NSString * uuid;          //设备广播的 uuid
@property (nonatomic, copy) NSNumber * major;         //设备的主 id
@property (nonatomic, copy) NSNumber * minor;         //设备的次 id
@property (nonatomic, assign) NSInteger proximity;    //表示设备距离的枚举值
@property (nonatomic, assign) double accuracy;        //设备的距离
@property (nonatomic, assign) NSInteger rssi;            //信号强度
@end

@interface WXGetBeaconsRes : WXIBeaconRes
@property (nonatomic, copy) NSArray<WXIBeaconInfo *> * beacons;
@end

@interface WXOnBeaconUpdateRes : NSObject
@property (nonatomic, copy) NSArray<WXIBeaconInfo *> * beacons;
@end



@protocol WXStartBeaconDiscoveryObject <WXCallbackFunction>
@property (nonatomic, copy) NSArray * uuids;
@property (nonatomic, assign) BOOL ignoreBluetoothAvailable;
@end



@interface KerWXObject (DeviceIbeacon) 

-(void) startBeaconDiscovery: (KerJSObject *) object;
-(void) stopBeaconDiscovery: (KerJSObject *) object;

-(void)getBeacons:(KerJSObject *) object;

-(void) onBeaconServiceChange:(KerJSObject *) object;
-(void) onBeaconUpdate:(KerJSObject *) object;

@end


