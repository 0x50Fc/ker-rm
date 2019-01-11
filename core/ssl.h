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
        
    protected:
        virtual void openConnection(struct sockaddr * addr,socklen_t len);
        kk::String _hostname;
    };
    
    
    
}

#endif /* ssl_h */
