//
//  container.h
//  KK
//
//  Created by zhanghailong on 2018/11/8.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#ifndef kk_container_h
#define kk_container_h

#include <core/kk.h>
#include <core/jit.h>
#include <core/dispatch.h>

namespace kk {
    
    class Container {
    public:
        virtual kk::DispatchQueue * queue() = 0;
        virtual void set(kk::Object * object) = 0;
        virtual kk::Object * get(kk::Object * object) = 0;
        virtual void remove(kk::Object * object) = 0;
    };
    
}


#endif /* container_h */
