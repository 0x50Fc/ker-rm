//
//  net.cc
//  Ker
//
//  Created by zhanghailong on 2019/1/8.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "net.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <unistd.h>

namespace kk {
    
    static std::map<kk::String,struct sockaddr *> gDNSResolveAddress;
    
    void DNSResolve::set(kk::CString hostname,kk::CString ipaddress) {
        
        if(hostname == nullptr) {
            return ;
        }
        
        if(ipaddress == nullptr) {
            kk::String s(hostname);
            NetDispatchQueue()->async([s]()->void{
                auto i = gDNSResolveAddress.find(s);
                if(i != gDNSResolveAddress.end()) {
                    gDNSResolveAddress.erase(i);
                }
            });
        } else {
            kk::String k(hostname);
            kk::String v(ipaddress);
            NetDispatchQueue()->async([k,v]()->void{
                struct sockaddr * a;
                if(kk::CStringHasSubstring(v.c_str(), ":")) {
                    struct sockaddr_in6 * addr = (struct sockaddr_in6 * ) malloc(sizeof(struct sockaddr_in6));
                    memset(addr,0,sizeof(struct sockaddr_in6));
                    inet_pton(AF_INET6, v.c_str(), &addr->sin6_addr);
                    addr->sin6_family = AF_INET6;
                    a = (sockaddr *) addr;
                } else {
                    struct sockaddr_in * addr = (struct sockaddr_in * ) malloc(sizeof(struct sockaddr_in));
                    memset(addr,0,sizeof(struct sockaddr_in));
                    addr->sin_addr.s_addr = inet_addr(v.c_str());
                    addr->sin_family = AF_INET;
                    a = (sockaddr *) addr;
                }
                auto i = gDNSResolveAddress.find(k);
                if(i != gDNSResolveAddress.end()) {
                    free(i->second);
                }
                gDNSResolveAddress[k] = a;
            });
        }
        
    }
    
    void DNSResolve::get(kk::CString hostname,kk::Int port,std::function<void(struct sockaddr *,socklen_t)> && func) {
        
        kk::String v;
        
        if(hostname != nullptr) {
            v.append(hostname);
        }
        
        NetDispatchQueue()->async([v,func,port]()->void{
            
            auto i = gDNSResolveAddress.find(v);
            struct sockaddr * a = nullptr;
            
            if(i == gDNSResolveAddress.end()) {
                
                struct addrinfo *res, *cur;
                int ret;
                struct sockaddr *addr = nullptr;
                struct addrinfo hint;
                
                memset(&hint, 0, sizeof(struct addrinfo));
                
                hint.ai_flags = AI_ADDRCONFIG;
                hint.ai_socktype = SOCK_STREAM;
                hint.ai_protocol = IPPROTO_TCP;
                
                char p[64];
                
                snprintf(p, sizeof(p), "%d",port);
                
                ret = getaddrinfo(v.c_str(), p,&hint ,&res);
                
                if (ret != -1) {
                    
                    for (cur = res; cur != nullptr; cur = cur->ai_next) {
                        addr = (struct sockaddr *)cur->ai_addr;
                        if(addr->sa_family ==AF_INET) {
                            kk::Log("[DNSResolve] [IP] %s => %s",v.c_str(),inet_neta(ntohl(((struct sockaddr_in *) addr)->sin_addr.s_addr), p, sizeof(p)));
                        } else {
                            kk::Log("[DNSResolve] [IP] %s => %s",v.c_str(),inet_ntop(addr->sa_family, &addr, p, sizeof(p)));
                        }
                        
                    }
                    
                    for (cur = res; cur != nullptr; cur = cur->ai_next) {
                        addr = (struct sockaddr *)cur->ai_addr;
                        if(addr->sa_family == AF_INET) {
                            break;
                        }
                    }
                    
                    if(cur == nullptr) {
                        for (cur = res; cur != nullptr; cur = cur->ai_next) {
                            addr = (struct sockaddr *)cur->ai_addr;
                            if(addr->sa_family == AF_INET6) {
                                break;
                            }
                        }
                    }
                    
                    if(cur != nullptr && addr != nullptr) {
                        if(addr->sa_family == AF_INET) {
                            a = (struct sockaddr *) malloc(sizeof(struct sockaddr_in));
                            memcpy(a,addr,sizeof(struct sockaddr_in));
                        } else if(addr->sa_family == AF_INET6) {
                            a = (struct sockaddr *) malloc(sizeof(struct sockaddr_in6));
                            memcpy(a,addr,sizeof(struct sockaddr_in6));
                        }
                    }
                    
                    freeaddrinfo(res);
                    
                    
                }
                
                if(a != nullptr) {
                    gDNSResolveAddress[v] = a;
                }
                
            } else {
                a = i->second;
            }
            
            if(a == nullptr) {
                func(nullptr,0);
            } else if(a->sa_family == AF_INET6) {
                ((struct sockaddr_in6 *) a)->sin6_port = htons(port);
                func(a,sizeof(struct sockaddr_in6));
            } else if(a->sa_family == AF_INET) {
                ((struct sockaddr_in *) a)->sin_port = htons(port);
                func(a,sizeof(struct sockaddr_in));
            } else {
                func(nullptr,0);
            }
        });
        
    }
    
