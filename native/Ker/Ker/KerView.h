//
//  KerView.h
//  Ker
//
//  Created by hailong11 on 2018/12/28.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>

#include <ui/view.h>

@class KerApp;

@interface KerView : NSObject

@property(nonatomic,weak,readonly) KerApp * app;

-(instancetype) initWithApp:(KerApp *) app;

-(void) execCommand:(kk::ui::ViewCommand *) command;

-(void) execCanvasCommand:(kk::ui::CanvasCommand *) command;

@end

