//
//  net.h
//  Ker
//
//  Created by zhanghailong on 2019/1/8.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef kk_net_h
#define kk_net_h

#include <core/kk.h>
#include <core/jit.h>
#include <core/event.h>
#include <core/dispatch.h>
#include <netinet/in.h>

namespace kk {

    typedef kk::Int SocketId;
    
    class DNSResolve {
    public:
        static void set(kk::CString hostname,kk::CString ipaddress);
        static void get(kk::CString hostname,std::function<void(struct sockaddr *,socklen_t)> && func);
    };
    
    class TCPClient;
    
    class TCPServer : public EventEmitter {
    public:
        TCPServer(kk::Int port,kk::CString address);
        virtual ~TCPServer();
        virtual kk::Int port();
        virtual void close();
        static void Openlib();
        Ker_CLASS(TCPServer,EventEmitter,"TCPServer")
    protected:
        virtual void onAccept();
        virtual kk::Strong<TCPClient> newClient(SocketId fd,kk::CString address,kk::Int port);
        virtual void doClose(kk::CString errmsg);
        kk::Int _port;
        SocketId _fd;
        kk::Strong<DispatchQueue> _queue;
        kk::Strong<DispatchSource> _source;
        kk::Int _AF;
    };
    
    class TCPClient : public EventEmitter {
    public:
        TCPClient(kk::CString hostname,kk::Int port);
        TCPClient(SocketId fd,kk::CString address,kk::Int port,DispatchQueue * queue);
        virtual ~TCPClient();
        virtual kk::CString address();
        virtual kk::Int port();
        virtual void close();
        virtual kk::Int write(Any & value);
        virtual kk::Int write(void * data,size_t size);
        
        static void Openlib();
        
        Ker_CLASS(TCPClient,EventEmitter,"TCPClient")
        
    protected:
    
        virtual void onResolve(struct sockaddr * addr);
        virtual void onRead();
        virtual void onWrite();
        virtual void doClose(kk::CString errmsg);
        virtual void doData(void * data,size_t size);
        kk::Int _port;
        kk::String _address;
        SocketId _fd;
        kk::Strong<DispatchQueue> _queue;
        kk::Strong<DispatchSource> _rd;
        kk::Strong<DispatchSource> _wd;
    };
    
    DispatchQueue * NetDispatchQueue();
    
}

#endif /* tcp_hpp */