    TCPServer::TCPServer(kk::Int port,kk::CString address):_port(port),_queue(getCurrentDispatchQueue()) {
        
        _AF = AF_INET;
        
        if(address != nullptr) {
            if(kk::CStringHasSubstring(address, ":")) {
                _AF = AF_INET6;
            }
        }
        
        _fd = socket(_AF,SOCK_STREAM, 0);
        
        if(_fd == -1) {
            doClose("Socket Create Error");
            return;
        }
        
        if(-1 == fcntl(_fd, F_SETFL, O_NONBLOCK)) {
            ::close(_fd);
            _fd = -1;
            doClose("Socket Nonblock Error");
            return;
        }
        
        {
            int opt = 1;
            if (-1 == setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
                ::close(_fd);
                _fd = -1;
                doClose("Socket ReuseAddr Error");
                return;
            }
        }
        
        {
            int opt = 1;
            if (-1 == setsockopt(_fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt))) {
                ::close(_fd);
                _fd = -1;
                doClose("Socket No-SIGPIPE Error");
                return;
            }
        }
        
        
        if(_AF == AF_INET) {
            struct sockaddr_in addr = {0};
            
            addr.sin_family = AF_INET;
            
            if(address != nullptr) {
                addr.sin_addr.s_addr = inet_addr(address);
            } else {
                addr.sin_addr.s_addr = htonl(INADDR_ANY);
            }
            
            addr.sin_port = htons(port);
            
            if (-1 == bind(_fd, (const struct sockaddr *)&addr, (socklen_t) sizeof(addr))){
                ::close(_fd);
                _fd = -1;
                doClose("Socket Bind Error");
                return;
            }
            
        } else {
            
            struct sockaddr_in6 addr = {0};
            
            addr.sin6_family = AF_INET6;
            
            if(address != nullptr) {
                inet_pton(AF_INET6, address, &addr.sin6_addr);
            }
            
            addr.sin6_port = htons(port);
            
            if (-1 == bind(_fd, (const struct sockaddr *)&addr, (socklen_t) sizeof(addr))){
                ::close(_fd);
                _fd = -1;
                doClose("Socket Bind Error");
                return;
            }
        }
        
        if (-1 == listen(_fd, 409600)) {
            ::close(_fd);
            _fd = -1;
            doClose("Socket Listener Error");
            return;
        }
        
