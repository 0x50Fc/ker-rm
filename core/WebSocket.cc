//
//  WebSocket.cc
//  Ker
//
//  Created by zhanghailong on 2019/1/14.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "WebSocket.h"

#include <core/uri.h>
#include <core/ssl.h>

#ifndef ntohll

#define ntohll(x) \
((__uint64_t)((((__uint64_t)(x) & 0xff00000000000000ULL) >> 56) | \
(((__uint64_t)(x) & 0x00ff000000000000ULL) >> 40) | \
(((__uint64_t)(x) & 0x0000ff0000000000ULL) >> 24) | \
(((__uint64_t)(x) & 0x000000ff00000000ULL) >>  8) | \
(((__uint64_t)(x) & 0x00000000ff000000ULL) <<  8) | \
(((__uint64_t)(x) & 0x0000000000ff0000ULL) << 24) | \
(((__uint64_t)(x) & 0x000000000000ff00ULL) << 40) | \
(((__uint64_t)(x) & 0x00000000000000ffULL) << 56)))

#define htonll(x) ntohll(x)

#endif

#define Sec_WebSocket_Key "RCfYMqhgCo4N4E+cIZ0iPg=="
#define MAX_BUF_SIZE 2147483647

#define KKFinMask           0x80
#define KKOpCodeMask        0x0F
#define KKRSVMask           0x70
#define KKMaskMask          0x80
#define KKPayloadLenMask    0x7F
#define KKMaxFrameSize      32


namespace kk  {
    
    enum WebSocketOpCode {
        WebSocketOpCodeContinueFrame = 0x0,
        WebSocketOpCodeTextFrame = 0x1,
        WebSocketOpCodeBinaryFrame = 0x2,
        WebSocketOpCodeConnectionClose = 0x8,
        WebSocketOpCodePing = 0x9,
        WebSocketOpCodePong = 0xA,
    };
    
    WebSocket::WebSocket(DispatchQueue * queue):_queue(queue),_bodyType(WebSocketTypeNone) {
        
    }
    
    WebSocket::WebSocket():WebSocket(getCurrentDispatchQueue()) {
        
    }
    
    WebSocket::~WebSocket() {
        if(_conn != nullptr) {
            _conn->off();
            _conn->close();
            _conn = nullptr;
        }
    }
    
    kk::Boolean WebSocket::open(kk::CString url,kk::CString protocol) {
        
        if(url == nullptr) {
            return false;
        }
        
        if(_conn == nullptr) {
            return false;
        }
        
        _URI = new URI(url);
        
        if(kk::CStringEqual( _URI->scheme(),"wss") || kk::CStringEqual( _URI->scheme(),"https")) {
            int port = 443;
            if(_URI->port() != nullptr) {
                port = atoi(_URI->port());
            }
            _conn = new SSLConnection(_URI->host(),port);
        } else {
            int port = 80;
            if(_URI->port() != nullptr) {
                port = atoi(_URI->port());
            }
            _conn = new TCPConnection(_URI->host(),port);
        }
        
        _conn->on("open", new kk::TFunction<void,kk::CString,Event *>([this](kk::CString name,Event * e)->void{
            this->onOpen();
        }));
        
        _conn->on("data", new kk::TFunction<void,kk::CString,Event *>([this](kk::CString name,Event * e)->void{
            this->onReading();
        }));
        
        _conn->on("close", new kk::TFunction<void,kk::CString,Event *>([this](kk::CString name,Event * e)->void{
            this->close();
            this->onClose(e);
        }));
        
        return false;
    }
    
    void WebSocket::onOpen() {
        
        Buffer & output = _conn->outputBuffer();
        
        kk::String origin;
        
        if(_URI->scheme() != nullptr) {
            if(kk::CStringEqual(_URI->scheme(), "wss")) {
                origin.append("https");
            } else if(kk::CStringEqual(_URI->scheme(), "ws")) {
                origin.append("http");
            } else {
                origin.append(_URI->scheme());
            }
            
            origin.append("://");
        }
        
        if(_URI->host() != nullptr) {
            origin.append(_URI->host());
        }
        
        if(_URI->path() == nullptr) {
            output.format("GET / HTTP/1.1\r\n");
        } else {
            output.format("GET %s HTTP/1.1\r\n", _URI->path());
        }
        
        output.format("Host: %s\r\n",_URI->host());
        output.format("Upgrade: %s\r\n","websocket");
        output.format("Connection: %s\r\n","Upgrade");
        output.format("Origin: %s\r\n",origin.c_str());
        output.format("Sec-WebSocket-Key: %s\r\n",Sec_WebSocket_Key);
        output.format("Sec-WebSocket-Version: %s\r\n","13");
        output.format("\r\n");
        
        _conn->flush();
        
    }
    
