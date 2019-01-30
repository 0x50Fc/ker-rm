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
        virtual kk::String MD5(const void * data, size_t size);
        virtual kk::String MD5(kk::CString v);
        virtual void encodeBASE64(const void * data, size_t size,Buffer & buffer);
        virtual kk::Boolean decodeBASE64(kk::CString v, size_t length, Buffer & buffer);
        virtual kk::String encodeBASE64(Any data);
        virtual kk::Strong<ArrayBuffer> decodeBASE64(kk::CString v);
        Ker_CLASS(Crypto,Object,"Crypto")
        
        static void Openlib();
        
    };
    
}

#endif /* crypto_h */
