//
//  KerPageWindowController.h
//  Ker
//
//  Created by zhanghailong on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Ker/KerPage.h>
#import <ker/KerViewProtocol.h>

@interface KerPageWindowController : NSObject<KerPageDelegate>

@property(nonatomic,assign,readonly) KerId pageId;

-(instancetype) initWithPageId:(KerId) pageId;

-(void) showInView:(UIView *) view animated:(BOOL) animated;

-(void) showAnimated:(BOOL) animated;

-(void) viewDidLayoutSubviews;

+(KerPageWindowController *) topPageController;

+(NSArray *) pageControllers;

@end
