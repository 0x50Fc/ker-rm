//
//  app.cc
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <ui/page.h>
#include <typeinfo>

namespace kk {

    namespace ui {
        
        void addPageOpenlib(std::function<void(duk_context *,Page *)> && func) {
            kk::Openlib<kk::ui::Page *>::add(std::move(func));
        }
        
        Page::Page(App * app,kk::Uint64 pageId,kk::CString type):_app(app),_pageId(pageId),_librarys(new TObject<kk::String, kk::Any>()),_type(type) {
            
            app->queue()->sync([this]()->void{
                
                duk_context * ctx = _app->jsContext();
                
                _func = new kk::TFunction<void, kk::CString,kk::Event *>([this](kk::CString name,kk::Event * event)->void{
                    kk::String v("app.");
                    v.append(name);
                    emit(v.c_str(), event);
                });
                
                _app->on("*", (kk::TFunction<void, kk::CString,kk::Event *> *) _func);
                
                kk::Openlib<kk::Container *>::openlib(ctx, this);
                kk::Openlib<kk::ui::Page *>::openlib(ctx, this);
                
            });
        }
        
        Page::~Page() {

            if(_app != nullptr) {
                _app->removePage(_pageId);
                _app->off("*", (kk::TFunction<void, kk::CString,kk::Event *> *) _func);
            }
            
            if(_view != nullptr) {
                _view->removeRecycleViews();
                _view->removeAllSubviews();
            }
            
            auto jitContext = JITContext::current();
            
            if(_librarys != nullptr){
                auto i = _librarys->begin();
                auto e = _librarys->end();
                while(i != e) {
                    auto & v = i->second;
                    if(v.type == TypeObject || v.type == TypeFunction) {
                        Object * a = v.objectValue;
                        if(a != nullptr) {
                            jitContext->remove(a);
                        }
                    }
                    i ++;
                }
            }
            
            if(_app != nullptr) {
                
                duk_context * ctx = _app->jsContext();
            
                jitContext->remove(this);
                
                duk_gc(ctx, DUK_GC_COMPACT);
                
            }
            
            kk::Log("[Page] [dealloc]");
            
        }
        
        kk::DispatchQueue * Page::queue() {
            return _app->Context::queue();
        }
        
