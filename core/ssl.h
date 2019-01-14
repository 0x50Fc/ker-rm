//
//  ssl.h
//  Ker
//
//  Created by zhanghailong on 2019/1/10.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef kk_ssl_h
#define kk_ssl_h

#include <core/net.h>

namespace kk {

    class SSLConnection : public TCPConnection {
    public:
        
        SSLConnection(kk::CString hostname,kk::Int port);
        
        static void Openlib();
        
        Ker_CLASS(SSLConnection,TCPConnection,"SSLConnection")
        
        virtual kk::Boolean isSSLConnected();
        
    protected:
        virtual void openConnection();
        virtual void openSSLConnection(std::function<ssize_t(NetStream *,void *,size_t)> && onRead,std::function<ssize_t(NetStream *,const void *,size_t)> && onWrite);
        kk::String _hostname;
        kk::Boolean _SSLConnected;
    };
    
    
    
}

#endif /* ssl_h */
