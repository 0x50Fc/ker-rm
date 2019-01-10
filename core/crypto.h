//
//  crypto.h
//  Ker
//
//  Created by zhanghailong on 2018/12/13.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#ifndef kk_crypto_h
#define kk_crypto_h

#include <core/kk.h>
#include <core/jit.h>

namespace kk {
    
    class Crypto : public Object {
    public:
        Crypto();
        virtual ~Crypto();
        virtual kk::String MD5(kk::CString v);
        
        Ker_CLASS(Crypto,Object,"Crypto")
        
        static void Openlib();
        
    };
    
}

#endif /* crypto_h */
