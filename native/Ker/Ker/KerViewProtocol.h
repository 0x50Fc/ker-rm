//
//  KerViewProtocol.h
//  Ker
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>

@protocol KerViewProtocol <NSObject>

+(instancetype) KerViewCreateWithConfiguration:(void *) configuration;

-(void) KerViewObtain:(void *) view;

-(void) KerViewSetAttribute:(const char *) key value:(const char *) value;

-(void) KerViewRecycle:(void *) view;

-(UIView *) KerViewContentView;

@end




