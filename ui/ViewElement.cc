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
     
        ViewContext::ViewContext(App * app):LayoutContext(),_app(app) {
            
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
        
        void ViewContext::obtainView(ViewElement * element) {
            if(_view == nullptr || element == nullptr) {
                return;
            }
            element->obtainView(this);
        }
        
        ViewElement::ViewElement(Document * document,CString name, ElementKey elementId):kk::LayoutElement(document,name,elementId) {
            
        }
        
        void ViewElement::onLayout(LayoutContext * context) {
            LayoutElement::onLayout(context);
            
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
                
                ViewContext * v = dynamic_cast<ViewContext *>(context);
                
                if(v != nullptr) {
                    obtainChildrenView(v);
                }
                
            }
            
            
            
        }
        
        void ViewElement::setContentOffset(ViewContext * context,Float x,Float y) {
            _contentOffset.x = x;
            _contentOffset.y = y;
            obtainChildrenView(context);
        }
        
        View * ViewElement::view() {
            return _view;
        }
        
        void ViewElement::obtainChildrenView(ViewContext * context) {
            
            if(_view == nullptr) {
                return;
            }
            
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
            
            _view->removeRecycleViews();
            
        }
        
        void ViewElement::obtainView(ViewContext * context) {
            
            if(_view != nullptr) {
                return;
            }
            
            View * v = nullptr;
            
            ViewElement * p = dynamic_cast<ViewElement *>(parent());
            
            if(p == nullptr) {
                _view = context->view();
            } else {
                
                v = p->view();
                
                if(v == nullptr) {
                    return;
                }
                
                _view = v->obtainView(get("reuse"));
                
                if(_view == nullptr) {
                    Strong<View> vv = createView(context);
                    _view = (View *) vv;
                }
                
            }

            if(_view != nullptr){
                
                View * p = _view->parent();
                
                if(v != nullptr && (p == nullptr || p != v)) {
                    
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
                
                obtainChildrenView(context);
                
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
            Strong<View> v = app->createView(name(), nullptr);
            return v;
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
        
        void ViewElement::onEvent(ViewContext * context,CString name,Event * event) {
            if(kk::CStringEqual(name, "scroll")) {
                if(_view != nullptr) {
                    Point p = _view->contentOffset();
                    setContentOffset(context, p.y, p.y);
                }
            }
            emit(name, event);
        }
        void ViewElement::onObtainView(ViewContext * context,View * view) {
            
            kk::Weak<ViewContext> v = context;
            kk::Weak<ViewElement> e = this;

            view->on("*", new kk::TFunction<void,kk::CString,Event *>([v,e](kk::CString name,Event * event)->void{
                ViewContext * context = v.operator->();
                ViewElement * element = e.operator->();
                if(context && element) {
                    kk::Strong<kk::ElementEvent> e = new kk::ElementEvent(element);
                    e->setData(event->data());
                    element->onEvent(context,name, e);
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
                    kk::PutMethod<ViewElement,void,ViewContext *>(ctx, -1, "obtainView", &ViewElement::obtainView);
                    kk::PutMethod<ViewElement,void>(ctx, -1, "recycleView", &ViewElement::recycleView);
                });
                
                kk::PushClass<ViewContext,App *>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutProperty<ViewContext,View *>(ctx, -1, "view", &ViewContext::view,&ViewContext::setView);
                    kk::PutMethod<ViewContext,void,ViewElement *>(ctx, -1, "obtainView", &ViewContext::obtainView);
                    
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
