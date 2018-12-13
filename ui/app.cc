//
//  app.cc
//  KK
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <ui/app.h>
#include <ui/page.h>
#include <ui/CGContext.h>
#include <ui/package.h>
#include <core/crypto.h>

namespace kk {
    
    namespace ui {
    
        App::App(kk::CString basePath,kk::CString platform,kk::CString userAgent,kk::CString appkey):Context(basePath,kk::mainDispatchQueue()),_appkey(appkey) {

            duk_context * ctx = jsContext();
            
            PushWeakObject(ctx, this);
            duk_put_global_string(ctx, "app");
            
            duk_push_string(ctx, platform);
            duk_put_global_string(ctx, "platform");
            
            duk_push_string(ctx, userAgent);
            duk_put_global_string(ctx, "userAgent");
            
            kk::Openlib<App *>::openlib(ctx, this);
        }
        
        App::~App() {
            kk::Log("[App] [dealloc]");
        }
        
        kk::CString App::appkey() {
            return _appkey.c_str();
        }
        
        void App::open(kk::CString uri,kk::Boolean animated) {
            if(uri == nullptr) {
                return;
            }
            kk::Strong<Event> e = new Event();
            kk::Strong<kk::TObject<kk::String, kk::Any>> data = new kk::TObject<kk::String, kk::Any>({{"uri", kk::Any(uri)},{"animated",kk::Any(animated)}});
            e->setData((kk::TObject<kk::String, kk::Any> *) data);
            emit("open", e);
        }
        
        void App::back(kk::Uint delta,kk::Boolean animated) {
            kk::Strong<Event> e = new Event();
            kk::Strong<kk::TObject<kk::String, kk::Any>> data = new kk::TObject<kk::String, kk::Any>({{"delta", kk::Any(delta)},{"animated",kk::Any(animated)}});
            e->setData((kk::TObject<kk::String, kk::Any> *) data);
            emit("back", e);
        }
        
        kk::Strong<View> App::createView(kk::CString name,ViewConfiguration * configuration) {
            return kk::ui::createView(name, configuration, this);
        }
        
        
        Size App::getAttributedTextContentSize(AttributedText * text,Float maxWidth) {
            return ::kk::ui::getAttributedTextContentSize(this, text, maxWidth);
        }
        
        kk::Strong<Package> App::createPackage(kk::CString URI) {
            return ::kk::ui::createPackage(this,URI);
        }
        
        void App::Openlib() {
            
            kk::OpenBaselib();
            kk::Event::Openlib();
            kk::EventEmitter::Openlib();
            kk::Timer::Openlib();
            kk::Sqlite::Openlib();
            kk::Crypto::Openlib();
            kk::ui::Context::Openlib();
            kk::ui::View::Openlib();
            kk::ui::Canvas::Openlib();
            kk::ui::AttributedText::Openlib();
            kk::ui::WebViewConfiguration::Openlib();
            kk::ui::Context::Openlib();
            kk::ui::Page::Openlib();
            kk::ui::CG::Context::Openlib();
            kk::ui::Package::Openlib();
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<App>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutMethod<App,void,kk::CString,kk::Boolean>(ctx, -1, "open", &App::open);
                    kk::PutMethod<App,void,kk::Uint,kk::Boolean>(ctx, -1, "back", &App::back);
                    kk::PutMethod<App,Size,AttributedText *,Float>(ctx, -1, "getAttributedTextContentSize", &App::getAttributedTextContentSize);
                    kk::PutStrongMethod<App,Package,kk::CString>(ctx,-1,"createPackage",&App::createPackage);
                
                    kk::PutStrongMethod<App,View,kk::CString,ViewConfiguration *>(ctx,-1,"createView",&App::createView);
                    kk::PutProperty<App,kk::CString>(ctx, -1, "appkey", &App::appkey);
                    
                });
                
                
            });
            
        }
        
        void addAppOpenlib(std::function<void(duk_context *,App *)> && func) {
            kk::Openlib<App *>::add(std::move(func));
        }
        
    }
    
}
