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
    CGRect _keyboardFrame;
}

@property(nonatomic,assign) BOOL navigationBarHidden;

@end

@implementation KerPageViewController

@synthesize pageId = _pageId;
@synthesize navigationBarHidden = _navigationBarHidden;

-(instancetype) initWithPageId:(KerId) pageId {
    if((self = [super initWithNibName:nil bundle:nil])) {
        
        self.edgesForExtendedLayout = UIRectEdgeAll;
        self.automaticallyAdjustsScrollViewInsets = NO;
        self.navigationItem.backBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@" " style:UIBarButtonItemStylePlain target:nil action:nil];
        
        _pageId = pageId;
        [[KerUI getPage:pageId] setDelegate:self];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardVisible:) name:UIKeyboardWillShowNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardHidden:) name:UIKeyboardWillHideNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardVisible:) name:UIKeyboardWillChangeFrameNotification object:nil];
        
    }
    return self;
}

-(void) dealloc {
    
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIKeyboardWillShowNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIKeyboardWillHideNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIKeyboardWillChangeFrameNotification object:nil];
    
    
    [KerUI removePage:_pageId];
}


-(void) keyboardVisible:(NSNotification *) notification {
    _keyboardFrame = [[[notification userInfo] valueForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];
    if(_contentView != nil) {
        CGRect r = self.view.bounds;
        r.origin = CGPointZero;
        r.size.height -= _keyboardFrame.size.height;
        _contentView.frame = r;
        [[KerUI getPage:_pageId] viewDidLayoutSubviews];
    }
}

-(void) keyboardHidden:(NSNotification *) notification {
    _keyboardFrame = CGRectZero;
    if(_contentView != nil) {
        CGRect r = self.view.bounds;
        r.origin = CGPointZero;
        _contentView.frame = r;
        [[KerUI getPage:_pageId] viewDidLayoutSubviews];
    }
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    if(_contentView == nil) {
        CGRect r = self.view.bounds;
        r.origin = CGPointZero;
        r.size.height -= _keyboardFrame.size.height;
        _contentView = [[UIView alloc] initWithFrame:r];
        [_contentView setAutoresizingMask:UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight];
    }
    
    [self.view addSubview:_contentView];
    
    [[KerUI getPage:_pageId] open:_contentView];
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
            self.contentView.backgroundColor = [UIColor colorWithKerCString:[v UTF8String]];
        }
    }
    {
        NSString * v =[options ker_getString:@"navigationBarHidden"];
        if(v != nil ) {
            _navigationBarHidden = [v boolValue];
            [self.navigationController setNavigationBarHidden:_navigationBarHidden animated:YES];
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


-(void) KerPage:(KerPage *) page setLeftView:(UIView *) leftView {
    if(leftView == nil) {
        self.navigationItem.leftBarButtonItem = nil;
    } else {
        self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:leftView];
    }
}

-(void) KerPage:(KerPage *) page setRightView:(UIView *) rightView {
    if(rightView == nil) {
        self.navigationItem.rightBarButtonItem = nil;
    } else {
        self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:rightView];
    }
}

-(void) KerPage:(KerPage *) page setTitleView:(UIView *) titleView {
    self.navigationItem.titleView = titleView;
}

-(void) viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    if([self.navigationController isNavigationBarHidden]) {
        [self.navigationController setNavigationBarHidden:NO animated:YES];
    }
}

-(void) viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    if([self.navigationController isNavigationBarHidden] != _navigationBarHidden) {
        [self.navigationController setNavigationBarHidden:_navigationBarHidden animated:YES];
    }
}

@end
