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
    
    
    static const kk::Ubyte Crypto_base64_enctab[64] = {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,  /* A...P */
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,  /* Q...f */
        0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,  /* g...v */
        0x77, 0x78, 0x79, 0x7a, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2b, 0x2f   /* w.../ */
    };
    
    static const kk::Byte Crypto_base64_dectab[256] = {
        /* -1 = error, -2 = allowed whitespace, -3 = padding ('='), 0...63 decoded bytes */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -1, -1, -2, -1, -1,  /* 0x00...0x0f */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 0x10...0x1f */
        -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,  /* 0x20...0x2f */
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -3, -1, -1,  /* 0x30...0x3f */
        -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,  /* 0x40...0x4f */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,  /* 0x50...0x5f */
        -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,  /* 0x60...0x6f */
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,  /* 0x70...0x7f */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 0x80...0x8f */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 0x90...0x9f */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 0xa0...0xaf */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 0xb0...0xbf */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 0xc0...0xcf */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 0xd0...0xdf */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 0xe0...0xef */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1   /* 0xf0...0xff */
    };
    
    void Crypto::encodeBASE64(const void * data, size_t size,Buffer & buffer) {
        
        kk::Uint t;
        size_t n_full, n_full3, n_final;
        const kk::Ubyte *src_end_fast;
        kk::Ubyte * src = (kk::Ubyte *) data;
        
        n_full = size / 3;  /* full 3-byte -> 4-char conversions */
        n_full3 = n_full * 3;
        n_final = size - n_full3;
        
        buffer.capacity(buffer.byteLength() + (kk::Uint) n_full * 4 + 5);
        kk::Byte * dst = (kk::Byte *) buffer.data() + buffer.byteLength();
        kk::Byte * dst_s = dst;
        
        src_end_fast = (kk::Ubyte *) data + n_full3;
        
        while (src != src_end_fast) {
            t = (kk::Uint) (*src++);
            t = (t << 8) + (kk::Uint) (*src++);
            t = (t << 8) + (kk::Uint) (*src++);
            
            *dst++ = Crypto_base64_enctab[t >> 18];
            *dst++ = Crypto_base64_enctab[(t >> 12) & 0x3f];
            *dst++ = Crypto_base64_enctab[(t >> 6) & 0x3f];
            *dst++ = Crypto_base64_enctab[t & 0x3f];
            
        }
        
        switch (n_final) {
                /* case 0: nop */
            case 1: {
                /* XX== */
                t = (kk::Uint) (*src++);
                *dst++ = Crypto_base64_enctab[t >> 2];           /* XXXXXX-- */
                *dst++ = Crypto_base64_enctab[(t << 4) & 0x3f];  /* ------XX */
                *dst++ = '=';
                *dst++ = '=';
                break;
            }
            case 2: {
                /* XXX= */
                t = (kk::Uint) (*src++);
                t = (t << 8) + (kk::Uint) (*src++);
                *dst++ = Crypto_base64_enctab[t >> 10];          /* XXXXXX-- -------- */
                *dst++ = Crypto_base64_enctab[(t >> 4) & 0x3f];  /* ------XX XXXX---- */
                *dst++ = Crypto_base64_enctab[(t << 2) & 0x3f];  /* -------- ----XXXX */
                *dst++ = '=';
                break;
            }
        }
        
        buffer.setByteLength(buffer.byteLength() + (kk::Uint) (dst - dst_s));
        
    }
    
    kk::Boolean Crypto::decodeBASE64(kk::CString v,size_t length, Buffer & buffer) {
        
        kk::Int x;
        kk::Int t;
        kk::Uint n_equal;
        kk::Uint n_chars;
        const kk::Ubyte *src_end;
        const kk::Ubyte *src_end_safe;
        kk::Ubyte * src = (kk::Ubyte *) v;
        src_end = src + length;
        src_end_safe = src_end - 4;

        buffer.capacity( buffer.byteLength() + (kk::Uint) ((length * 3 / 4) + 5));
        
        kk::Byte * dst = (kk::Byte *) buffer.data() + buffer.byteLength();
        kk::Byte * dst_s = dst;
        
        for (;;) {

            while (src <= src_end_safe) {
                
                t = (kk::Int) Crypto_base64_dectab[*src++];
                t = (t << 6) | (kk::Int) Crypto_base64_dectab[*src++];
                t = (t << 6) | (kk::Int) Crypto_base64_dectab[*src++];
                t = (t << 6) | (kk::Int) Crypto_base64_dectab[*src++];
                
                if (t < 0) {
                    src -= 4;
                    break;
                }
                
                *dst++ = (kk::Ubyte) (t >> 16);
                *dst++ = (kk::Ubyte) ((t >> 8) & 0xff);
                *dst++ = (kk::Ubyte) (t & 0xff);
            }
            
            n_equal = 0;
            n_chars = 0;
            t = 0;
            for (;;) {
                
                if ((src >= src_end)) {
                    goto done;  /* two level break */
                }
                
                x = Crypto_base64_dectab[*src++];
                
                if ((x < 0)) {
                    if (x == -2) {
                        continue;  /* allowed ascii whitespace */
                    } else if (x == -3) {
                        n_equal++;
                        t <<= 6;
                    } else {
                        goto decode_error;
                    }
                } else {
                    if (n_equal > 0) {
                        goto decode_error;
                    }
                    t = (t << 6) + x;
                }
                
                if (n_chars == 3) {
                    *dst++ = (kk::Ubyte) (t >> 16);
                    *dst++ = (kk::Ubyte) ((t >> 8) & 0xff);
                    *dst++ = (kk::Ubyte) (t & 0xff);
                    
                    if ((n_equal > 0)) {
                        if (n_equal == 1) {
                            /* XXX= */
                            dst -= 1;
                        } else if (n_equal == 2) {
                            /* XX== */
                            dst -= 2;
                        } else {
                            goto decode_error;  /* invalid padding */
                        }

                    }
                    break;  /* back to fast loop */
                } else {
                    n_chars++;
                }
            }
        }
    done:
        
        if (n_chars != 0) {
            goto decode_error;
        }
        
        buffer.setByteLength(buffer.byteLength() + (kk::Uint) (dst - dst_s));
        
        return true;
        
    decode_error:
        return false;
    }
    
    kk::String Crypto::encodeBASE64(kk::Any data) {
        
        kk::Buffer b;
        
        if(data.type == TypeString) {
            encodeBASE64(data.stringValue, data.length, b);
        } else if(data.type == TypeObject) {
            ArrayBuffer * a = data;
            if(a != nullptr) {
                encodeBASE64(a->data(), a->byteLength(), b);
            }
        }
        
        return kk::String((kk::Byte *) b.data(),b.byteLength());
    }
    
    kk::Strong<ArrayBuffer> Crypto::decodeBASE64(kk::CString v) {
        if(v == nullptr) {
            return nullptr;
        }
        Buffer b;
        decodeBASE64(v,strlen(v) ,b);
        if(b.byteLength() > 0) {
            return new ArrayBuffer(b.data(),b.byteLength());
        }
        return nullptr;
    }
    
    kk::String Crypto::MD5(kk::CString v) {
        return MD5((const void *) v,v == nullptr ? 0 : (size_t) strlen(v));
    }
    
#if defined(__APPLE__)
    
    kk::String Crypto::MD5(const void * data,size_t size) {
        
        CC_MD5_CTX m;
        
        CC_MD5_Init(&m);
        
        if(data && size > 0) {
            CC_MD5_Update(&m, data, (CC_LONG) size);
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
    
    kk::String Crypto::MD5(const void * data,size_t size) {
        
        md5_state_t m;
        
        md5_init(&m);
        
        if(data && size > 0) {
            md5_append(&m, (md5_byte_t *) data, (size_t) size);
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
    
    kk::String Crypto::MD5(const void * data,size_t size) {
        
        MD5_CTX m;
        
        MD5_Init(&m);
        
        if(data && size > 0) {
            MD5_Update(&m, data, size);
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
                kk::PutMethod<Crypto,kk::String,kk::Any>(ctx, -1, "encodeBASE64", &Crypto::encodeBASE64);
                
            });
            
            kk::PushObject(ctx, new Crypto());
            duk_put_global_string(ctx, "crypto");
            
        });
        
    }
    
}

