//
//  KerPage.h
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Ker/KerApp.h>

typedef void * KerPageCPointer;

@class KerPage;

@protocol KerPageDelegate

@optional

-(void) KerPage:(KerPage *) page setOptions:(id) options;

@end

@interface KerPage : NSObject

@property(nonatomic,weak) id<KerPageDelegate> delegate;
@property(nonatomic,readonly,strong) KerApp  * app;
@property(nonatomic,readonly,strong) UIView * view;
@property(nonatomic,assign,readonly) KerPageCPointer CPointer;
@property(nonatomic,readonly,strong) NSMutableDictionary * librarys;

-(instancetype) initWithView:(UIView *) view app:(KerApp *) app;

-(void) run:(NSString *) path query:(NSDictionary<NSString *,NSString *> *) query;

-(void) setSize:(CGSize) size;

-(void) recycle;

@end

