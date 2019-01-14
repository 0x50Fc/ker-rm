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
    
    enum NetStreamStatus {
        NetStreamStatusNone,
        NetStreamStatusReading,
        NetStreamStatusWriting,
        NetStreamStatusError
    };
    
    class NetStream : public Object {
    public:
        NetStream(DispatchQueue * queue,SocketId fd);
        virtual SocketId fd();
        virtual DispatchQueue * queue();
        virtual NetStreamStatus status();
        virtual void setOnError(std::function<void(NetStream *,kk::CString errmsg)> && func);
        virtual void setOnEvent(std::function<void(NetStream *)> && func);
    protected:
        virtual void doError(kk::CString errmsg);
        virtual void doEvent();
        std::function<void(NetStream *)> _onEvent;
        std::function<void(NetStream *,kk::CString errmsg)> _onError;
        kk::Strong<DispatchQueue> _queue;
        SocketId _fd;
        NetStreamStatus _status;
    };
    
    class NetInputStream : public NetStream {
    public:
        NetInputStream(DispatchQueue * queue,SocketId fd);
        virtual ~NetInputStream();
        virtual void setProxy(std::function<ssize_t(NetStream *,void *,size_t)> && func);
        virtual kk::Boolean hasBytesAvailable();
        virtual ssize_t read(void * data,size_t size);
        virtual Buffer & readBuffer();
    protected:
        virtual void doError(kk::CString errmsg);
        virtual void doEvent();
        kk::Strong<DispatchSource> _cb;
        std::function<ssize_t(NetStream *,void *,size_t)> _proxy;
        Buffer _buffer;
    };
    
    class NetOutputStream : public NetStream {
    public:
        NetOutputStream(DispatchQueue * queue,SocketId fd);
        virtual ~NetOutputStream();
        virtual void setProxy(std::function<ssize_t(NetStream *,const void *,size_t)> && func);
        virtual kk::Boolean hasSpaceAvailable();
        virtual ssize_t write(const void * data,size_t size);
    protected:
        virtual void doError(kk::CString errmsg);
        virtual void doEvent();
        kk::Strong<DispatchSource> _cb;
        std::function<ssize_t(NetStream *,const void *,size_t)> _proxy;
        Buffer _buffer;
    };
    
    
    class TCPConnection : public EventEmitter {
    public:
        TCPConnection(kk::CString hostname,kk::Int port);
        TCPConnection(SocketId fd,kk::CString address,kk::Int port,DispatchQueue * queue);
        virtual ~TCPConnection();
        virtual kk::CString address();
        virtual kk::Int port();
        virtual void close();
        virtual void write(Any value);
        virtual void write(const void * data,size_t size);
        virtual kk::Boolean flush();
        virtual SocketId fd();
        virtual NetInputStream * input();
        virtual NetOutputStream * output();
        virtual Buffer & outputBuffer();
        virtual Buffer & inputBuffer();
        virtual DispatchQueue * queue();
        
        static void Openlib();
        
        Ker_CLASS(TCPConnection,EventEmitter,"TCPConnection")
        
    protected:
        virtual void onResolve(struct sockaddr * addr, socklen_t len);
        virtual void openConnection();
        virtual void onWrite();
        virtual void doClose(kk::CString errmsg);
        virtual void doOpen();
        virtual void onRead();
        kk::Int _port;
        kk::String _address;
        SocketId _fd;
        kk::Strong<DispatchQueue> _queue;
        kk::Strong<NetInputStream> _input;
        kk::Strong<NetOutputStream> _output;
        kk::Strong<DispatchSource> _cs;
        kk::Buffer _buffer;
    };
    
    DispatchQueue * NetDispatchQueue();
    
}

#endif /* tcp_hpp */
