//
//  UIView+KerViewProtocol.m
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "UIView+KerViewProtocol.h"
#import "KerObject.h"
#import "UIColor+Ker.h"
#include <ui/ui.h>
#include <ui/view.h>
#include <objc/runtime.h>

namespace kk {
    namespace ui {
        extern CGImageRef GetCGImage(kk::ui::Image * image);
    }
}

@implementation UIView (KerViewProtocol)

+(instancetype) KerViewCreateWithConfiguration:(void *)configuration {
    return [[self alloc] initWithFrame:CGRectZero];
}

-(UIView *) KerViewContentView {
    return self;
}

-(void) KerViewObtain:(KerViewCPointer) view {
    
}

-(void) KerView:(KerViewCPointer) view setAttribute:(const char *) key value:(const char *) value {
    
    if(key == nullptr) {
        return ;
    }
    
    if(strcmp(key, "background-color") == 0) {
        self.backgroundColor = [UIColor colorWithKerCString:value];
    } else if(strcmp(key, "transform") == 0) {
        kk::ui::Transform v = kk::ui::TransformFromString(value);
        self.transform = CGAffineTransformIdentity;
        self.transform = CGAffineTransformMake(v.a, v.b, v.c, v.d, v.tx, v.ty);
    } else if(strcmp(key, "background-image") == 0) {
        if(value == nullptr) {
            self.layer.contents = nil;
        } else {
            kk::String s(value);
            if(kk::CStringHasPrefix(value, "url(") && kk::CStringHasSuffix(value, ")")) {
                s = s.substr(4,s.length() - 5);
            }
            kk::ui::View * v = (kk::ui::View *) view;
            kk::Strong<kk::ui::Image> image = v->context()->createImage(s.c_str());
            CGImageRef i = kk::ui::GetCGImage(image);
            if(i != nil) {
                self.layer.contents = (__bridge id) i;
            } else {
                self.layer.contents = nil;
            }
        }
    } else if(strcmp(key, "background-gravity") == 0) {
        if(value == nullptr) {
            self.layer.contentsGravity = @"resize";
        } else {
            self.layer.contentsGravity = [NSString stringWithCString:value encoding:NSUTF8StringEncoding];
        }
    } else if(strcmp(key, "overflow") == 0) {
        self.clipsToBounds = kk::CStringEqual(value, "hidden");
    } else if(strcmp(key, "hidden") == 0) {
        self.hidden = value && strcmp(value, "true") == 0;
    } else if(strcmp(key, "enabled") == 0) {
        self.userInteractionEnabled = value && strcmp(value, "true") == 0;
    } else if(strcmp(key, "border-radius") == 0) {
        self.layer.cornerRadius = value ? atof(value) : 0;
    } else if(strcmp(key, "border-width") == 0) {
        self.layer.borderWidth = value ? atof(value) : 0;
    } else if(strcmp(key, "border-color") == 0) {
        self.layer.borderColor = [UIColor colorWithKerCString:value].CGColor;
    } else if(strcmp(key, "opacity") == 0) {
        self.layer.opacity = value ? atof(value) : 0;
    }
    
}

-(void) KerViewRecycle:(KerViewCPointer) view {
    
}

@end

