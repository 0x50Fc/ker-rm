//
//  ViewController.m
//  demo
//
//  Created by hailong11 on 2018/11/16.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "ViewController.h"
#import <Ker/Ker.h>

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
}

- (IBAction)doTapAction:(id)sender {
    
    KerApp * app = [[KerApp alloc] initWithBasePath:[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"main"]];
    
    [app run:@{}];
    
}

@end
