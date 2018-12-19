//
//  KerWXObject+DeviceScreen.h
//  KerWX
//
//  Created by zuowu on 2018/12/19.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>

@interface WXGetScreenBrightnessRes : WXCallbackRes
@property (nonatomic, assign) double value;
@end



@protocol WXSetScreenBrightnessObject <WXCallbackFunction>
@property (nonatomic, assign) double value;
@end

@protocol WXSetKeepScreenOnObject <WXCallbackFunction>
@property (nonatomic, assign) BOOL keepScreenOn;
@end


@interface KerWXObject (DeviceScreen)

-(void) onUserCaptureScreen:(KerJSObject *) object;

-(void) getScreenBrightness:(KerJSObject *) object;

-(void) setScreenBrightness:(KerJSObject *) object;

-(void) setKeepScreenOn:(KerJSObject *) object;

@end


