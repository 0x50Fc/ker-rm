//
//  WebSocket.h
//  Ker
//
//  Created by zhanghailong on 2019/1/14.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef kk_WebSocket_h
#define kk_WebSocket_h

#include <core/net.h>
#include <core/uri.h>

namespace kk {
    
    enum WebSocketType {
        WebSocketTypeNone,WebSocketTypeText,WebSocketTypeBinary,WebSocketTypePing
    };
    
    class WebSocket : public EventEmitter {
    public:
        WebSocket(DispatchQueue * queue);
        WebSocket();
        virtual ~WebSocket();
        virtual kk::Boolean open(kk::CString url,kk::CString protocol);
        virtual void close();
        virtual void write(const void * data,size_t n,WebSocketType type);
        virtual void write(const void * data,size_t n);
        virtual void write(kk::CString text);
        virtual void write(kk::Any v);
        
        static void Openlib();
        
        Ker_CLASS(WebSocket,EventEmitter,"WebSocket")
        
        
    protected:
        
        virtual void onData(const void * data, size_t n, WebSocketType type);
        virtual void onOpen();
        virtual void onReading();
        virtual void onClose(kk::CString errmsg);
        virtual void onClose(kk::Event * event);
        kk::Strong<DispatchQueue> _queue;
        kk::Strong<TCPConnection> _conn;
        kk::Strong<URI> _URI;
        Buffer _body;
        WebSocketType _bodyType;
        kk::Int64 _bodyLength;
    };
    
}

#endif /* WebSocket_h */
