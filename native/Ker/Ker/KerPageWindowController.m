//
//  KerPageWindowController.m
//  Ker
//
//  Created by zhanghailong on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerPageWindowController.h"
#import "KerUI.h"

@interface KerPageWindowController () {
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

@synthesize pageId = _pageId;

-(instancetype) initWithPageId:(KerId) pageId {
    if((self = [super init])) {
        _pageId = pageId;
        [[KerUI getPage:pageId] setDelegate:self];
    }
    return self;
}

-(void) dealloc {
    [KerUI removePage:_pageId];
}

-(void) showInView:(UIView *) view animated:(BOOL) animated {

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(doUIApplicationWillChangeStatusBarOrientationNotification) name:UIApplicationWillChangeStatusBarOrientationNotification object:nil];
    
    [(NSMutableArray *)[KerPageWindowController pageControllers] addObject:self];
    
    [[KerUI getPage:_pageId] open:view];
    
    
}

-(void) doUIApplicationWillChangeStatusBarOrientationNotification {
    [[KerUI getPage:_pageId] viewDidLayoutSubviews];
}

-(void) showAnimated:(BOOL) animated {
    [self showInView:[UIApplication sharedApplication].keyWindow animated:animated];
}

-(void) _close {
    
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationWillChangeStatusBarOrientationNotification object:nil];
    
    KerId pageId = _pageId;
    
    [(NSMutableArray *)[KerPageWindowController pageControllers] removeObject:self];
    
    [KerUI removePage:pageId];
    
}

-(void) KerPage:(KerPage *)pageController close:(BOOL)animated {
    
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