    void WebSocket::onData(const void * data, size_t n, WebSocketType type) {
        
    }
    
    void WebSocket::onReading() {
        
        while(1) {
            Buffer & input = _conn->inputBuffer();
            uint8_t * p = input.data();
            size_t n = input.byteLength();
            
            if(_bodyType != WebSocketTypeNone && _bodyLength > 0 && n > 0) {
                
                ssize_t v = (ssize_t) _bodyLength - _body.byteLength();
                
                if(v > 0 && n >= v) {
                    _body.append(p, (kk::Uint) v);
                    input.drain((kk::Uint) v);
                    p = input.data();
                    n = input.byteLength();
                }
                
                if(_body.byteLength() == _bodyLength){
                    onData(_body.data(),_body.byteLength(),_bodyType);
                    _body.drain(_body.byteLength());
                    _bodyType = WebSocketTypeNone;
                    _bodyLength = 0;
                } else {
                    return;
                }
            }
            
            if(n >= 2) {
                
                bool isFin = (KKFinMask & p[0]);
                uint8_t receivedOpcode = KKOpCodeMask & p[0];
                bool isMasked = (KKMaskMask & p[1]);
                uint8_t payloadLen = (KKPayloadLenMask & p[1]);
                int offset = 2;
                
                if((isMasked  || (KKRSVMask & p[0])) && receivedOpcode != WebSocketOpCodePong) {
                    this->close();
                    this->onClose("不支持的协议");
                    return;
                }
                
                bool isControlFrame = (receivedOpcode == WebSocketOpCodeConnectionClose || receivedOpcode == WebSocketOpCodePing);
                
                if(!isControlFrame && (receivedOpcode != WebSocketOpCodeBinaryFrame && receivedOpcode != WebSocketOpCodeContinueFrame && receivedOpcode != WebSocketOpCodeTextFrame && receivedOpcode != WebSocketOpCodePong)) {
                    this->close();
                    this->onClose("不支持的协议");
                    return;
                }
                
                if(isControlFrame && !isFin) {
                    this->close();
                    this->onClose("不支持的协议");
                    return;
                }
                
                if(receivedOpcode == WebSocketOpCodeConnectionClose) {
                    this->close();
                    this->onClose("链接已关闭");
                    return;
                }
                
                if(isControlFrame && payloadLen > 125) {
                    this->close();
                    this->onClose("不支持的协议");
                    return;
                }
                
                uint64_t dataLength = payloadLen;
                if(payloadLen == 127) {
                    dataLength =  ntohll((*(uint64_t *)(p+offset)));
                    offset += sizeof(uint64_t);
                } else if(payloadLen == 126) {
                    dataLength = ntohs(*(uint16_t *)(p+offset));
                    offset += sizeof(uint16_t);
                }
                
                if(n < offset) {
                    return;
                }
                
                uint64_t len = dataLength;
                
                if(dataLength > (n-offset) || (n - offset) < dataLength) {
                    len = n-offset;
                }
                
                if(receivedOpcode == WebSocketOpCodePong) {
                    input.drain((kk::Uint) (offset + len));
                    continue;
                }
                
                if(receivedOpcode == WebSocketOpCodeContinueFrame) {
                    _body.append(p + offset, (kk::Uint) len);
                    input.drain((kk::Uint) (offset + len));
                } else if(receivedOpcode == WebSocketOpCodeTextFrame) {
                    _bodyType = WebSocketTypeText;
                    _body.append(p + offset, (kk::Uint) len);
                    input.drain((kk::Uint) (offset + len));
                } else if(receivedOpcode == WebSocketOpCodeBinaryFrame) {
                    _bodyType = WebSocketTypeBinary;
                    _body.append(p + offset, (kk::Uint) len);
                    input.drain((kk::Uint) (offset + len));
                } else if(receivedOpcode == WebSocketOpCodePing) {
                    _bodyType = WebSocketTypePing;
                    _body.append(p + offset, (kk::Uint) len);
                    input.drain((kk::Uint) (offset + len));
                } else {
                    this->close();
                    this->onClose("不支持的协议");
                    return;
                }
                
                if(isFin && _body.byteLength() == dataLength) {
                    onData(_body.data(),_body.byteLength(),_bodyType);
                    _body.drain(_body.byteLength());
                    _bodyType = WebSocketTypeNone;
                    _bodyLength = 0;
                } else {
                    _bodyLength = dataLength;
                }
                
                continue;
            }

            break;
        }
    }
    
