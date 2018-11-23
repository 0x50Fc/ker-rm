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

namespace kk {
    
    namespace ui {
    
        App::App(kk::CString basePath,kk::CString platform):Context(basePath,kk::mainDispatchQueue()) {

            duk_context * ctx = jsContext();
            
            PushWeakObject(ctx, this);
            duk_put_global_string(ctx, "app");
            
            duk_push_string(ctx, platform);
            duk_put_global_string(ctx, "platform");
            
            kk::Openlib<App *>::openlib(ctx, this);
        }
        
        App::~App() {
            kk::Log("[App] [dealloc]");
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
        
        void App::Openlib() {
            
            kk::OpenBaselib();
            kk::Event::Openlib();
            kk::EventEmitter::Openlib();
            kk::Timer::Openlib();
            kk::ui::Context::Openlib();
            kk::ui::View::Openlib();
            kk::ui::Canvas::Openlib();
            kk::ui::AttributedText::Openlib();
            kk::ui::WebViewConfiguration::Openlib();
            kk::ui::Context::Openlib();
            kk::ui::Page::Openlib();
            kk::ui::CG::Context::Openlib();
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<App>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutMethod<App,void,kk::CString,kk::Boolean>(ctx, -1, "open", &App::open);
                    kk::PutMethod<App,void,kk::Uint,kk::Boolean>(ctx, -1, "back", &App::back);
                
                    kk::PutStrongMethod<App,View,kk::CString,ViewConfiguration *>(ctx,-1,"createView",&App::createView);
                    
                });
                
            });
            
        }
        
        void addAppOpenlib(std::function<void(duk_context *,App *)> && func) {
            kk::Openlib<App *>::add(std::move(func));
        }
        
    }
    
}
