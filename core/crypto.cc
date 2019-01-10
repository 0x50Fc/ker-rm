//
//  crypto.cc
//  Ker
//
//  Created by zhanghailong on 2018/12/13.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//


#include <core/crypto.h>

#if defined(__APPLE__)

#include <CommonCrypto/CommonCrypto.h>

#elif defined(__ANDROID_API__)

#include <md5.h>

#else

#include <openssl/md5.h>

#endif


namespace kk {

    Crypto::Crypto() {
        
    }
    
    Crypto::~Crypto() {
        
    }
    
    
#if defined(__APPLE__)
    
    kk::String Crypto::MD5(kk::CString string) {
        
        CC_MD5_CTX m;
        
        CC_MD5_Init(&m);
        
        if(string) {
            CC_MD5_Update(&m, string, (CC_LONG) strlen(string));
        }
        
        unsigned char md[16];
        
        CC_MD5_Final(md, &m);
        
        char s[40] = "";
        
        snprintf(s, sizeof(s), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
                 ,md[0],md[1],md[2],md[3],md[4],md[5],md[6],md[7]
                 ,md[8],md[9],md[10],md[11],md[12],md[13],md[14],md[15]);
        
        return s;
    }
#elif defined(__ANDROID_API__)
    
    kk::String Crypto::MD5(kk::CString string) {
        
        md5_state_t m;
        
        md5_init(&m);
        
        if(string) {
            md5_append(&m, (md5_byte_t *) string, (size_t) strlen(string));
        }
        
        md5_byte_t md[16];
        
        md5_finish(&m,md);
        
        char s[40] = "";
        
        snprintf(s, sizeof(s), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
                 ,md[0],md[1],md[2],md[3],md[4],md[5],md[6],md[7]
                 ,md[8],md[9],md[10],md[11],md[12],md[13],md[14],md[15]);
        
        return s;
    }
    
#else
    
    kk::String Crypto::MD5(kk::CString string) {
        
        MD5_CTX m;
        
        MD5_Init(&m);
        
        if(string) {
            MD5_Update(&m, string, (size_t) strlen(string));
        }
        
        unsigned char md[16];
        
        MD5_Final(md, &m);
        
        char s[40] = "";
        
        snprintf(s, sizeof(s), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
                 ,md[0],md[1],md[2],md[3],md[4],md[5],md[6],md[7]
                 ,md[8],md[9],md[10],md[11],md[12],md[13],md[14],md[15]);
        
        return s;
    }
    
#endif

    void Crypto::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushInterface<Crypto>(ctx, [](duk_context * ctx)->void{
                
                kk::PutMethod<Crypto,kk::String,kk::CString>(ctx, -1, "MD5", &Crypto::MD5);
                
            });
            
            kk::PushObject(ctx, new Crypto());
            duk_put_global_string(ctx, "crypto");
            
        });
        
    }
    
}

