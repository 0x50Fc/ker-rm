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
                    a = (sockaddr *) addr;
                } else {
                    struct sockaddr_in * addr = (struct sockaddr_in * ) malloc(sizeof(struct sockaddr_in));
                    memset(addr,0,sizeof(struct sockaddr_in));
                    addr->sin_addr.s_addr = inet_addr(v.c_str());
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
    
    void DNSResolve::get(kk::CString hostname,std::function<void(struct sockaddr *,socklen_t)> && func) {
        
        kk::String v;
        
        if(hostname != nullptr) {
            v.append(hostname);
        }
        
        NetDispatchQueue()->async([v,func]()->void{
            
            auto i = gDNSResolveAddress.find(v);
            struct sockaddr * a = nullptr;
            
            if(i == gDNSResolveAddress.end()) {
                
                struct addrinfo *res, *cur;
                int ret;
                struct sockaddr *addr = nullptr;
            
                ret = getaddrinfo(v.c_str(), NULL,NULL ,&res);
                
                if (ret != -1) {
                    
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
                struct sockaddr_in6 addr;
                memcpy(&addr, a, sizeof(struct sockaddr_in6));
                func((struct sockaddr *) &addr,sizeof(struct sockaddr_in6));
            } else if(a->sa_family == AF_INET) {
                struct sockaddr_in addr;
                memcpy(&addr, a, sizeof(struct sockaddr_in));
                func((struct sockaddr *) &addr,sizeof(struct sockaddr_in));
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
        
        int opt = 1;
        if (-1 == setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
            ::close(_fd);
            _fd = -1;
            doClose("Socket ReuseAddr Error");
            return;
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
        
        _source = createDispatchSource((Uint64) _fd,DispatchSourceTypeRead,NetDispatchQueue());
        _source->setEvent([this]()->void{
            this->onAccept();
        });
        _source->resume();
    }
    
    void TCPServer::onAccept() {
        
        if(_fd == -1) {
            return;
        }
        
        kk::Strong<TCPClient> cli;
        
        if(_AF == AF_INET) {
            
            struct sockaddr_in addr = {0};
            
            socklen_t len = sizeof(addr);
            
            SocketId fd = accept(_fd, (struct sockaddr *) & addr, &len);
            
            if(-1 == fd) {
                close();
                doClose("Socket Accept Error");
                return;
            } else {
                cli = newClient(fd,inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
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
                cli = newClient(fd,address,ntohs(addr.sin6_port));
            }
            
        }
        
        kk::Strong<kk::Event> e = new kk::Event();
        e->setData(cli);
        kk::Weak<TCPServer> v = this;
        _queue->async([v,e]()->void{
            kk::Strong<TCPServer> vv = v.operator->();
            if(vv != nullptr) {
                vv->emit("accept", e.operator->());
            }
        });
    }
    
    void TCPServer::doClose(kk::CString errmsg) {

        kk::Strong<kk::Event> e = new kk::Event();
        
        if(errmsg != nullptr) {
            e->setData(new TObject<kk::String, kk::String>({{"errmsg",errmsg}}));
        }
        
        kk::Weak<TCPServer> v = this;
        
        _queue->async([e,v]()->void{
            kk::Strong<TCPServer> vv = v.operator->();
            if(vv != nullptr) {
                vv->emit("error", e.operator->());
            }
        });
    }
    
    kk::Strong<TCPClient> TCPServer::newClient(SocketId fd,kk::CString address,kk::Int port) {
        return new TCPClient(fd,address,port,_queue);
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
    
    TCPClient::TCPClient(kk::CString hostname,kk::Int port):_fd(-1),_queue(getCurrentDispatchQueue()) {
        kk::Weak<TCPClient> v = this;
        DNSResolve::get(hostname, [v,port](struct sockaddr * addr,size_t len)->void{
            kk::Strong<TCPClient> vv = v.operator->();
            if(vv != nullptr) {
                if(addr == nullptr) {
                    vv->doClose("DNS Resolve Error");
                } else if(addr->sa_family ==AF_INET){
                    struct sockaddr_in * in = (struct sockaddr_in * ) addr;
                    in->sin_port = htons(port);
                    vv->onResolve(addr);
                } else if(addr->sa_family ==AF_INET){
                    struct sockaddr_in6 * in = (struct sockaddr_in6 * ) addr;
                    in->sin6_port = htons(port);
                    vv->onResolve(addr);
                } else {
                    vv->doClose("DNS Resolve Error");
                }
            }
            
        });
    }
    
    TCPClient::TCPClient(SocketId fd,kk::CString address,kk::Int port,DispatchQueue * queue):_port(port),_queue(queue),_fd(fd),_address(address) {
        
        _rd = createDispatchSource((Uint64) _fd,DispatchSourceTypeRead,NetDispatchQueue());
        _rd->setEvent([this]()->void{
            this->onRead();
        });
        _rd->resume();
        
        _wd = createDispatchSource((Uint64) _fd,DispatchSourceTypeWrite,NetDispatchQueue());
        _wd->setEvent([this]()->void{
            this->onWrite();
        });
    }
    
    TCPClient::~TCPClient() {
        
        if(_wd != nullptr) {
            _wd->cancel();
        }
        
        if(_rd != nullptr) {
            _rd->cancel();
        }
    
        if(_fd != -1) {
            shutdown(_fd, SHUT_RDWR);
            ::close(_fd);
            _fd = -1;
        }
        
    }
    
    kk::CString TCPClient::address() {
        return _address.c_str();
    }
    
    kk::Int TCPClient::port() {
        return _port;
    }
    
    void TCPClient::close() {
        
        if(_wd != nullptr) {
            _wd->cancel();
            _wd = nullptr;
        }
        
        if(_rd != nullptr) {
            _rd->cancel();
            _rd = nullptr;
        }
        
        if(_fd != -1) {
            shutdown(_fd, SHUT_RDWR);
            ::close(_fd);
            _fd = -1;
        }
        
    }
    
    void TCPClient::onResolve(struct sockaddr * addr) {
        
        
    }
    
    void TCPClient::onRead() {
        
        if(_fd == -1) {
            return;
        }
        
        char data[204800];
        
        ssize_t n = ::recv(_fd, data, sizeof(data), 0);
        
        if(n == -1) {
            close();
            doClose("Socket Recv Error");
            return;
        } else if(n > 0) {
            doData(data,n);
        }
    }
    
    kk::Int TCPClient::write(Any & value) {
        if(value.type == TypeString) {
            return write((void *) value.stringValue, value.length);
        } else if(value.type == TypeObject) {
            ArrayBuffer * b = value.objectValue;
            if(b != nullptr) {
                return write(b->data(),(size_t) b->byteLength());
            }
        }
        return -1;
    }
    
    kk::Int TCPClient::write(void * data,size_t size) {
        
        if(_fd == -1) {
            return -1;
        }
        
        ssize_t v = ::send(_fd, data, size, 0);
        
        if(v == -1) {
            close();
            doClose("Socket Send Error");
        } else {
            _wd->resume();
        }
        
        return (kk::Int) v;
    }
    
    void TCPClient::onWrite(){
        _wd->suspend();
        kk::Weak<TCPClient> v = this;
        _queue->async([v]()->void{
            kk::Strong<TCPClient> vv = v.operator->();
            if(vv != nullptr) {
                kk::Strong<Event> e = new Event();
                vv->emit("done", e.operator->());
            }
        });
    }
    
    void TCPClient::doData(void * data,size_t size) {
        kk::Strong<Event> e = new Event();
        e->setData(new ArrayBuffer(data,(kk::Uint) size));
        kk::Weak<TCPClient> v = this;
        _queue->async([v,e]()->void{
            kk::Strong<TCPClient> vv = v.operator->();
            if(vv != nullptr) {
                vv->emit("data", e.operator->());
            }
        });
    }
    
    void TCPClient::doClose(kk::CString errmsg) {
        
        kk::Strong<kk::Event> e = new kk::Event();
        
        if(errmsg != nullptr) {
            e->setData(new TObject<kk::String, kk::String>({{"errmsg",errmsg}}));
        }
        
        kk::Weak<TCPClient> v = this;
        
        _queue->async([e,v]()->void{
            kk::Strong<TCPClient> vv = v.operator->();
            if(vv != nullptr) {
                vv->emit("error", e.operator->());
            }
        });
    }
    
    void TCPClient::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<TCPClient,kk::CString,kk::Int>(ctx, [](duk_context * ctx)->void{
                kk::PutProperty<TCPClient,kk::Int>(ctx, -1, "port", &TCPClient::port);
                kk::PutProperty<TCPClient,kk::CString>(ctx, -1, "address", &TCPClient::address);
                kk::PutMethod<TCPClient,void>(ctx, -1, "close", &TCPClient::close);
            });
            
        });
        
    }
    
}
