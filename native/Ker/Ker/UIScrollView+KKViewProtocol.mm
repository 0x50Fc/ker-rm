//
//  UIScrollView+KerViewProtocol.m
//  KK
//
//  Created by zhanghailong on 2018/11/8.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "UIScrollView+KerViewProtocol.h"
#import "KerObject.h"
#include <ui/ui.h>
#include <ui/view.h>
#include <objc/runtime.h>
#import "KerUI.h"

@interface UIScrollViewKKViewProtocol : NSObject

@property(nonatomic,assign) CGPoint contentOffset;
@property(nonatomic,assign) kk::Uint64 viewId;
@property(nonatomic,assign) KerId app;

@end

@implementation UIScrollViewKKViewProtocol


-(instancetype) init{
    if((self = [super init])) {
        _contentOffset.x = NSNotFound;
        _contentOffset.y = NSNotFound;
    }
    return self;
}

-(void) observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey,id> *)change context:(void *)context {
    
    if([object isKindOfClass:[UIScrollView class]] && [keyPath isEqualToString:@"contentOffset"]
       && _viewId != 0 && _app != 0) {
        
        CGPoint offset = [object contentOffset];
        
        if(!CGPointEqualToPoint(_contentOffset, offset)) {
            _contentOffset = offset;
            [KerUI app:_app setContentOffset:offset viewId:_viewId];
            [KerUI app:_app emit:@"scroll" viewId:_viewId data:nil];
        }
        
    }
    
}

@end

@implementation UIScrollView (KerViewProtocol)

-(void) KerViewObtain:(KerId) viewId app:(KerId) app {
    [super KerViewObtain:viewId app:app];
    if (@available(iOS 11.0, *)) {
       self.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
    }
    UIScrollViewKKViewProtocol * object = [[UIScrollViewKKViewProtocol alloc] init];
    object.viewId = viewId;
    object.app = app;
    [self addObserver:object forKeyPath:@"contentOffset" options:NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld context:nil];
    objc_setAssociatedObject(self, "__UIScrollViewKKViewProtocol", object, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

-(void) KerViewRecycle:(KerId) viewId app:(KerId) app{
    [super KerViewRecycle:viewId app:app];
    UIScrollViewKKViewProtocol * object = objc_getAssociatedObject(self, "__UIScrollViewKKViewProtocol");
    if(object) {
        object.viewId = 0;
        object.app = 0;
        [self removeObserver:object forKeyPath:@"contentOffset"];
        objc_setAssociatedObject(self, "__UIScrollViewKKViewProtocol", nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
}

@end

