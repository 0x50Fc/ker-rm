//
//  ViewElement.c
//  Ker
//
//  Created by zhanghailong on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "ViewElement.h"

namespace kk {
 
    namespace ui {
     
        ViewContext::ViewContext(App * app):_app(app) {
            
        }
        
        App * ViewContext::app() {
            return _app;
        }
        
        View * ViewContext::view() {
            return _view;
        }
        
        void ViewContext::setView(View * view) {
            _view = view;
        }
        
        ViewElement::ViewElement(Document * document,CString name, ElementKey elementId):kk::LayoutElement(document,name,elementId) {
            
        }
        
        void ViewElement::onLayout(LayoutContext * context) {
            LayoutElement::onLayout(context);
            
            ViewContext * v = dynamic_cast<ViewContext *>(context);
            
            if(v != nullptr) {
                ViewElement * p = dynamic_cast<ViewElement *>(parent());
                if(p == nullptr) {
                    if(isVisible()) {
                        obtainView(v);
                    } else {
                        recycleView();
                    }
                } else {
                    if(p->isVisibleChildren(this)) {
                        obtainView(v);
                    } else {
                        recycleView();
                    }
                }
            }
            
            if(_view != nullptr) {
                
                Rect r = {{frame.origin.x,frame.origin.y},{frame.size.width,frame.size.height}};
                
                _view->setFrame(r);
                
                Size s = {contentSize.width,contentSize.height};
                
                if(kk::CStringEqual(get("overflow-x"), "hidden")) {
                    s.width = 0;
                } else {
                    s.width = MAX(s.width,r.size.width + 1);
                }
                
                if(kk::CStringEqual(get("overflow-y"), "hidden")) {
                    s.height = 0;
                } else {
                    s.height = MAX(s.height,r.size.height + 1);
                }
                
                _view->setContentSize(s);
                
            }
            
        }
        
        void ViewElement::setContentOffset(ViewContext * context,Float x,Float y) {
            _contentOffset.x = x;
            _contentOffset.y = y;
            Element * p = firstChild();
            while(p) {
                ViewElement * e = dynamic_cast<ViewElement *>(p);
                if(e) {
                    if(isVisibleChildren(e)) {
                        e->obtainView(context);
                    } else {
                        e->recycleView();
                    }
                }
                p = p->nextSibling();
            }
        }
        
        View * ViewElement::view() {
            return _view;
        }
        
        void ViewElement::obtainView(ViewContext * context) {
            
            if(_view != nullptr) {
                return;
            }
            
            View * v = nullptr;
            
            ViewElement * p = dynamic_cast<ViewElement *>(parent());
            
            if(p == nullptr) {
                v = context->view();
            } else {
                v = p->view();
            }
            
            if(v == nullptr) {
                return;
            }
            
            _view = v->obtainView(get("reuse"));
            
            if(_view == nullptr) {
                _view = createView(context);
            }
            
            if(_view != nullptr){
                
                View * p = _view->parent();
                
                if(p == nullptr || p != v) {
                    
                    if(p != nullptr) {
                        _view->removeView();
                    }
                    
                    if(kk::CStringEqual(get("target"), "back")) {
                        v->addSubview(_view, SubviewPositionBack);
                    } else {
                        v->addSubview(_view, SubviewPositionFront);
                    }
                    
                }
                
                onObtainView(context,_view);
            }
        }
        
        void ViewElement::recycleView() {
            if(_view != nullptr) {
                View * p = _view->parent();
                if(p != nullptr) {
                    p->recycleView(_view, get("reuse"));
                } else {
                    _view->removeView();
                }
                onRecycleView(_view);
                _view = nullptr;
            }
        }
        
        Strong<View> ViewElement::createView(ViewContext * context) {
            App * app = context->app();
            if(app == nullptr) {
                return nullptr;
            }
            return app->createView(get("#name"), nullptr);
        }
        
        kk::Boolean ViewElement::isVisibleChildren(ViewElement * element) {
            if(element == nullptr
               || element->hidden
               || element->frame.size.width <=0.0f
               || element->frame.size.height <=0.0f) {
                return false;
            }
            Float left = _contentOffset.x;
            Float top = _contentOffset.y;
            Float right = left + frame.size.width;
            Float bottom = top + frame.size.height;
            Float l = element->frame.origin.x;
            Float t = element->frame.origin.y;
            Float r = element->frame.origin.x + element->frame.size.width;
            Float b = element->frame.origin.y + element->frame.size.height;
            
            l = MAX(left,l);
            r = MIN(right,r);
            
            t = MAX(top,t);
            b = MIN(bottom,b);
            
            return r - l > 0.0f && b - t > 0.0f;
            
        }

        kk::Boolean ViewElement::isVisible() {
            return ! hidden && frame.size.width > 0.0f && frame.size.height > 0.0f;
        }
        
        void ViewElement::onObtainView(ViewContext * context,View * view) {
            
            kk::Weak<ViewContext> v = context;
            kk::Weak<ViewElement> e = this;
            
            view->on("scroll", new kk::TFunction<void,kk::CString,Event *>([v,e](kk::CString name,Event * event)->void{
                ViewContext * context = v.operator->();
                ViewElement * element = e.operator->();
                if(context && element) {
                    View * view = element->view();
                    if(view != nullptr) {
                        Point p = view->contentOffset();
                        element->setContentOffset(context, p.y, p.y);
                    }
                }
            }));
            
            view->on("*", new kk::TFunction<void,kk::CString,Event *>([v,e](kk::CString name,Event * event)->void{
                ViewContext * context = v.operator->();
                ViewElement * element = e.operator->();
                if(context && element) {
                    kk::Strong<kk::ElementEvent> e = new kk::ElementEvent(element);
                    e->setData(event->data());
                    element->emit(name, e);
                }
            }));
            
            {
                auto & m = attributes();
                auto i = m.begin();
                auto e = m.end();
                while(i != e) {
                    view->set(i->first.c_str(), i->second.c_str());
                    i ++;
                }
            }
            
        }
        
        void ViewElement::onRecycleView(View * view) {
            view->off("scroll", (kk::TFunction<void,kk::CString,Event *> *) nullptr);
            view->off("*", (kk::TFunction<void,kk::CString,Event *> *) nullptr);
        }
        
        void ViewElement::changedKey(CString key) {
            LayoutElement::changedKey(key);
            if(_view != nullptr) {
                _view->set(key, get(key));
            }
        }
        
        void ViewElement::Openlib() {
            
            library("view");
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<ViewElement>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutProperty<ViewElement,View *>(ctx, -1, "view", &ViewElement::view);
                    
                });
                
            });
            
            
        }
        
        static Element * ViewElementCreate(Document * document,kk::CString name,ElementKey elementId) {
            return new ViewElement(document,name,elementId);
        }
        
        void ViewElement::library(kk::CString name) {
            Document::library(name, ViewElementCreate);
        }
    }
    
}
