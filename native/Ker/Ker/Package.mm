//
//  Package.m
//  Ker
//
//  Created by hailong11 on 2018/12/10.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CommonCrypto/CommonCrypto.h>

#include <ui/ui.h>
#include <ui/app.h>
#include <ui/package.h>
#include <core/event.h>
#include "unzip.h"

#import "KerApp.h"

namespace kk {
    extern ::dispatch_queue_t DispatchQueueGCD(DispatchQueue * queue);
}

namespace kk {
    
    namespace ui {
    
        class OSPackage : public Package {
        public:
            
            OSPackage(kk::CString URI,kk::CString basePath,kk::CString appkey):Package(URI),_basePath(basePath),_appkey(appkey) {

            }
            
            virtual void run(kk::Object * query) {
                
                @autoreleasepool {
                    
                    KerApp * app = [[KerApp alloc] initWithBasePath:[NSString stringWithCString:_basePath.c_str() encoding:NSUTF8StringEncoding]
                                                             appkey:[NSString stringWithCString:_appkey.c_str() encoding:NSUTF8StringEncoding]];
                    
                    kk::ui::App * v = (kk::ui::App *) [app CPointer];
                    
                    v->exec("main.js", new TObject<kk::String, kk::Any>({{"query",query}}));
                    
                }
                
            }
            
        protected:
            kk::String _basePath;
            kk::String _appkey;
        };
        
        kk::Strong<Package> createPackage(Context * context,kk::CString URI) {
            
            if(URI == nullptr) {
                return nullptr;
            }
            
            @autoreleasepool {
                
                NSString * u = [NSString stringWithCString:URI encoding:NSUTF8StringEncoding];
                
                if(![u containsString:@"://"]) {
                    u = [[NSString stringWithCString:context->basePath() encoding:NSUTF8StringEncoding] stringByAppendingPathComponent:u];
                }
                
                NSString * basePath = nil;
                NSString * appkey = nil;
                
                [KerApp getPackage:u basePath:&basePath appkey:&appkey];
                
                if(appkey == nil) {
                    return nullptr;
                }
                
                OSPackage * v = new OSPackage(URI,[basePath UTF8String],[appkey UTF8String]);
                
                dispatch_queue_t queue = kk::DispatchQueueGCD(context->queue());
                
                if([KerApp hasPackage:u basePath:basePath appkey:appkey]) {
                
                    v->retain();
                    
                    dispatch_async(queue, ^{
                        
                        v->setState(PackageStateLoaded);
                        
                        kk::Event * e = new kk::Event();
                        
                        v->emit("load", e);
                        
                        v->release();
                    });
                    
                } else {
                    
                    v->retain();
                    
                    [KerApp getPackage:u callback:^(NSString *basePath, NSString *appkey, NSString *errmsg) {
                        
                        if(errmsg != nil) {
                            
                            v->setState(PackageStateError);
                            
                            kk::Event * e = new kk::Event();
                            
                            e->setData(new TObject<kk::String, kk::String>({{"errmsg",[errmsg UTF8String]}}));
                            
                            v->emit("error", e);
                            
                        } else {
                            
                            v->setState(PackageStateLoaded);
                            
                            kk::Event * e = new kk::Event();
                            
                            v->emit("load", e);
                            
                        }
                        
                        v->release();
                        
                    } queue:queue];
                    
                }
                
                return v;
                
            }
            
        }
    }
    
}

