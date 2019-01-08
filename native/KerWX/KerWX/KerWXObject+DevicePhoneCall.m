//
//  KerWXObject+DevicePhoneCall.m
//  KerWX
//
//  Created by zuowu on 2018/12/13.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+DevicePhoneCall.h"

@implementation KerWXObject (DevicePhoneCall)

-(UIViewController *)phoneCallViewController{
    return [[UIApplication sharedApplication] keyWindow].rootViewController;
}

-(void)makePhoneCall:(KerJSObject *) kerJSObject{
    
    id<WXMakePhoneCallObject> object = [kerJSObject implementProtocol:@protocol(WXMakePhoneCallObject)];
    UIAlertController * actionSheet = [UIAlertController alertControllerWithTitle:nil message:nil preferredStyle:UIAlertControllerStyleActionSheet];
    
    NSString * str = [NSString stringWithFormat:@"呼叫 %@", object.phoneNumber];
    UIAlertAction *action1 = [UIAlertAction actionWithTitle:str style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        
        NSMutableString * callPhone = [NSMutableString stringWithFormat:@"tel:%@",object.phoneNumber];
        
        if (@available(iOS 10.0, *)) {
            
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:callPhone] options:@{} completionHandler:^(BOOL success) {
                
                if (success) {
                    
                    WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"makePhoneCall:ok"];
                    [object success:res];
                    [object complete:res];
                    
                }else{
                    
                    WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"makePhoneCall:fail unknow"];
                    [object fail:res];
                    [object complete:res];

                }
            }];
            
        } else {
            // Fallback on earlier versions
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:callPhone]];
            WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"makePhoneCall:ok"];
            [object success:res];
            [object complete:res];
        }
    }];
    
    UIAlertAction *action2 = [UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {

        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"makePhoneCall:fail cancel"];
        [object fail:res];
        [object complete:res];
        
    }];
    
    [actionSheet addAction:action1];
    [actionSheet addAction:action2];
    
    [self.phoneCallViewController presentViewController:actionSheet animated:YES completion:nil];
    
}

@end
