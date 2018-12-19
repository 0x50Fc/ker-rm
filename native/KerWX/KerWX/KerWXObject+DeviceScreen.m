//
//  KerWXObject+DeviceScreen.m
//  KerWX
//
//  Created by zuowu on 2018/12/19.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+DeviceScreen.h"
#import <objc/runtime.h>

#define WXScreenKey "WXScreenKey"

@implementation WXGetScreenBrightnessRes

@synthesize value = _value;

-(instancetype)initWithValue:(double) value errMsg:(NSString *) msg{
    if (self = [super init]) {
        self.value = value;
        self.errMsg = msg;
    }
    return self;
}

@end


@interface WXScreen : NSObject

@property (nonatomic, strong) KerJSObject * onUserCaptureScreen;

@end

@implementation WXScreen

@synthesize onUserCaptureScreen = _onUserCaptureScreen;

-(void)setOnUserCaptureScreen:(KerJSObject *)onUserCaptureScreen{
    
    _onUserCaptureScreen = onUserCaptureScreen;

    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationUserDidTakeScreenshotNotification object:nil queue:nil usingBlock:^(NSNotification * _Nonnull note) {
        
        if (self.onUserCaptureScreen) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [self.onUserCaptureScreen callWithArguments:@[@"12d"]];
            });
            
        }
    }];
}

-(void) getScreenBrightness:(KerJSObject *) object{
    
    id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];
    WXGetScreenBrightnessRes * res = [[WXGetScreenBrightnessRes alloc] initWithValue:[UIScreen mainScreen].brightness errMsg:@"getScreenBrightness:ok"];
    dispatch_async(dispatch_get_main_queue(), ^{
        [v success:res];
        [v complete:res];
    });
}

-(void) setScreenBrightness:(KerJSObject *) object{
    
    id<WXSetScreenBrightnessObject> v = [object implementProtocol:@protocol(WXSetScreenBrightnessObject)];
    [[UIScreen mainScreen] setBrightness:v.value];
    WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"setScreenBrightness:ok"];
    dispatch_async(dispatch_get_main_queue(), ^{
        [v success:res];
        [v complete:res];
    });

}

-(void) setKeepScreenOn:(KerJSObject *) object{
    
    id<WXSetKeepScreenOnObject> v = [object implementProtocol:@protocol(WXSetKeepScreenOnObject)];
    [[UIApplication sharedApplication] setIdleTimerDisabled:v.keepScreenOn];
    WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"setKeepScreenOn:ok"];
    dispatch_async(dispatch_get_main_queue(), ^{
        [v success:res];
        [v complete:res];
    });
}

@end

@implementation KerWXObject (DeviceScreen)

-(WXScreen *)WXScreen{
    WXScreen * screen = objc_getAssociatedObject(self, WXScreenKey);
    if (screen == nil) {
        screen = [[WXScreen alloc]init];
        objc_setAssociatedObject(self, WXScreenKey, screen, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return screen;
}

-(void) onUserCaptureScreen:(KerJSObject *) object{
    self.WXScreen.onUserCaptureScreen = object;
}
-(void) getScreenBrightness:(KerJSObject *) object{
    [self.WXScreen getScreenBrightness:object];
}
-(void) setScreenBrightness:(KerJSObject *) object{
    [self.WXScreen setScreenBrightness:object];
}
-(void) setKeepScreenOn:(KerJSObject *) object{
    [self.WXScreen setKeepScreenOn:object];
}

@end
