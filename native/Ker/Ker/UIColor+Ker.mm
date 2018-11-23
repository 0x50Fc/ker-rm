//
//  UIColor+Ker.m
//  Ker
//
//  Created by hailong11 on 2018/11/21.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "UIColor+Ker.h"

@implementation UIColor (Ker)

+(UIColor *) colorWithKerUIColor:(kk::ui::Color *) color {
    if(color == nullptr) {
        return nil;
    }
    return [UIColor colorWithRed:color->r green:color->g blue:color->b alpha:color->a];
}


+(UIColor *) colorWithKerCString:(const char *) color {
    kk::ui::Color v(color);
    return [self colorWithKerUIColor:&v];
}

@end
