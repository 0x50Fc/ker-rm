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
    }
}

typedef unsigned long long KerViewId;

typedef kk::ui::ViewConfiguration * KerViewConfigurationCPointer;

#else

typedef unsigned long long KerViewId;

typedef void * KerViewConfigurationCPointer;

#endif

@class KerApp;

@protocol KerViewProtocol <NSObject>

+(instancetype) KerViewCreateWithConfiguration:(KerViewConfigurationCPointer) configuration app:(KerApp *) app;

-(void) KerViewObtain:(KerViewId) view app:(KerApp *) app;

-(void) KerView:(KerViewId) view setAttribute:(NSString *) key value:(NSString *) value app:(KerApp *) app;

-(void) KerView:(KerViewId) view setContent:(NSString *) content contentType:(NSString *) contentType basePath:(NSString *) basePath app:(KerApp *) app;

-(void) KerViewRecycle:(KerViewId) view app:(KerApp *) app;

-(UIView *) KerViewContentView;

@end




