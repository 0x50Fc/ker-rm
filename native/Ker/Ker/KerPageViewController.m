//
//  KerPageViewController.m
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KerPageViewController.h"
#import "KerObject.h"
#import "UIColor+Ker.h"
#import "KerUI.h"

@interface KerPageViewController () {
   
}

@end

@implementation KerPageViewController

@synthesize pageId = _pageId;

-(instancetype) initWithPageId:(KerId) pageId {
    if((self = [super initWithNibName:nil bundle:nil])) {
        _pageId = pageId;
        [[KerUI getPage:pageId] setDelegate:self];
    }
    return self;
}

-(void) dealloc {
    [KerUI removePage:_pageId];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [[KerUI getPage:_pageId] open:self.view];
}


-(void) viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];
    [[KerUI getPage:_pageId] viewDidLayoutSubviews];
}

-(void) KerPage:(KerPage *) page setOptions:(id) options {
    {
        NSString * v =[options ker_getString:@"title"];
        if(v != nil) {
            self.title = v;
        }
    }
    {
        NSString * v =[options ker_getString:@"backgroundColor"];
        if(v != nil ) {
            self.view.backgroundColor = [UIColor colorWithKerCString:[v UTF8String]];
        }
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
