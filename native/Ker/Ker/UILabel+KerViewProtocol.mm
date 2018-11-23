//
//  UILabel+KerViewProtocol.m
//  Ker
//
//  Created by hailong11 on 2018/11/21.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "UILabel+KerViewProtocol.h"
#import "UIColor+Ker.h"
#import "UIFont+Ker.h"

@implementation UILabel (KerViewProtocol)

-(void) KerViewObtain:(KerViewCPointer) view {
    [super KerViewObtain:view];
    self.numberOfLines = 0;
}

-(void) KerView:(KerViewCPointer)view setAttribute:(const char *)key value:(const char *)value {
    [super KerView:view setAttribute:key value:value];
    if(key == nullptr) {
        return;
    }
    if(strcmp(key, "color") == 0) {
        self.textColor = [UIColor colorWithKerCString:value];
    } else if(strcmp(key, "font") == 0) {
        self.font = [UIFont fontWithKerCString:value];
    } else if(strcmp(key, "text-align") == 0) {
        kk::ui::TextAlign v = kk::ui::TextAlignFromString(value);
        switch (v) {
            case kk::ui::TextAlignCenter:
                self.textAlignment = NSTextAlignmentCenter;
                break;
            case kk::ui::TextAlignEnd:
            case kk::ui::TextAlignRight:
                self.textAlignment = NSTextAlignmentRight;
                break;
            default:
                self.textAlignment = NSTextAlignmentLeft;
                break;
        }
    } else if(strcmp(key, "#text") == 0) {
        self.text = value == nullptr ? nil : [NSString stringWithCString:value encoding:NSUTF8StringEncoding];
    }
}

@end
