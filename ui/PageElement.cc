//
//  PageElement.cc
//  Ker
//
//  Created by zhanghailong on 2019/1/21.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "PageElement.h"

namespace kk {
    
    namespace ui {
    
        PageElement::PageElement(Document * document,CString name, ElementKey elementId):ViewElement(document,name,elementId) {
            
        }
        
        PageElement::~PageElement() {
            if(_page != nullptr) {
                _page->off("component.*", (EventFunction *) _onEvent);
            }
        }
        
        kk::Boolean PageElement::layout(LayoutContext * context) {
            return true;
        }
        
        void PageElement::changedKey(CString key) {
            ViewElement::changedKey(key);
            if(kk::CStringEqual(key, "path")) {
                if(_view != nullptr) {
                    reloadPage();
                }
            }
        }
        
        void PageElement::reloadPage() {
            
            if(_view != nullptr && _page != nullptr) {
                
                kk::Strong<App> app = _page->app();
                
                unloadPage();
                
                loadPage(app, _view);
                
            }
            
        }
        
        void PageElement::loadPage(App * app,View * view) {
            
            kk::CString path = get("path");
            
            if(app != nullptr && path != nullptr && !kk::CStringEqual(path, "")) {
                
                kk::Strong<Page> v = app->createPage("component");
                _page = (Page *) v;
                
                if(_onEvent == nullptr) {
                    _onEvent = new EventFunction([this](kk::CString name,kk::Event * event)->void{
                        kk::Strong<ElementEvent> e = new ElementEvent(this);
                        e->setData(event->data());
                        this->ViewElement::emit(name + 10, e);
                    });
                }
                
                v->on("component.*", (EventFunction *) _onEvent);
                
                kk::Strong<TObject<kk::String, kk::String>> query = new TObject<kk::String, kk::String>();
                
                {
                    auto & m = attributes();
                    auto i = m.begin();
                    auto e = m.end();
                    while(i != e) {
                        (*query)[i->first] = i->second;
                        i ++;
                    }
                }
                
                v->run(path, (TObject<kk::String, kk::String> *) query);
                
                v->setView(view);
                
                Size size = {frame.size.width,frame.size.height};
                
                v->setSize(size);
                
                v->ready();
                
                kk::Strong<Event> e = new Event();
                ViewElement::emit("load", e);
            }
            
        }
        
        void PageElement::unloadPage() {
            if(_page != nullptr) {
                kk::Strong<Event> e = new Event();
                ViewElement::emit("unload", e);
                _page->off("component.*", (EventFunction *) _onEvent);
                _page = nullptr;
            }
        }
        
        Page * PageElement::page() {
            return _page;
        }
        
        void PageElement::emit(kk::CString name,Event * event) {
            ViewElement::emit(name, event);
            if(_page != nullptr) {
                _page->emit(name, event);
            }
        }
        
        void PageElement::onObtainView(ViewContext * context,View * view) {
            ViewElement::onObtainView(context, view);
            
            App * app = context->app();
            
            if(app != nullptr) {
                loadPage(app, view);
            }
            
        }
        
        void PageElement::onRecycleView(View * view) {
            
            unloadPage();
            
            ViewElement::onRecycleView(view);
        }
        
        void PageElement::Openlib() {
            
            library("page");
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<PageElement>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutProperty<PageElement,Page *>(ctx, -1, "page", &PageElement::page);
                    
                });
                
            });
            
        }
        
        void PageElement::library(kk::CString name) {
            Document::library(name, [](Document * document,kk::CString name,ElementKey elementId)->Element *{
                return new PageElement(document,name,elementId);
            });
        }
        
    }
    
}