        void Page::unload() {
            kk::Strong<Event> e = new Event();
            emit("unload", e);
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
        
        App * Page::app() {
            return _app;
        }
        
        View * Page::view() {
            return _view;
        }
        
        void Page::setView(View * v) {
            _view = v;
        }
        
        View * Page::leftView() {
            return _leftView;
        }
        
        void Page::setLeftView(View * v) {
            _leftView = v;
            
            {
                kk::Strong<PageSetLeftViewCommand> cmd = new PageSetLeftViewCommand();
                cmd->pageId = _pageId;
                cmd->viewId = v == nullptr ? 0:v->viewId();
                cmd->appid = _app->appid();
                UI::main()->execCommand(cmd);
            }
            
        }
        
        View * Page::rightView() {
            return _rightView;
        }
        
        void Page::setRightView(View * v) {
            _rightView = v;
            
            {
                kk::Strong<PageSetRightViewCommand> cmd = new PageSetRightViewCommand();
                cmd->pageId = _pageId;
                cmd->viewId = v == nullptr ? 0:v->viewId();
                cmd->appid = _app->appid();
                UI::main()->execCommand(cmd);
            }
            
        }
        
        View * Page::titleView() {
            return _titleView;
        }
        
        void Page::setTitleView(View * v) {
            _titleView = v;
            
            {
                kk::Strong<PageSetTitleViewCommand> cmd = new PageSetTitleViewCommand();
                cmd->pageId = _pageId;
                cmd->viewId = v == nullptr ? 0:v->viewId();
                cmd->appid = _app->appid();
                UI::main()->execCommand(cmd);
            }
            
        }
        
        void Page::setSize(Size & size) {
            Size v = _size;
            _size = size;
            if(v.width != size.width || v.height != size.height) {
                Strong<Event> e = new Event();
                emit("resize", e);
            }
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
            {
                kk::Strong<PageSetOptionsCommand> cmd = new PageSetOptionsCommand();
                cmd->pageId = _pageId;
                cmd->data = data;
                cmd->appid = _app->appid();
                UI::main()->execCommand(cmd);
            }
        }
        
        void Page::addLibrary(kk::CString name,kk::Any & value) {
            (*_librarys)[name] = value;
        }
        
        void Page::close(kk::Boolean animated) {
            {
                kk::Strong<PageCloseCommand> cmd = new PageCloseCommand();
                cmd->pageId = _pageId;
                cmd->animated = animated;
                cmd->appid = _app->appid();
                UI::main()->execCommand(cmd);
            }
        }
        
        kk::Uint64 Page::pageId() {
            return _pageId;
        }
        
        void Page::run(kk::CString path , Object * query) {

            kk::String code("(function(page,path,query,require,global,module,exports");
            
            std::vector<kk::Any> vs;
            
            _librarys->forEach([&code,&vs](kk::Any &value,kk::Any & key)->void{
                code.append(",");
                code.append((kk::CString)key);
                vs.push_back(value);
            });
            
            code.append("){");
        
            code.append(_app->getTextContent(path));
            
            code.append("})");
            
            duk_context * ctx = _app->jsContext();
            
            duk_push_string(ctx, path);
            duk_compile_string_filename(ctx, 0, code.c_str());
            
            if(duk_is_function(ctx, -1)) {
                
                if(duk_pcall(ctx, 0) == DUK_EXEC_SUCCESS) {
                    
                    PushWeakObject(ctx, this);
                    duk_push_string(ctx, path);
                    PushObject(ctx, query);
                    kk::String basePath = CStringPathDirname(path);
                    duk_push_require(ctx, basePath.c_str(), (JSResource *) _app);
                    duk_push_global_object(ctx);
                    
                    duk_push_object(ctx);
                    duk_push_object(ctx);
                    duk_dup(ctx, -1);
                    duk_put_prop_string(ctx, -3, "exports");
                    
                    auto i = vs.begin();
                    
                    while (i != vs.end()) {
                        PushAny(ctx, * i);
                        i ++;
                    }
                    
                    if(duk_pcall(ctx, 7 + (duk_idx_t) vs.size()) != DUK_EXEC_SUCCESS) {
                        Error(ctx, -1, "[Page::run] ");
                    }
                    
                } else {
                    Error(ctx, -1, "[Page::run] ");
                }
                
            }
            
            duk_pop(ctx);
        
            duk_gc(ctx, DUK_GC_COMPACT);
            
        }
        
        void Page::ready() {
            kk::Strong<Event> e = new Event();
            emit("ready", e);
        }
        
        kk::CString Page::type() {
            return _type.c_str();
        }
        
        void Page::Openlib() {
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<Page>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutProperty<Page,App *>(ctx, -1, "app", &Page::app);
                    
                    kk::PutProperty<Page,View *>(ctx, -1, "view", &Page::view);
                    
                    kk::PutProperty<Page,View *>(ctx, -1, "leftView", &Page::leftView, &Page::setLeftView);
                    kk::PutProperty<Page,View *>(ctx, -1, "rightView", &Page::rightView, &Page::setRightView);
                    kk::PutProperty<Page,View *>(ctx, -1, "titleView", &Page::titleView, &Page::setTitleView);
                    
                    kk::PutMethod<Page,void,Object *>(ctx, -1, "set", &Page::set);
                    
                    kk::PutMethod<Page,void,Object *>(ctx, -1, "remove", &Page::remove);
                    
                    kk::PutProperty<Page,Float>(ctx, -1, "width", &Page::width);
                    
                    kk::PutProperty<Page,Float>(ctx, -1, "height", &Page::height);
                    
                    kk::PutMethod<Page,void,Object *>(ctx, -1, "setOptions", &Page::setOptions);
                    
                    kk::PutMethod<Page,void,kk::Boolean>(ctx, -1, "close", &Page::close);
                    
                    kk::PutProperty<Page,kk::Uint64>(ctx, -1, "id", &Page::pageId);
                    
                });
                
            });
            
        }
        
    }
    
}

