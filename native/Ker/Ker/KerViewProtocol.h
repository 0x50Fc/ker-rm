//
//  KerViewProtocol.h
//  Ker
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>

#if defined(__cplusplus)

namespace kk {
    namespace ui {
        class App;
        class View;
        class ViewConfiguration;
        class Page;
        class Command;
        class PageCommand;
    }
}

typedef unsigned long long KerId;
typedef kk::ui::App * KerAppCPointer;
typedef kk::ui::ViewConfiguration * KerViewConfigurationCPointer;
typedef kk::ui::Page * KerPageCPointer;
typedef kk::ui::Command * KerCommandCPointer;
typedef kk::ui::PageCommand * KerPageCommandCPointer;

#else

typedef unsigned long long KerId;
typedef void * KerAppCPointer;
typedef void * KerViewConfigurationCPointer;
typedef void * KerPageCPointer;
typedef void * KerCommandCPointer;
typedef void * KerPageCommandCPointer;

#endif

@class KerApp;

@protocol KerViewProtocol <NSObject>

+(instancetype) KerViewCreateWithConfiguration:(KerViewConfigurationCPointer) configuration app:(KerId) appId;

-(void) KerViewObtain:(KerId) viewId app:(KerId) appId;

-(void) KerView:(KerId) viewId setAttribute:(NSString *) key value:(NSString *) value app:(KerId) appId;

-(void) KerView:(KerId) viewId setContent:(NSString *) content contentType:(NSString *) contentType basePath:(NSString *) basePath app:(KerId) appId;

-(void) KerViewRecycle:(KerId) viewId app:(KerId) appId;

-(UIView *) KerViewContentView;

@end




