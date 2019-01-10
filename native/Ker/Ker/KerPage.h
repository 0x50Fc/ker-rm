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

@end

@interface KerPage : NSObject

@property(nonatomic,weak) id<KerPageDelegate> delegate;
@property(nonatomic,assign,readonly) KerPageCPointer page;
@property(nonatomic,strong,readonly) UIView * view;

-(instancetype) initWithPage:(KerPageCPointer) page;

-(void) recycle;

-(void) setOptions:(id) options ;

-(void) close:(BOOL) animated;

-(void) setSize:(CGSize) size;

-(void) open:(UIView *) view;

-(void) viewDidLayoutSubviews;

@end

