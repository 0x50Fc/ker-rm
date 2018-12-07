//
//  KerPageWindowController.h
//  Ker
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Ker/KerPage.h>
#import <Ker/KerApp.h>

@interface KerPageWindowController : NSObject<KerPageDelegate>

@property(nonatomic,strong) KerApp * app;
@property(nonatomic,strong) NSString * path;
@property(nonatomic,strong,readonly) KerPage * page;
@property(nonatomic,strong) NSDictionary<NSString *,NSString *> * query;

-(void) showInView:(UIView *) view animated:(BOOL) animated;

-(void) showAnimated:(BOOL) animated;

-(void) viewDidLayoutSubviews;

+(KerPageWindowController *) topPageController;

+(NSArray *) pageControllers;

@end