    void WebSocket::onClose(kk::Event * event) {
        emit("error", event);
    }
    
    void WebSocket::onClose(kk::CString errmsg) {
        
        kk::Strong<kk::Event> e = new kk::Event();
        
        if(errmsg != nullptr) {
            e->setData(new TObject<kk::String, kk::String>({{"errmsg",errmsg}}));
        }
        
        onClose((kk::Event *) e);
        
    }
    
    void WebSocket::close() {
        if(_conn != nullptr) {
            _conn->close();
            _conn = nullptr;
        }
    }
    
    void WebSocket::write(const void * data,size_t n,WebSocketType type) {
        
        if(_conn == nullptr) {
            return ;
        }
        
        uint8_t frame[KKMaxFrameSize];
        
        memset(frame, 0, sizeof(frame));
        
        switch (type) {
            case WebSocketTypePing:
                frame[0] = KKFinMask | WebSocketOpCodePing;
                break;
            case WebSocketTypeText:
                frame[0] = KKFinMask | WebSocketOpCodeTextFrame;
                break;
            case WebSocketTypeBinary:
                frame[0] = KKFinMask | WebSocketOpCodeBinaryFrame;
                break;
            default:
                return;
        }
        
        uint64_t offset = 2;
        
        if(n < 126) {
            frame[1] |= n;
        } else if(n <= UINT16_MAX) {
            frame[1] |= 126;
            *((uint16_t *)(frame + offset)) = htons((uint16_t)n);
            offset += sizeof(uint16_t);
        } else {
            frame[1] |= 127;
            *((uint64_t *)(frame + offset)) = htonll((uint64_t)n);
            offset += sizeof(uint64_t);
        }
        
        frame[1] |= KKMaskMask;
        uint8_t *mask_key = (frame + offset);
        for(int i=0;i<sizeof(uint32_t);i++) {
            mask_key[i] = rand();
        }
        offset += sizeof(uint32_t);
        
        Buffer & output = _conn->outputBuffer();
        
        output.append(frame, (kk::Uint) offset);
        
        uint8_t * p = (uint8_t *) data;
        uint8_t u;
        
        for(int i=0;i<n;i++) {
            u = p[i] ^ mask_key[i % sizeof(uint32_t)];
            output.append(&u, 1);
        }
        
        _conn->flush();
    }
    
    void WebSocket::write(const void * data,size_t n) {
        write(data, n, WebSocketTypeBinary);
    }
    
    void WebSocket::write(kk::CString text) {
        write(text, strlen(text), WebSocketTypeText);
    }
    
    void WebSocket::write(kk::Any v) {
        if(v.type == TypeString) {
            write(v.stringValue, v.length, WebSocketTypeText);
        } else if(v.type == TypeObject) {
            ArrayBuffer * b = v.objectValue;
            if(b != nullptr) {
                write(b->data(),b->byteLength(),WebSocketTypeBinary);
            }
        }
    }
    
    void WebSocket::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<WebSocket>(ctx, [](duk_context * ctx)->void{
                kk::PutMethod<WebSocket,kk::Boolean,kk::CString,kk::CString>(ctx, -1, "open", &WebSocket::open);
                kk::PutMethod<WebSocket,void>(ctx, -1, "close", &WebSocket::close);
                kk::PutMethod<WebSocket,void,kk::Any>(ctx, -1, "send", &WebSocket::write);
            });
            
        });
        
    }
    
}
