//
//  KerPage.h
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>

#if defined(__cplusplus)

namespace kk {
    namespace ui {
        class Page;
    }
}

typedef kk::ui::Page * KerPageCPointer;

#else

typedef void * KerPageCPointer;

#endif

@class KerPage;

@protocol KerPageDelegate

@optional

-(void) KerPage:(KerPage *) page setOptions:(id) options;

-(void) KerPage:(KerPage *) page close:(BOOL) animated;

-(void) KerPage:(KerPage *) page setLeftView:(UIView *) leftView;

-(void) KerPage:(KerPage *) page setRightView:(UIView *) rightView;

-(void) KerPage:(KerPage *) page setTitleView:(UIView *) titleView;

@end

@interface KerPage : NSObject

@property(nonatomic,weak) id<KerPageDelegate> delegate;
@property(nonatomic,assign,readonly) KerPageCPointer page;
@property(nonatomic,strong,readonly) UIView * view;
@property(nonatomic,strong,readonly) NSString * type;

-(instancetype) initWithPage:(KerPageCPointer) page;

-(void) recycle;

-(void) setOptions:(id) options ;

-(void) close:(BOOL) animated;

-(void) setSize:(CGSize) size;

-(void) open:(UIView *) view;

-(void) viewDidLayoutSubviews;

-(void) setLeftView:(UIView *) view;

-(void) setRightView:(UIView *) view;

-(void) setTitleView:(UIView *) view;

@end

