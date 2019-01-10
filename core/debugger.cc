//
//  debugger.cc
//  Ker
//
//  Created by zhanghailong on 2019/1/7.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include <core/debugger.h>
#include <core/dispatch.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

namespace kk {
    
    namespace Debugger {
        
    
        static kk::Int _listenId = -1;
        
        kk::Boolean start(kk::Int port) {
            
            if(_listenId != -1) {
                stop();
            }
            
            _listenId = socket(AF_INET, SOCK_STREAM, 0);
            
            if(_listenId < 0) {
                kk::Log("[DUK] [DEBUGGER] [ERROR] %s",strerror(errno));
                return false;
            }
            
            int on = 1;
            if (setsockopt(_listenId, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on)) < 0) {
                kk::Log("[DUK] [DEBUGGER] [ERROR] %s",strerror(errno));
                close(_listenId);
                _listenId = -1;
                return false;
            }
            
            struct sockaddr_in addr;
            
            memset((void *) &addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = INADDR_ANY;
            addr.sin_port = htons(port);
            
            if (bind(_listenId, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
                kk::Log("[DUK] [DEBUGGER] [ERROR] %s",strerror(errno));
                close(_listenId);
                _listenId = -1;
                return false;
            }
            
            if(listen(_listenId, 1 ) < 0) {
                close(_listenId);
                _listenId = -1;
                return false;
            }
            
            kk::Log("[DUK] [DEBUGGER] [SERVER] %d",port);
            
            return true;
        }
        
        void stop() {
            if(_listenId != -1) {
                close(_listenId);
                _listenId = -1;
            }
        }
        
        static duk_size_t duk_trans_socket_read_cb(void *udata, char *buffer, duk_size_t length) {
            
            ssize_t ret;
            
            int client_sock = (int) (long) udata;
            
            if (client_sock < 0) {
                return 0;
            }
            
            if (length == 0) {
                kk::Log("[DUK] [DEBUGGER] [ERROR] read request length == 0");
                return 0;
            }
            
            if (buffer == NULL) {
                kk::Log("[DUK] [DEBUGGER] [ERROR] read request buffer == NULL");
                return 0;
            }
            
            ret = read(client_sock, (void *) buffer, (size_t) length);
            
            if (ret < 0) {
                kk::Log("[DUK] [DEBUGGER] [ERROR] debug read failed, closing connection: %s",strerror(errno));
                return 0;
            } else if (ret == 0) {
                kk::Log("[DUK] [DEBUGGER] [ERROR] debug read failed, ret == 0 (EOF)");
                return 0;
            } else if (ret > (ssize_t) length) {
                kk::Log("[DUK] [DEBUGGER] [ERROR] debug read failed, ret too large (%ld > %ld)",(long) ret, (long) length);
                return 0;
            }
            
            return (duk_size_t) ret;
            
        }
        
        duk_size_t duk_trans_socket_write_cb(void *udata, const char *buffer, duk_size_t length) {
            ssize_t ret;
            
            int client_sock = (int) (long) udata;
            
            if (client_sock < 0) {
                return 0;
            }
            
            if (length == 0) {
                kk::Log("[DUK] [DEBUGGER] [ERROR] read request length == 0");
                return 0;
            }
            
            if (buffer == NULL) {
                kk::Log("[DUK] [DEBUGGER] [ERROR] read request buffer == NULL");
                return 0;
            }
            
            ret = write(client_sock, (const void *) buffer, (size_t) length);
            
            if (ret <= 0 || ret > (ssize_t) length) {
                kk::Log("[DUK] [DEBUGGER] [ERROR] debug write failed: %s",strerror(errno));
                return 0;
            }
            
            return (duk_size_t) ret;
            
        }
        
        duk_size_t duk_trans_socket_peek_cb(void *udata) {
            
            struct timeval tm;
            fd_set rfds;
            int select_rc;
            
            int client_sock = (int) (long) udata;
            
            if (client_sock < 0) {
                return 0;
            }
            
            FD_ZERO(&rfds);
            FD_SET(client_sock, &rfds);
            tm.tv_sec = tm.tv_usec = 0;
            select_rc = select(client_sock + 1, &rfds, NULL, NULL, &tm);
            if (select_rc == 0) {
                return 0;
            } else if (select_rc == 1) {
                return 1;
            }
            return 0;
        }
        
        void duk_trans_socket_read_flush_cb(void *udata) {
            
        }
        
        void duk_trans_socket_write_flush_cb(void *udata) {
            
        }
        
        static duk_idx_t debugger_request(duk_context *ctx, void *udata, duk_idx_t nvalues) {
            return 0;
        }
        
        static duk_idx_t debugger_dealloc(duk_context *ctx) {
            duk_get_prop_string(ctx, -1, "id");
            if(duk_is_number(ctx, -1)) {
                kk::Int id = duk_to_int(ctx, -1);
                close(id);
            }
            duk_pop(ctx);
            return 0;
        }
        
        static void debugger_detached(duk_context *ctx, void *udata) {
            duk_push_global_object(ctx);
            duk_push_string(ctx, "__debugger");
            duk_del_prop(ctx, -2);
            duk_pop(ctx);
        }
        
        kk::Boolean debug(duk_context * ctx) {
            
            if(_listenId == -1) {
                return false;
            }
            
            duk_get_global_string(ctx, "__debugger");
            
            if(!duk_is_undefined(ctx, -1)) {
                duk_pop(ctx);
                return false;
            }
            
            duk_pop(ctx);
            
            struct sockaddr_in addr;
            socklen_t sz;
            
            sz = (socklen_t) sizeof(addr);
            
            kk::Log("[DUK] [DEBUGGER] [CLIENT] ...");
            
            kk::Int cli = accept(_listenId, (struct sockaddr *) &addr, &sz);
            
            if (cli < 0) {
                kk::Log("[DUK] [DEBUGGER] [ERROR] %s",strerror(errno));
                return false;
            }
            
            kk::Log("[DUK] [DEBUGGER] [CLIENT] %s:%d ...",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
            
            duk_push_global_object(ctx);
            {
                duk_push_string(ctx, "__debugger");
                duk_push_object(ctx);
                {
                    duk_push_int(ctx, cli);
                    duk_put_prop_string(ctx, -2, "id");
                    duk_push_c_function(ctx, debugger_dealloc, 1);
                    duk_set_finalizer(ctx, -2);
                }
                duk_put_prop(ctx, -3);
            }
            duk_pop(ctx);
            
            duk_debugger_attach(ctx,
                                duk_trans_socket_read_cb,
                                duk_trans_socket_write_cb,
                                duk_trans_socket_peek_cb,
                                duk_trans_socket_read_flush_cb,
                                duk_trans_socket_write_flush_cb,
                                debugger_request,
                                debugger_detached,
                                (void *) (long) cli);
            
            return 0;
        }
        
    }
    
}

