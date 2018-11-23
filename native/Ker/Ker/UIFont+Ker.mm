//
//  UIFont+Ker.m
//  Ker
//
//  Created by hailong11 on 2018/11/21.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "UIFont+Ker.h"

@implementation UIFont (Ker)

+(UIFont *) fontWithKerUIFont:(kk::ui::Font *) v {
    
    if(v == nullptr) {
        return nil;
    }
    
    UIFont * font = nil;
    
    if(v->family != "") {
        NSString * name = [[UIFont fontNamesForFamilyName:[NSString stringWithCString:v->family.c_str() encoding:NSUTF8StringEncoding]] firstObject];
        if(name != nil) {
            font = [UIFont fontWithName:name size:v->size];
        }
    }
    
    if(font == nil && v->weight == kk::ui::FontWeightBold) {
        font = [UIFont boldSystemFontOfSize:v->size];
    }
    
    if(font == nil && v->style == kk::ui::FontStyleItalic) {
        font = [UIFont italicSystemFontOfSize:v->size];
    }
    
    if(font == nil ) {
        font = [UIFont systemFontOfSize:v->size];
    }
    
    return font;
}

+(UIFont *) fontWithKerCString:(const char *) font {
    kk::ui::Font v(font);
    return [self fontWithKerUIFont:&v];
}

@end
