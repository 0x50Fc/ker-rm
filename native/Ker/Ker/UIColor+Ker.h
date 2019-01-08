//
//  UIColor+Ker.h
//  Ker
//
//  Created by zhanghailong on 2018/11/21.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>

#if defined(__cplusplus)

#include <ui/ui.h>

#endif

@interface UIColor (Ker)

#if defined(__cplusplus)

+(UIColor *) colorWithKerUIColor:(kk::ui::Color *) color;

#endif

+(UIColor *) colorWithKerCString:(const char *) color;


@end

