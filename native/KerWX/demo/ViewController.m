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
    
    [KerUI open:[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"app"] query:@{} callback:nil];
    
//    [KerApp run:@"http://10.222.96.92:8080/1.0.ker" query:@{}];
    
}

@end
