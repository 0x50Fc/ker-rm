//
//  KerPageViewController.h
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Ker/KerViewProtocol.h>
#import <Ker/KerPage.h>

@interface KerPageViewController : UIViewController<KerPageDelegate>

@property(nonatomic,strong) UIView * contentView;
@property(nonatomic,assign,readonly) KerId pageId;

-(instancetype) initWithPageId:(KerId) pageId;

@end
