//
//  UI.h
//  KK
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef UI_h
#define UI_h

namespace kk {
    
    namespace ui {
        
        class CGImageGetter {
        public:
            virtual CGImageRef CGImage() = 0;
        };
        
        CGImageRef GetCGImage(Image * image);
        
    }
    
}

#endif /* UI_h */
