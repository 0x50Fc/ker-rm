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
        static void get(kk::CString hostname,kk::Int port,std::function<void(struct sockaddr *,socklen_t)> && func);
    };
    
    class TCPConnection;
    
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
        virtual kk::Strong<TCPConnection> newConnection(SocketId fd,kk::CString address,kk::Int port);
        virtual void doClose(kk::CString errmsg);
        kk::Int _port;
        SocketId _fd;
        kk::Strong<DispatchQueue> _queue;
        kk::Strong<DispatchSource> _source;
        kk::Int _AF;
    };
    
    enum NetBufferFlushState {
        NetBufferFlushStateDone = 0,NetBufferFlushStateContinue = 1,NetBufferFlushStateError = -1
    };
    
    class NetBuffer : public Object {
    public:
        NetBuffer(SocketId fd);
        virtual ~NetBuffer();
        virtual kk::Buffer & input();
        virtual kk::Buffer & output();
        virtual SocketId fd();
        virtual void write(const void * data,size_t size);
        virtual NetBufferFlushState flush();
        virtual ssize_t read(void * data,size_t size);
    protected:
        SocketId _fd;
        kk::Buffer _input;
        kk::Buffer _output;
    };
    
    class NetEventBuffer : public Object {
    public:
        NetEventBuffer(DispatchQueue * queue,SocketId fd);
        virtual ~NetEventBuffer();
        virtual DispatchQueue * queue();
        virtual Boolean isReading();
        virtual void startReading();
        virtual void stopReading();
        virtual Boolean isWriting();
        virtual void startWriting();
        virtual void stopWriting();
        virtual void setCB(std::function<void(NetEventBuffer *)> && onRead,std::function<void(NetEventBuffer *)> && onWrite,std::function<void(NetEventBuffer *,kk::CString errmsg)> && onError);
        virtual void setOnSend(std::function<ssize_t(NetEventBuffer *,const void *,size_t)> && onSend);
        virtual void setOnRecv(std::function<ssize_t(NetEventBuffer *,void *,size_t)> && onRecv);
        virtual kk::Buffer & input();
        virtual kk::Buffer & output();
        virtual SocketId fd();
        virtual kk::CString errmsg();
    protected:
        
        virtual ssize_t send(const void * data,size_t size);
        virtual ssize_t recv(void *data,size_t size);
        
        virtual void onReading();
        virtual void onWriting();
        
        virtual void doError(kk::CString errmsg);
        
        std::function<ssize_t(NetEventBuffer *,const void *,size_t)> _onSend;
        std::function<ssize_t(NetEventBuffer *,void *,size_t)> _onRecv;
        
        std::function<void(NetEventBuffer *)> _onRead;
        std::function<void(NetEventBuffer *)> _onWrite;
        std::function<void(NetEventBuffer *,kk::CString errmsg)> _onError;
        kk::Strong<DispatchQueue> _queue;
        kk::Strong<DispatchSource> _rd;
        kk::Strong<DispatchSource> _wd;
        SocketId _fd;
        kk::Boolean _reading;
        kk::Boolean _writing;
        kk::Buffer _input;
        kk::Buffer _output;
        kk::String _errmsg;
    };
    
    class TCPConnection : public EventEmitter {
    public:
        TCPConnection(kk::CString hostname,kk::Int port);
        TCPConnection(SocketId fd,kk::CString address,kk::Int port,DispatchQueue * queue);
        virtual ~TCPConnection();
        virtual kk::CString address();
        virtual kk::Int port();
        virtual void close();
        virtual void read();
        virtual void write(Any value);
        virtual void write(const void * data,size_t size);
        virtual SocketId fd();
        virtual NetEventBuffer * buffer();
        virtual DispatchQueue * queue();
        
        static void Openlib();
        
        Ker_CLASS(TCPConnection,EventEmitter,"TCPConnection")
        
    protected:
        virtual void onResolve(struct sockaddr * addr, socklen_t len);
        virtual void openConnection(struct sockaddr * addr,socklen_t len);
        virtual void onOpen();
        virtual void onOpen(std::function<ssize_t(NetEventBuffer *,const void *,size_t)> && onSend,std::function<ssize_t(NetEventBuffer *,void *,size_t)> && onRecv);
        virtual void onWrite();
        virtual void doClose(kk::CString errmsg);
        virtual void onRead();
        kk::Int _port;
        kk::String _address;
        SocketId _fd;
        kk::Strong<DispatchQueue> _queue;
        kk::Strong<NetEventBuffer> _buffer;
    };
    
    DispatchQueue * NetDispatchQueue();
    
}

#endif /* tcp_hpp */
