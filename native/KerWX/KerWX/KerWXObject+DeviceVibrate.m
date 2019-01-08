//
//  KerWXObject+DeviceVibrate.m
//  KerWX
//
//  Created by zuowu on 2018/12/19.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+DeviceVibrate.h"
#import <AudioToolbox/AudioToolbox.h>

@implementation KerWXObject (DeviceVibrate)

-(void) vibrateLong:(KerJSObject *) object{
    AudioServicesPlaySystemSoundWithCompletion(kSystemSoundID_Vibrate, ^{
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"vibrateLong:ok"];
        id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];
        [v success:res];
        [v complete:res];
    });
}

-(void) vibrateShort:(KerJSObject *) object{
    //手机发生较短时间的振动（15 ms）。仅在 iPhone 7 / 7 Plus 以上及 Android 机型生效
    //短震动还在研究
    if (@available(iOS 10.0, *)) {
        UIImpactFeedbackGenerator *generator = [[UIImpactFeedbackGenerator alloc] initWithStyle: UIImpactFeedbackStyleLight];
        [generator prepare];
        [generator impactOccurred];
    } else {
        AudioServicesPlaySystemSound(1520);
    }
    
    WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"vibrateShort:ok"];
    id<WXCallbackFunction> v = [object implementProtocol:@protocol(WXCallbackFunction)];
    [v success:res];
    [v complete:res];
}
@end
