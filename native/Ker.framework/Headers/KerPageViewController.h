//
//  KerPageViewController.h
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Ker/KerPage.h>
#import <Ker/KerApp.h>

@interface KerPageViewController : UIViewController<KerPageDelegate>

@property(nonatomic,strong) KerApp * app;
@property(nonatomic,strong) NSString * path;
@property(nonatomic,strong,readonly) KerPage * page;
@property(nonatomic,strong) NSDictionary<NSString *,NSString *> * query;

@end
