//
//  debugger.h
//  Ker
//
//  Created by zhanghailong on 2019/1/7.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef kk_debugger_h
#define kk_debugger_h

#include <core/kk.h>
#include <core/jit.h>

namespace kk {
    
    namespace Debugger {
        
        kk::Boolean start(kk::Int port);
        void stop();
        kk::Boolean debug(duk_context * ctx);
        
    }
    
}

#endif /* debugger_h */
