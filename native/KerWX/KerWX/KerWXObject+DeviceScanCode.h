//
//  KerWXObject+DeviceScanCode.h
//  KerWX
//
//  Created by zuowu on 2018/12/19.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>

@interface WXScanCodeRes : WXCallbackRes
@property (nonatomic, copy) NSString * result;
@property (nonatomic, copy) NSString * scanType;
@property (nonatomic, copy) NSString * charSet;
@property (nonatomic, copy) NSString * path;
@property (nonatomic, copy) NSString * rawData;
@end

@protocol WXScanCodeObject <WXCallbackFunction>
@property (nonatomic, assign) BOOL onlyFromCamera;
@property (nonatomic, copy) NSArray<NSString *> * scanType;
@end

@interface KerWXObject (DeviceScanCode)

-(void) scanCode:(KerJSObject *) object;

@end

@interface NSString (DeviceScanCode)

-(NSString *)ker_scanType;

@end