        _source = createDispatchSource((Uint64) _fd,DispatchSourceTypeRead,_queue);
        _source->setEvent([this]()->void{
            this->onAccept();
        });
        _source->resume();
    }
    
    void TCPServer::onAccept() {
        
        if(_fd == -1) {
            return;
        }
        
        kk::Strong<TCPConnection> conn;
        
        if(_AF == AF_INET) {
            
            struct sockaddr_in addr = {0};
            
            socklen_t len = sizeof(addr);
            
            SocketId fd = accept(_fd, (struct sockaddr *) & addr, &len);
            
            if(-1 == fd) {
                close();
                doClose("Socket Accept Error");
                return;
            } else {
                conn = newConnection(fd,inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
            }
            
        } else {
            
            struct sockaddr_in6 addr = {0};
            
            socklen_t len = sizeof(addr);
            
            SocketId fd = accept(_fd, (struct sockaddr *) & addr, &len);
            
            if(-1 == fd) {
                close();
                doClose("Socket Accept Error");
                return;
            } else {
                char address[255] = {0};
                inet_ntop(AF_INET6, &addr.sin6_addr, address, sizeof(address));
                conn = newConnection(fd,address,ntohs(addr.sin6_port));
            }
            
        }
        
        kk::Strong<kk::Event> e = new kk::Event();
        e->setData(conn);
        emit("accept", e);
    }
    
    void TCPServer::doClose(kk::CString errmsg) {

        kk::Strong<kk::Event> e = new kk::Event();
        
        if(errmsg != nullptr) {
            e->setData(new TObject<kk::String, kk::String>({{"errmsg",errmsg}}));
        }
        
        emit("error", e);
    
    }
    
    kk::Strong<TCPConnection> TCPServer::newConnection(SocketId fd,kk::CString address,kk::Int port) {
        return new TCPConnection(fd,address,port,_queue);
    }
    
    TCPServer::~TCPServer() {
        
        if(_source != nullptr) {
            _source->cancel();
        }
        
        if(_fd != -1) {
            shutdown(_fd, SHUT_RDWR);
            ::close(_fd);
        }
        
    }
    
    void TCPServer::close() {
        
        if(_source != nullptr) {
            _source->cancel();
            _source = nullptr;
        }
        
        if(_fd != -1) {
            shutdown(_fd, SHUT_RDWR);
            ::close(_fd);
            _fd = -1;
        }
        
    }
    
    kk::Int TCPServer::port() {
        return _port;
    }
    
    void TCPServer::Openlib() {
 
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<TCPServer,kk::Int,kk::CString>(ctx, [](duk_context * ctx)->void{
                kk::PutProperty<TCPServer,kk::Int>(ctx, -1, "port", &TCPServer::port);
                kk::PutMethod<TCPServer,void>(ctx, -1, "close", &TCPServer::close);
            });
            
        });
        
    }
    
    NetBuffer::NetBuffer(SocketId fd):_fd(fd) {
        
    }
    
    NetBuffer::~NetBuffer() {
        
    }
    
    kk::Buffer & NetBuffer::input() {
        return _input;
    }
    
    kk::Buffer & NetBuffer::output() {
        return _output;
    }
    
    SocketId NetBuffer::fd() {
        return _fd;
    }
    
    void NetBuffer::write(const void * data,size_t size) {
        _output.append(data, (kk::Uint) size);
    }
    
    NetBufferFlushState NetBuffer::flush() {
        
        if(_output.byteLength() == 0){
            return NetBufferFlushStateDone;
        }
        
        ssize_t n;
        
        do {
            n = ::write(_fd, _output.data(), _output.byteLength());
            if(n == -1 && errno == EAGAIN) {
                continue;
            }
        } while(0);
        
        if(n == -1) {
            return NetBufferFlushStateError;
        } else if(n == 0) {
            return NetBufferFlushStateContinue;
        } else {
            _output.drain((kk::Uint) n);
        }
        
        if(_output.byteLength() == 0){
            return NetBufferFlushStateDone;
        }
        
        return NetBufferFlushStateContinue;
    }
    
    ssize_t NetBuffer::read(void * data,size_t size) {
        
        ssize_t n = size;
        
        if(_input.byteLength() < size) {
            
            _input.capacity(_input.byteLength() + 2048);
            
            do {
                n = ::read(_fd, _input.data() + _input.byteLength(), _input.size() - _input.byteLength());
                if(n == -1 && errno == EAGAIN){
                    continue;
                }
            } while (0);
            
            if(n == -1) {
                return n;
            } else if(n == 0) {
                return n;
            } else {
                _input.setByteLength(_input.byteLength() + (kk::Uint) n);
                if(n > size) {
                    n = size;
                }
            }
        }
        
        memcpy(data, _input.data(), n);
        
        _input.drain((kk::Uint) n);
        
        return n;
    }
    
    NetEventBuffer::NetEventBuffer(DispatchQueue * queue,SocketId fd):_fd(fd),_queue(queue),_writing(false),_reading(false) {
        _rd = createDispatchSource(_fd, DispatchSourceTypeRead, queue);
        _rd->setEvent([this]()->void{
            this->onReading();
        });
        _wd = createDispatchSource(_fd, DispatchSourceTypeWrite, queue);
        _wd->setEvent([this]()->void{
            this->onWriting();
        });
    }
    
    void NetEventBuffer::setOnSend(std::function<ssize_t(NetEventBuffer *,const void *,size_t)> && onSend) {
        _onSend = onSend;
    }
    
    void NetEventBuffer::setOnRecv(std::function<ssize_t(NetEventBuffer *,void *,size_t)> && onRecv) {
        _onRecv = onRecv;
    }
    
    NetEventBuffer::~NetEventBuffer() {
        _rd->cancel();
        _wd->cancel();
    }
    
    kk::CString NetEventBuffer::errmsg() {
        if(_errmsg.empty()) {
            return nullptr;
        }
        return _errmsg.c_str();
    }
    
    ssize_t NetEventBuffer::send(const void * data,size_t size) {
        if(_onSend != nullptr) {
            return _onSend(this,data,size);
        }
        
        do {
            ssize_t r = write(_fd, data, size);
            if(r == -1 && errno == EAGAIN) {
                continue;
            }
            return r;
        } while (1) ;
    }
    
    ssize_t NetEventBuffer::recv(void *data,size_t size) {
        if(_onRecv != nullptr) {
            return _onRecv(this,data,size);
        }
        do {
            ssize_t r = ::read(_fd, data, size);
            if(r == -1 && errno == EAGAIN) {
                continue;
            }
            return r;
        } while (1) ;
    }
    
    void NetEventBuffer::onReading() {
        
        _input.capacity(_input.byteLength() + 2048);
        
        ssize_t r = recv(_input.data() + _input.byteLength(), _input.size() - _input.byteLength());
        
        if(r == -1) {
            doError("Error in onReading");
        } else {
            _input.setByteLength(_input.byteLength() + (kk::Uint) r);
            if(_onRead != nullptr) {
                _onRead(this);
            } else {
                stopReading();
            }
        }
    }
    
    void NetEventBuffer::onWriting() {
        if(_output.byteLength() > 0) {
            ssize_t r = send(_output.data(), _output.byteLength());
            if(r == -1) {
                doError("Error in onWriting");
            } else {
                _output.drain((kk::Uint) r);
            }
        } else if(_onWrite != nullptr) {
            _onWrite(this);
        } else {
            stopWriting();
        }
    }
    
    void NetEventBuffer::doError(kk::CString errmsg) {
        _errmsg = errmsg;
        stopReading();
        stopWriting();
        if(_onError != nullptr) {
            _onError(this,errmsg);
        }
    }
    
    DispatchQueue * NetEventBuffer::queue(){
        return _queue;
    }
    
    SocketId NetEventBuffer::fd() {
        return _fd;
    }
    
    Boolean NetEventBuffer::isReading() {
        return _reading;
    }
    
    void NetEventBuffer::startReading() {
        if(_reading) {
            return;
        }
        _reading = true;
        _rd->resume();
    }
    
    void NetEventBuffer::stopReading() {
        if(!_reading) {
            return ;
        }
        _reading = false;
        _rd->suspend();
    }
    
    Boolean NetEventBuffer::isWriting() {
        return _writing;
    }
    
    void NetEventBuffer::startWriting() {
        if(_writing) {
            return;
        }
        _writing = true;
        _wd->resume();
    }
    
    void NetEventBuffer::stopWriting() {
        if(!_writing) {
            return ;
        }
        _writing = false;
        _wd->suspend();
    }
    
    kk::Buffer & NetEventBuffer::input() {
        return _input;
    }
    
    kk::Buffer & NetEventBuffer::output() {
        return _output;
    }
    
    void NetEventBuffer::setCB(std::function<void(NetEventBuffer *)> && onRead,std::function<void(NetEventBuffer *)> && onWrite,std::function<void(NetEventBuffer *,kk::CString errmsg)> && onError) {
        _onRead = onRead;
        _onWrite = onWrite;
        _onError = onError;
    }
    
    TCPConnection::TCPConnection(kk::CString hostname,kk::Int port):_fd(-1),_queue(getCurrentDispatchQueue()),_port(0){
        kk::Weak<TCPConnection> v = this;
        DNSResolve::get(hostname, port,[v,port](struct sockaddr * addr,socklen_t len)->void{
            kk::Strong<TCPConnection> vv = v.operator->();
            if(vv != nullptr) {
                if(addr == nullptr) {
                    vv->doClose("DNS Resolve Error");
                } else {
                    vv->onResolve(addr,len);
                }
            }
        });
    }
    
    TCPConnection::TCPConnection(SocketId fd,kk::CString address,kk::Int port,DispatchQueue * queue):_port(port),_queue(queue),_fd(fd),_address(address) {
        _buffer = new NetEventBuffer(queue,fd);
    }
    
    TCPConnection::~TCPConnection() {
        close();
    }
    
    DispatchQueue * TCPConnection::queue() {
        return _queue;
    }
    
    kk::CString TCPConnection::address() {
        return _address.c_str();
    }
    
    kk::Int TCPConnection::port() {
        return _port;
    }
    
    SocketId TCPConnection::fd() {
        return _fd;
    }
    
    void TCPConnection::close() {
        
        if(_buffer != nullptr) {
            _buffer->setOnSend(nullptr);
            _buffer->setOnRecv(nullptr);
            _buffer->setCB(nullptr, nullptr, nullptr);
            _buffer = nullptr;
        }
        
        if(_fd != -1) {
            
            if(-1 != fcntl(_fd, F_SETFL, O_NONBLOCK)) {
                char data[204];
                ssize_t n;
                do {
                    n = ::read(_fd, data, sizeof(data));
                } while(n > 0);
            }
            
            ::close(_fd);
            _fd = -1;
        }
    }
    
    void TCPConnection::onResolve(struct sockaddr * addr, socklen_t len) {
        
        if(addr->sa_family == AF_INET) {
            _port = ntohs(((struct sockaddr_in *) addr)->sin_port);
            char ip[64];
            _address = inet_neta( ntohl(((struct sockaddr_in *) addr)->sin_addr.s_addr), ip, sizeof(ip));
        } else {
            _port = ntohs(((struct sockaddr_in6 *) addr)->sin6_port);
            char ip[64];
            _address = inet_ntop(addr->sa_family, &addr, ip, sizeof(ip));
        }
        
        kk::Log("[IP] %s:%d",_address.c_str(),_port);
        
        _fd = socket(addr->sa_family,SOCK_STREAM, 0);
        
        if(_fd == -1) {
            doClose("Socket Create Error");
            return;
        }
        
        {
            int opt = 1;
            if (-1 == setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
                ::close(_fd);
                _fd = -1;
                doClose("Socket ReuseAddr Error");
                return;
            }
        }
        
        {
            struct sockaddr_in a = {0};
            
            a.sin_family = AF_INET;
            a.sin_addr.s_addr = htonl(INADDR_ANY);
            a.sin_port = htons(0);
            
            if (-1 == ::bind(_fd, (struct sockaddr *) &a, sizeof(struct sockaddr_in))) {
                ::close(_fd);
                _fd = -1;
                doClose("Socket Connect Error");
                return;
            }
        }
        
        openConnection(addr, len);
        
        
    }
    
    void TCPConnection::openConnection(struct sockaddr * addr,socklen_t len) {
        
        if (-1 == ::connect(_fd, addr, len)) {
            close();
            doClose("Socket Connect Error");
            return;
        }
        
        if(-1 == fcntl(_fd, F_SETFL, O_NONBLOCK)) {
            close();
            doClose("Socket Nonblock Error");
            return;
        }
        
        kk::Weak<TCPConnection> v = this;
        _queue->async([v]()->void{
            kk::Strong<TCPConnection> conn = v.operator->();
            if(conn != nullptr) {
                conn->onOpen();
            }
        });
    }
    
    void TCPConnection::onOpen() {
        onOpen(nullptr, nullptr);
    }
    
    void TCPConnection::onOpen(std::function<ssize_t(NetEventBuffer *,const void *,size_t)> && onSend,std::function<ssize_t(NetEventBuffer *,void *,size_t)> && onRecv) {
        
        _buffer = new NetEventBuffer(_queue,_fd);
        
        _buffer->setOnRecv(std::move(onRecv));
        _buffer->setOnSend(std::move(onSend));
        
        _buffer->setCB([this](NetEventBuffer * buffer)->void{
            this->onRead();
        }, [this](NetEventBuffer * buffer)->void{
            this->onWrite();
        }, [this](NetEventBuffer * buffer,kk::CString error)->void{
            this->close();
            this->doClose(error);
        });
        
        kk::Strong<Event> e = new Event();
        emit("open", e);
    }
    
    void TCPConnection::write(Any value) {
        if(value.type == TypeString) {
            write((void *) value.stringValue, value.length);
        } else if(value.type == TypeObject) {
            ArrayBuffer * b = value.objectValue;
            if(b != nullptr) {
                write(b->data(),b->byteLength());
            }
        }
    }
    
    void TCPConnection::write(const void * data,size_t size) {
        if(_buffer == nullptr) {
            return;
        }
        
        Buffer & output = _buffer->output();
        output.append(data,(kk::Uint) size);
        _buffer->startWriting();
    }
    
    void TCPConnection::onWrite(){
        _buffer->stopWriting();
        kk::Strong<Event> e = new Event();
        emit("done", e);
    }
    
    void TCPConnection::onRead() {
        
        _buffer->stopReading();
        
        Buffer & input = _buffer->input();
        
        kk::Strong<Event> e = new Event();
        
        e->setData(new ArrayBuffer(input.data(),input.byteLength()));
        
        input.drain(input.byteLength());
        
        emit("data", e);
        
    }
    
    void TCPConnection::read() {
        if(_buffer != nullptr) {
            kk::Weak<NetEventBuffer> v = (NetEventBuffer *) _buffer;
            _buffer->queue()->async([v]()->void{
                NetEventBuffer * vv = v.operator->();
                if(vv != nullptr) {
                    vv->startReading();
                }
            });
        }
    }
    
    NetEventBuffer * TCPConnection::buffer() {
        return _buffer;
    }
    
    void TCPConnection::doClose(kk::CString errmsg) {
        
        kk::Strong<kk::Event> e = new kk::Event();
        
        if(errmsg != nullptr) {
            e->setData(new TObject<kk::String, kk::String>({{"errmsg",errmsg}}));
        }
        
        if((DispatchQueue *) _queue == getCurrentDispatchQueue()) {
            emit("close", e);
        } else {
            kk::Weak<TCPConnection> v = this;
            _queue->async([e,v]()->void{
                kk::Strong<TCPConnection> conn = v.operator->();
                if(conn != nullptr) {
                    conn->emit("close", e.operator->());
                }
            });
        }
        
    }
    
    void TCPConnection::Openlib() {
        
//        DNSResolve::set("weibo.com", "123.125.104.197");
//        DNSResolve::set("app.ziyouker.com", "59.110.190.36");
//        

        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<TCPConnection,kk::CString,kk::Int>(ctx, [](duk_context * ctx)->void{
                kk::PutProperty<TCPConnection,kk::Int>(ctx, -1, "port", &TCPConnection::port);
                kk::PutProperty<TCPConnection,kk::CString>(ctx, -1, "address", &TCPConnection::address);
                kk::PutMethod<TCPConnection,void>(ctx, -1, "close", &TCPConnection::close);
                kk::PutMethod<TCPConnection,void,kk::Any>(ctx, -1, "write", &TCPConnection::write);
                kk::PutMethod<TCPConnection,void>(ctx, -1, "read", &TCPConnection::read);
            });
            
        });
        
    }
    
}
