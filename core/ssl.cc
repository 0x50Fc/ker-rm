//
//  ssl.c
//  Ker
//
//  Created by zhanghailong on 2019/1/10.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "ssl.h"

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

#ifdef __APPLE__
#include <Security/Security.h>
#endif


namespace kk {

    SSLConnection::SSLConnection(kk::CString hostname,kk::Int port):TCPConnection(hostname,port),_hostname(hostname),_SSLConnected(false) {
        
    }
    
    kk::Boolean SSLConnection::isSSLConnected() {
        return _SSLConnected;
    }
    
#ifdef __APPLE__
    
    void SSLConnection::openConnection() {
        
        _input = new NetInputStream(_queue,_fd);
        _output = new NetOutputStream(_queue,_fd);
        
        try {
            
            SSLContextRef sslContext = SSLCreateContext(kCFAllocatorDefault, kSSLClientSide, kSSLStreamType);
            CFAutorelease(sslContext);
            
            OSStatus s = SSLSetIOFuncs(sslContext,
                                       [](SSLConnectionRef connection,void *data, size_t *dataLength)->OSStatus{
                                           
                                           kk::Log("[SSL] SSLRead");
                                           
                                           SSLConnection * conn = (SSLConnection *) connection;
                                           
                                           if(conn->input() == nullptr || conn->input()->status() == NetStreamStatusError) {
                                               return errSSLClosedAbort;
                                           }
                                           
                                           if(conn->isSSLConnected()) {
                                               ssize_t n = ::read(conn->fd(), data, * dataLength);
                                               if(n == -1) {
                                                   return errSSLClosedAbort;
                                               } else if(n == 0) {
                                                   return errSSLWouldBlock;
                                               }
                                               return noErr;
                                           }
                                           
                                           ssize_t n = conn->input()->read(data, * dataLength);
                                           
                                           if(n == -1) {
                                               return errSSLClosedAbort;
                                           } else if(n == 0) {
                                               * dataLength = 0;
                                               return errSSLWouldBlock;
                                           } else {
                                               kk::Log("[SSL] Read %d of %d",n,* dataLength);
                                               * dataLength = n;
                                               return noErr;
                                           }
                                          
                                       }, [](SSLConnectionRef connection,const void *data,size_t *dataLength)->OSStatus{
                                           
                                           kk::Log("[SSL] SSLWrite");
                                           
                                           SSLConnection * conn = (SSLConnection *) connection;
                                           
                                           if(conn->output() == nullptr || conn->output()->status() == NetStreamStatusError) {
                                               return errSSLClosedAbort;
                                           }
                                           
                                           if(conn->isSSLConnected()) {
                                               ssize_t n = ::write(conn->fd(), data, * dataLength);
                                               if(n == -1) {
                                                   return errSSLClosedAbort;
                                               } else if(n == 0) {
                                                   return errSSLWouldBlock;
                                               }
                                               return noErr;
                                           }
                                           
                                           ssize_t n = conn->output()->write(data, * dataLength);
                                           
                                           if(n == -1) {
                                               return errSSLClosedAbort;
                                           } else if(n == 0) {
                                               * dataLength = 0;
                                               return errSSLWouldBlock;
                                           } else {
                                               kk::Log("[SSL] Write %d of %d",n,* dataLength);
                                               * dataLength = n;
                                               return noErr;
                                           }
                                           
                                       });
            
            if (s != noErr) {
                throw "Error in SSLSetIOFuncs";
            }
            
            s = SSLSetConnection(sslContext, this);
            
            if (s != noErr) {
                throw "Error in SSLSetConnection";
            }
            
            s = SSLSetPeerDomainName(sslContext, _hostname.c_str(), _hostname.size());
            
            if (s != noErr) {
                throw "Error in SSLSetPeerDomainName";
            }
            
            kk::Weak<SSLConnection> v = this;
            kk::Strong<kk::NativeObject> n = new kk::NativeObject((kk::Native *) sslContext);
            
            static int count = 0;
            
            std::function<void()> handshake = [v,n]()->void{
                kk::Strong<SSLConnection> conn = v.operator->();
                SSLContextRef sslContext = (SSLContextRef) n.operator->()->native();
                if(conn != nullptr) {
                    
                    OSStatus s = SSLHandshake(sslContext);
                    if(s == noErr) {
                        conn->openSSLConnection([n](NetStream * stream, void * data, size_t size)->ssize_t{
                            
                            SSLContextRef sslContext = (SSLContextRef) n.operator->()->native();
                            size_t n = size;
                            
                            OSStatus s = SSLRead(sslContext, data, size, &n);
                            
                            if(s == noErr) {
                                return n;
                            } else if(s == errSSLWouldBlock) {
                                return 0;
                            } else {
                                return -1;
                            }
                            
                        },[n](NetStream * stream, const void * data, size_t size)->ssize_t{
                            
                            SSLContextRef sslContext = (SSLContextRef) n.operator->()->native();
                            size_t n = size;
                            
                            OSStatus s = SSLWrite(sslContext, data, size, &n);
                            
                            if(s == noErr) {
                                return n;
                            } else if(s == errSSLWouldBlock) {
                                return 0;
                            } else {
                                return -1;
                            }
                            
                        });
                    } else if( s == errSSLWouldBlock ) {
                        
                    } else {
                        kk::Log("[SSL] [SSLHandshake] [ERROR] %d",s);
                        conn->close();
                        conn->doClose("Error in SSLHandshake");
                    }
                }
            };
            
            std::function<void(NetStream *,kk::CString errmsg)> onError = [this](NetStream * stream,kk::CString errmsg)->void{
                kk::Log("[SSL] [ERROR] %s", errmsg);
                this->close();
                this->doClose(errmsg);
            };
            
            _input->setOnError(std::move(onError));
            _input->setOnEvent([handshake,this](NetStream * stream)->void{
                kk::Log("[SSL] onReading %d >>",count ++);
                handshake();
                kk::Log("[SSL] <<");
            });
            
            _output->setOnError(std::move(onError));
            _output->setOnEvent([handshake,this](NetStream * stream)->void{
                kk::Log("[SSL] onWriting %d >>",count ++);
                handshake();
                kk::Log("[SSL] <<");
            });
            
            _input->readBuffer().capacity(20480);
            
            kk::Log("[SSL] %d >>",count ++);
            
            handshake();
            
            kk::Log("[SSL] <<");
            
        }
        catch(kk::CString errmsg) {
            close();
            doClose(errmsg);
            return;
        }
        
        doOpen();
    }
    
#endif
    
    
    void SSLConnection::openSSLConnection(std::function<ssize_t(NetStream *,void *,size_t)> && onRead,std::function<ssize_t(NetStream *,const void *,size_t)> && onWrite) {
        
        _SSLConnected = true;
        
        _input->setProxy(std::move(onRead));
        _output->setProxy(std::move(onWrite));
        
        std::function<void(NetStream *,kk::CString errmsg)> onError = [this](NetStream * stream,kk::CString errmsg)->void{
            this->close();
            this->doClose(errmsg);
        };
        
        _input->setOnError(std::move(onError));
        _input->setOnEvent([this](NetStream * stream)->void{
            this->onRead();
        });
        
        _output->setOnError(std::move(onError));
        _output->setOnEvent([this](NetStream * stream)->void{
            this->onWrite();
        });
        
        doOpen();
        
    }
    
    void SSLConnection::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<SSLConnection,kk::CString,kk::Int>(ctx, [](duk_context * ctx)->void{
            });
            
        });
        
    }
    
}

