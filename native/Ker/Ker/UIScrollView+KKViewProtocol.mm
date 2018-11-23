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

@interface UIScrollViewKKViewProtocol : NSObject

@property(nonatomic,assign) CGPoint contentOffset;
@property(nonatomic,assign) kk::ui::View * view;

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
    
    if([object isKindOfClass:[UIScrollView class]] && [keyPath isEqualToString:@"contentOffset"] && _view) {
        
        if(!CGPointEqualToPoint(_contentOffset, [object contentOffset])) {
            kk::Strong<kk::Event> e = new kk::Event();
            _view->emit("scroll", e);
            _contentOffset = [object contentOffset];
        }
        
    }
    
}

@end

@implementation UIScrollView (KerViewProtocol)

-(void) KerViewObtain:(KerViewCPointer) view {
    [super KerViewObtain:view];
    UIScrollViewKKViewProtocol * object = [[UIScrollViewKKViewProtocol alloc] init];
    object.view = (kk::ui::View *) view;
    [self addObserver:object forKeyPath:@"contentOffset" options:NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld context:nil];
    objc_setAssociatedObject(self, "__UIScrollViewKKViewProtocol", object, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

-(void) KerViewRecycle:(KerViewCPointer) view {
    [super KerViewRecycle:view];
    UIScrollViewKKViewProtocol * object = objc_getAssociatedObject(self, "__UIScrollViewKKViewProtocol");
    if(object) {
        [self removeObserver:object forKeyPath:@"contentOffset"];
        objc_setAssociatedObject(self, "__UIScrollViewKKViewProtocol", nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
}

@end

