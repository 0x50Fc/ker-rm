//
//  KerPageViewController.m
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KerPageViewController.h"
#import "KerObject.h"

@interface KerPageViewController () {
    CGSize _layoutSize;
}

@end

@implementation KerPageViewController

-(void) dealloc {
    [_page recycle];
}

- (void)viewDidLoad {
    [super viewDidLoad];

    if(_app != nil && _path != nil) {
        _page = [[KerPage alloc] initWithView:self.view app:_app];
        _page.delegate = self;
        [_page run:_path query:_query];
    }
    
}


-(void) viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];
    if(CGSizeEqualToSize(_layoutSize, self.view.bounds.size)) {
        _layoutSize = self.view.bounds.size;
        [_page setSize:_layoutSize];
    }
}

-(void) KerPage:(KerPage *) page setOptions:(id) options {
    NSString * v =[options ker_getString:@"title"];
    if(v != nil) {
        self.title = v;
    }
}

-(void) KerPage:(KerPage *) page close:(BOOL) animated {
    
    UIViewController * topViewController = self;
    
    if(self.navigationController != nil) {
        if([self.navigationController.viewControllers count] <= 1) {
            topViewController = self.navigationController;
        } else {
            [self.navigationController popViewControllerAnimated:animated];
            return;
        }
    }
    
    if([topViewController presentedViewController]) {
        [topViewController dismissViewControllerAnimated:animated completion:nil];
    }
    
}

@end
