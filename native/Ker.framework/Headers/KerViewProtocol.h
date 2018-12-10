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
        class View;
        class ViewConfiguration;
    }
}

typedef kk::ui::View * KerViewCPointer;
typedef kk::ui::ViewConfiguration * KerViewConfigurationCPointer;

#else

typedef void * KerViewCPointer;

typedef void * KerViewConfigurationCPointer;

#endif

@protocol KerViewProtocol <NSObject>

+(instancetype) KerViewCreateWithConfiguration:(KerViewConfigurationCPointer) configuration;

-(void) KerViewObtain:(KerViewCPointer) view;

-(void) KerView:(KerViewCPointer) view setAttribute:(const char *) key value:(const char *) value;

-(void) KerView:(KerViewCPointer) view setContent:(const char *) content contentType:(const char *) contentType basePath:(const char *) basePath;

-(void) KerViewRecycle:(KerViewCPointer) view;

-(UIView *) KerViewContentView;

@end




