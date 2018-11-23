//
//  UIFont+Ker.h
//  Ker
//
//  Created by hailong11 on 2018/11/21.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>

#if defined(__cplusplus)

#include <ui/ui.h>

#endif

@interface UIFont (Ker)

#if defined(__cplusplus)

+(UIFont *) fontWithKerUIFont:(kk::ui::Font *) font;

#endif

+(UIFont *) fontWithKerCString:(const char *) font;

@end
