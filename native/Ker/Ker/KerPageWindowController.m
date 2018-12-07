//
//  KerPageWindowController.m
//  Ker
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerPageWindowController.h"

@interface KerPageWindowController () {
    CGSize _layoutSize;
}

@end

@implementation KerPageWindowController

+(KerPageWindowController *) topPageController {
    return [[self pageControllers] lastObject];
}

+(NSArray *) pageControllers {
    static NSMutableArray * v = nil;
    if(v == nil) {
        v = [NSMutableArray arrayWithCapacity:4];
    }
    return v;
}

-(void) dealloc {
    [_page recycle];
}

-(void) showInView:(UIView *) view animated:(BOOL) animated {
    
    if(_page != nil) {
        return;
    }
    
    [(NSMutableArray *)[KerPageWindowController pageControllers] addObject:self];
    
    if(_app != nil && _path != nil) {
        _page = [[KerPage alloc] initWithView:view app:_app];
        _page.delegate = self;
        [_page run:_path query:_query];
    }
    
    [self viewDidLayoutSubviews];
    
}

-(void) showAnimated:(BOOL) animated {
    [self showInView:[UIApplication sharedApplication].keyWindow animated:animated];
}

-(void) viewDidLayoutSubviews {
    
    if(_page == nil) {
        return;
    }
    
    UIView * view = _page.view;
    
    if(CGSizeEqualToSize(_layoutSize, view.bounds.size)) {
        _layoutSize = view.bounds.size;
        [_page setSize:_layoutSize];
    }
    
}

-(void) _close {
    
    if(_page == nil) {
        return;
    }
    
    KerPage * page = _page;
    _page.delegate = nil;
    _page = nil;
    
    [(NSMutableArray *)[KerPageWindowController pageControllers] removeObject:self];
    
    [page recycle];
}

-(void) KerPage:(KerPage *) page close:(BOOL) animated {
    
    if(animated) {
        __weak KerPageWindowController * v = self;
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(300 * NSEC_PER_MSEC)), dispatch_get_main_queue(), ^{
            [v _close];
        });
        
    } else {
        [self _close];
    }
    
}

@end
