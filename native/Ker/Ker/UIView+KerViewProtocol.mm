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
#include <ui/app.h>
#include <objc/runtime.h>
#import "KerUI.h"
#import "UIImage+Ker.h"

namespace kk {
    namespace ui {
        extern CGImageRef GetCGImage(kk::ui::Image * image);
    }
}

@implementation UIView (KerViewProtocol)

+(instancetype) KerViewCreateWithConfiguration:(KerViewConfigurationCPointer)configuration app:(KerId) app {
    return [[self alloc] initWithFrame:CGRectZero];
}

-(UIView *) KerViewContentView {
    return self;
}

-(void) KerViewObtain:(KerId) view app:(KerId) app {
    
}

-(void) KerView:(KerId) view setAttribute:(NSString *) key value:(NSString *) value app:(KerId) app{
    
    if(key == nil) {
        return ;
    }
    
    if([key isEqualToString:@"background-color"]) {
        self.backgroundColor = [UIColor colorWithKerCString:[value UTF8String]];
    } else if([key isEqualToString:@"transform"]) {
        kk::ui::Transform v = kk::ui::TransformFromString([value UTF8String]);
        self.transform = CGAffineTransformIdentity;
        self.transform = CGAffineTransformMake(v.a, v.b, v.c, v.d, v.tx, v.ty);
    } else if([key isEqualToString:@"background-image"]) {
        if(value == nullptr) {
            self.layer.contents = nil;
        } else {
            if([value hasPrefix:@"url("] && [value hasSuffix:@")"]) {
                NSRange r = {4,[value length] - 5};
                value = [value substringWithRange:r];
            }
            UIImage * image = [UIImage ker_imageWithURI:[KerUI resolvePath:value app:app]];
            self.layer.contents = (id) [image CGImage];
        }
    } else if([key isEqualToString:@"background-gravity"] || [key isEqualToString:@"gravity"]) {
        if(value == nullptr) {
            self.layer.contentsGravity = @"resize";
        } else {
            self.layer.contentsGravity = value;
        }
    } else if([key isEqualToString:@"overflow"]) {
        self.clipsToBounds = [value isEqualToString:@"hidden"];
    } else if([key isEqualToString:@"hidden"]) {
        self.hidden = value && ![value isEqualToString:@"false"];
    } else if([key isEqualToString:@"enabled"]) {
        self.userInteractionEnabled = value && ![value isEqualToString:@"false"];
    } else if([key isEqualToString:@"border-radius"]) {
        self.layer.cornerRadius = [value doubleValue];
    } else if([key isEqualToString:@"border-width"]) {
        self.layer.borderWidth = [value doubleValue];
    } else if([key isEqualToString:@"border-color"]) {
        self.layer.borderColor = [UIColor colorWithKerCString:[value UTF8String]].CGColor;
    } else if([key isEqualToString:@"opacity"]) {
        self.layer.opacity = [value doubleValue];
    }
    
}

-(void) KerViewRecycle:(KerId) view app:(KerId) app {
    
}

-(void) KerView:(KerId) view setContent:(NSString *) content contentType:(NSString *) contentType basePath:(NSString *) basePath app:(KerId) app {
    
}

@end

