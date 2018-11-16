//
//  page.cc
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <ui/page.h>

namespace kk {

    namespace ui {
        
        void addPageOpenlib(std::function<void(duk_context *,Page *)> && func) {
            kk::Openlib<kk::ui::Page *>::add(std::move(func));
        }
        
        Page::Page(App * app,View * view):_app(app),_view(view),_librarys(new TObject<kk::String, kk::Any>()) {
            duk_context * ctx = app->jsContext();
            duk_idx_t idx = duk_push_thread(ctx);
            _jsContext = duk_get_context(ctx, idx);
            
            duk_push_heap_stash(ctx);
            duk_push_sprintf(ctx, "0x%x",_jsContext);
            duk_dup(ctx, -3);
            duk_put_prop(ctx, -3);
            duk_pop_2(ctx);
            
            PushWeakObject(_jsContext, this);
            duk_put_global_string(_jsContext, "page");
            
            kk::Openlib<kk::Container *>::openlib(_jsContext, this);
            kk::Openlib<kk::ui::Page *>::openlib(_jsContext, this);
        }
        
        Page::~Page() {
            
            {
                duk_context * ctx = _app->jsContext();
                duk_push_heap_stash(ctx);
                duk_push_sprintf(ctx, "0x%x",_jsContext);
                duk_del_prop(ctx, -2);
                duk_pop(ctx);
            }
            
            duk_destroy_heap(_jsContext);
            
            kk::Log("[Page] [dealloc]");
            
        }
        
        kk::DispatchQueue * Page::queue() {
            return _app->Context::queue();
        }
        
        void Page::set(kk::Object * object) {
            auto i = _objects.find(object);
            if(i == _objects.end()) {
                _objects[object] = object;
            }
        }
        
        void Page::remove(kk::Object * object) {
            auto i = _objects.find(object);
            if(i != _objects.end()) {
                _objects.erase(i);
            }
        }
        
        kk::Object * Page::get(kk::Object * object) {
            auto i = _objects.find(object);
            if(i != _objects.end()) {
                return i->second;
            }
            return nullptr;
        }
        
        duk_context * Page::jsContext() {
            return _jsContext;
        }
        
        App * Page::app() {
            return _app;
        }
        
        View * Page::view() {
            return _view;
        }
        
        void Page::setSize(Size & size) {
            if(_size.width != size.width || _size.height != size.height) {
                Strong<Event> e = new Event();
                emit("resize", e);
            }
            _size = size;
        }
        
        Size Page::size() {
            return _size;
        }
        
        Float Page::width() {
            return _size.width;
        }
        
        Float Page::height() {
            return _size.height;
        }
        
        void Page::setOptions(kk::Object * data) {
            Strong<Event> e = new Event();
            e->setData(data);
            emit("options", e);
        }
        
        void Page::addLibrary(kk::CString name,kk::Any & value) {
            (*_librarys)[name] = value;
        }
        
        void Page::run(kk::CString path , kk::TObject<kk::String,kk::String> * query) {

            
            kk::String code("(function(path,query");
            
            std::vector<kk::Any> vs;
            
            _librarys->forEach([&code,&vs](kk::Any &value,kk::Any & key)->void{
                code.append(",");
                code.append((kk::CString)key);
                vs.push_back(value);
            });
            
            code.append("){");
        
            code.append(_app->getTextContent(path));
            
            code.append("})");
            
            duk_context * ctx = jsContext();
            
            duk_push_string(ctx, path);
            duk_compile_string_filename(ctx, 0, code.c_str());
            
            if(duk_is_function(ctx, -1)) {
                
                if(duk_pcall(ctx, 0) == DUK_EXEC_SUCCESS) {
                    
                    duk_push_string(ctx, path);
                    PushObject(ctx, query);
                    
                    auto i = vs.begin();
                    
                    while (i != vs.end()) {
                        PushAny(ctx, * i);
                        i ++;
                    }
                    
                    if(duk_pcall(ctx, 2 + vs.size()) != DUK_EXEC_SUCCESS) {
                        Error(ctx, -1, "[Page::run] ");
                    }
                    
                } else {
                    Error(ctx, -1, "[Page::run] ");
                }
                
            }
            
            duk_pop(ctx);
            
        }
        
        void Page::Openlib() {
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<Page>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutProperty<Page,App *>(ctx, -1, "app", &Page::app);
                    
                    kk::PutProperty<Page,View *>(ctx, -1, "view", &Page::view);
                    
                    kk::PutMethod<Page,void,Object *>(ctx, -1, "set", &Page::set);
                    
                    kk::PutMethod<Page,void,Object *>(ctx, -1, "remove", &Page::remove);
                    
                    kk::PutProperty<Page,Float>(ctx, -1, "width", &Page::width);
                    
                    kk::PutProperty<Page,Float>(ctx, -1, "height", &Page::height);
                    
                    kk::PutMethod<Page,void,Object *>(ctx, -1, "setOptions", &Page::setOptions);
                    
                });
                
            });
            
        }
        
    }
    
}

