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

    SSLConnection::SSLConnection(kk::CString hostname,kk::Int port):TCPConnection(hostname,port),_hostname(hostname) {
        
    }
    
#ifdef __APPLE__
    
    static OSStatus SSLConnection_SSLRead(SSLConnectionRef connection,void *data, size_t *dataLength) {
        
        NetBuffer * buffer = (NetBuffer *) connection;
        
        ssize_t n = buffer->read(data, * dataLength);
        
        if(n == -1) {
            return errSSLClosedAbort;
        } else if(n == 0) {
            return errSSLWouldBlock;
        } else {
            * dataLength = n;
        }
        
        return noErr;
    }
    
    static OSStatus SSLConnection_SSLWrite(SSLConnectionRef connection,const void *data,size_t *dataLength) {
        
        NetBuffer * buffer = (NetBuffer *) connection;
        
        buffer->write(data, * dataLength);

        do {
            
            NetBufferFlushState s = buffer->flush();
            
            if(s == NetBufferFlushStateDone) {
                break;
            } else if(s == NetBufferFlushStateContinue) {
                continue;
            } else if(s == NetBufferFlushStateError) {
                return errSSLClosedAbort;
            }
            
        } while(0);
        
        return noErr;
    }
    
    void SSLConnection::openConnection(struct sockaddr * addr,socklen_t len) {
        
        kk::String errmsg;
        
        do {
            
            if (-1 == ::connect(_fd, addr, len)) {
                errmsg = "Error in connect";
                break;
            }
            
            kk::Strong<NetBuffer> buffer = new NetBuffer(_fd);
            
            SSLContextRef sslContext = SSLCreateContext(kCFAllocatorDefault, kSSLClientSide, kSSLStreamType);
            
            CFAutorelease(sslContext);
            
            OSStatus s = SSLSetIOFuncs(sslContext, &SSLConnection_SSLRead, &SSLConnection_SSLWrite);
            
            if (s != noErr) {
                errmsg = "Error in SSLSetIOFuncs";
                break;
            }
            
            s = SSLSetConnection(sslContext, (SSLConnectionRef) buffer.get());
            
            if (s != noErr) {
                errmsg = "Error in SSLSetConnection";
                break;
            }
            
            s = SSLSetPeerDomainName(sslContext, _hostname.c_str(), _hostname.size());
            
            if (s != noErr) {
                errmsg = "Error in SSLSetPeerDomainName";
                break;
            }
            
            do {
                
                s = SSLHandshake(sslContext);
                
                if(s == errSSLWouldBlock) {
                    continue;
                }
                
            } while (0);
            
            //errSecSuccess
            
            if (s == noErr) {
                
                if(-1 == fcntl(_fd, F_SETFL, O_NONBLOCK)) {
                    errmsg = "Socket Nonblock Error";
                    break;
                }
                
                kk::Strong<kk::NativeObject> v = new kk::NativeObject((kk::Native *) sslContext);
                kk::Weak<SSLConnection> vv = this;
                
                _queue->async([v,vv,this,buffer]()->void{
                    kk::Strong<SSLConnection> conn = vv.operator->();
                    
                    if(conn != nullptr) {
                        this->onOpen([v,buffer](NetEventBuffer * buffer, const void * data, size_t size)->ssize_t{
                            
                            SSLContextRef sslContext = (SSLContextRef) v.operator->()->native();
                            size_t n = 0;
                            OSStatus s = SSLWrite(sslContext, data, size, &n);
                            
                            if(s == noErr) {
                                return n;
                            } else if(s == errSSLWouldBlock) {
                                return 0;
                            } else {
                                kk::Log("[SSLHandshake] %d",s);
                                return -1;
                            }
                            
                        }, [v,buffer](NetEventBuffer * buffer, void * data, size_t size)->ssize_t{
                            
                            SSLContextRef sslContext = (SSLContextRef) v.operator->()->native();
                            size_t n = 0;
                            OSStatus s = SSLRead(sslContext, data, size, &n);
                            
                            if(s == noErr) {
                                return n;
                            } else if(s == errSSLWouldBlock) {
                                return 0;
                            } else {
                                kk::Log("[SSLHandshake] %d",s);
                                return -1;
                            }
                            
                        });
                    }
                });
                
            } else {
                kk::Log("[SSLHandshake] %d",s);
                errmsg = "Error in SSLHandshake";
                break;
            }
            
        
            break;
        } while (1);
        
        if(!errmsg.empty()) {
            
            kk::Weak<SSLConnection> v = this;
            
            _queue->async([v,this,errmsg]()->void{
                kk::Strong<SSLConnection> conn = v.operator->();
                if(conn != nullptr) {
                    this->close();
                    this->doClose(errmsg.c_str());
                }
            });
            
        }
        
    }
    
#endif
    
    
    void SSLConnection::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<SSLConnection,kk::CString,kk::Int>(ctx, [](duk_context * ctx)->void{
            });
            
        });
        
    }
    
}

