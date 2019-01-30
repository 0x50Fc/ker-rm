//
//  ViewElement.c
//  Ker
//
//  Created by zhanghailong on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "ViewElement.h"
#include <document/BlockElement.h>

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
        
        Page * ViewContext::page() {
            return _page;
        }
        
        void ViewContext::setPage(Page * page) {
            _page = page;
        }
        
        void ViewContext::obtainView(ViewElement * element) {
            if(_view == nullptr || element == nullptr) {
                return;
            }
            element->obtainView(this);
        }
        
        ViewElement::ViewElement(Document * document,CString name, ElementKey elementId):kk::LayoutElement(document,name,elementId),_obtaining(false),_autoLevelId(0),_levelId(0) {
            
        }
        
        void ViewElement::onLayout(LayoutContext * context) {
            LayoutElement::onLayout(context);
            
            if(_view != nullptr) {
                onViewLayout();
            }
 
        }
        
        void ViewElement::onDidAddChildren(Element * element) {
            
            ViewElement * e = dynamic_cast<ViewElement *>(element);
            
            if(e != nullptr) {
                e->_levelId = ++ _autoLevelId;
            }
            
            LayoutElement::onDidAddChildren(element);
        }
        
        void ViewElement::onWillRemoveChildren(Element * element) {
            LayoutElement::onWillRemoveChildren(element);
            
            ViewElement * e = dynamic_cast<ViewElement *>(element);
            
            if(e != nullptr) {
                e->_levelId = 0;
            }
            
        }
        
        kk::String ViewElement::reuse() {
            kk::CString v = get("reuse");
            if(v == nullptr) {
                ViewElement * p = dynamic_cast<ViewElement *>( parent() );
                if(p != nullptr) {
                    kk::String n = p->reuse();
                    if(!n.empty()) {
                        n.append("_");
                        n.append(_name);
                        n.append("_");
                        char fmt[255];
                        snprintf(fmt, sizeof(fmt), "%d",_levelId);
                        n.append(fmt);
                        return n;
                    }
                }
            }
            return v == nullptr ? kk::String() : v;
        }
        
        void ViewElement::setNeedLayout() {
            if(_view == nullptr) {
                return;
            }
            LayoutElement::setNeedLayout();
        }
        
        void ViewElement::onViewLayout() {
            
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
        
        void ViewElement::setContentOffset(ViewContext * context,Float x,Float y) {
            _contentOffset.x = x;
            _contentOffset.y = y;
            obtainChildrenView(context);
        }
        
        View * ViewElement::view() {
            return _view;
        }
        
        kk::Boolean ViewElement::canRecycleView() {
            if(_view == nullptr) {
                return false;
            }
            kk::String s = reuse();
            return !s.empty();
        }
        
        static void ViewElementObtainChildrenViewForEach(Element * p,std::function<void(ViewElement *)> && item) {
            
            p = p->firstChild();
            
            while(p) {
                {
                    kk::BlockElement * e = dynamic_cast<kk::BlockElement *>(p);
                    if(e) {
                        ViewElementObtainChildrenViewForEach(e,std::move(item));
                        p = p->nextSibling();
                        continue;
                    }
                }
                {
                    ViewElement * e = dynamic_cast<ViewElement *>(p);
                    if(e) {
                        item(e);
                        p = p->nextSibling();
                        continue;
                    }
                }
                p = p->nextSibling();
            }
            
        }
        
        void ViewElement::obtainChildrenView(ViewContext * context) {
            
            if(_view == nullptr) {
                return;
            }
           
            ViewElementObtainChildrenViewForEach(this,[&](ViewElement * e)->void{
                
                if(isVisibleChildren(e)) {
                    e->obtainView(context);
                } else if(e->canRecycleView()){
                    e->recycleView();
                    e->recycleChildrenView();
                }
                
            });
           
            _view->removeRecycleViews();
            
        }
        
        ViewElement * ViewElement::parentViewElement() {
            
            Element * p = parent();
            
            while(p) {
                
                {
                    ViewElement * e = dynamic_cast<ViewElement *>(p);
                    
                    if(e != nullptr) {
                        return e;
                    }
                }
                
                {
                    BlockElement * e = dynamic_cast<BlockElement *>(p);
                    
                    if(e != nullptr) {
                        p = p->parent();
                        continue;
                    }
                
                }
                
                break;
            }
            
            return nullptr;
        }
        
        void ViewElement::obtainView(ViewContext * context) {
            
            if(_view == nullptr) {
                
                _obtaining = true;
                
                View * v = nullptr;
                
                ViewElement * p = parentViewElement();
                
                if(p == nullptr) {
                    _view = context->view();
                } else {
                    
                    v = p->view();
                    
                    if(v == nullptr) {
                        return;
                    }
                    
                    kk::String r = reuse();
                    
                    _view = v->obtainView(r.empty() ? nullptr : r.c_str());
                    
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
                    onViewLayout();
                }
                
            }

            if(_view != nullptr) {
                obtainViewKey(context);
                obtainChildrenView(context);
                _obtaining = false;
            }
            
        }
        
        void ViewElement::obtainViewKey(ViewContext * context) {
            if(_view != nullptr) {
                auto i = _changedKeys.begin();
                auto e = _changedKeys.end();
                while(i != e) {
                    auto & key = * i;
                    kk::CString v = get(key.c_str());
                    setViewKey(context, _view, key.c_str(), v);
                    i ++;
                }
                _changedKeys.clear();
            }
        }
        
        void ViewElement::recycle() {
            if(_view != nullptr) {
                _view->removeView();
            }
            recycleView();
            LayoutElement::recycle();
        }
        
        void ViewElement::recycleView() {
            if(_view != nullptr) {
                View * p = _view->parent();
                if(p != nullptr) {
                    kk::String r = reuse();
                    p->recycleView(_view, r.empty() ? nullptr : r.c_str());
                } else {
                    _view->removeView();
                }
                onRecycleView(_view);
                _view = nullptr;
                keys(_changedKeys);
            }
        }
        
        void ViewElement::recycleChildrenView() {
            Element * p = firstChild();
            while(p){
                ViewElement * e = dynamic_cast<ViewElement *>(p);
                if(e != nullptr) {
                    e->recycleView();
                    e->recycleChildrenView();
                }
                p = p->nextSibling();
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
                    setContentOffset(context, p.x, p.y);
                }
            } else if(kk::CStringEqual(name, "hover")) {
                addStatus("hover");
                changedStatus();
                obtainView(context);
            } else if(kk::CStringEqual(name, "out")) {
                removeStatus("hover");
                changedStatus();
                obtainView(context);
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
                    e->setDataSet(element->dataSet());
                    element->onEvent(context,name, e);
                }
            }));
            
        }
        
        void ViewElementSetStringValue(ViewContext * context,View * view,CString key, CString value) {
            view->set(key, value);
        }
        
        void ViewElementSetPixelValue(ViewContext * context,View * view,CString key, CString value) {
            if(value == nullptr) {
                view->set(key, value);
            } else {
                Pixel p(value);
                Float value = context->value(p, 0, 0);
                char fmt[255];
                snprintf(fmt, sizeof(fmt), "%gpx",value);
                view->set(key, fmt);
            }
        }
        
        void ViewElement::setViewKey(ViewContext * context,View * view,CString key, CString value) {
            
            if(kk::CStringEqual(key, "enabled")) {
                if(value && !kk::CStringEqual(value, "false")) {
                    removeStatus("disabled");
                    changedStatus();
                } else {
                    addStatus("disabled");
                    changedStatus();
                }
            }
            
            if(kk::CStringEqual(key, "scrollToTop") && value && !kk::CStringEqual(value, "false")) {
                view->scrollToTop(!_obtaining);
            } else if(kk::CStringEqual(key, "scrollToBottom") && value && !kk::CStringEqual(value, "false")) {
                view->scrollToBottom(!_obtaining);
            } else if(kk::CStringEqual(key, "scrollToLeft") && value && !kk::CStringEqual(value, "false")) {
                view->scrollToLeft(!_obtaining);
            } else if(kk::CStringEqual(key, "scrollToRight") && value && !kk::CStringEqual(value, "false")) {
                view->scrollToRight(!_obtaining);
            } else if(kk::CStringEqual(key, "border-radius")) {
                ViewElementSetPixelValue(context, view, key, value);
            } else if(kk::CStringEqual(key, "border-width")) {
                ViewElementSetPixelValue(context, view, key, value);
            } else if(kk::CStringEqual(key, "padding")) {
                kk::Edge e(value);
                view->setPadding(context->value(e.left, 0, 0),
                                 context->value(e.top, 0, 0),
                                 context->value(e.right, 0, 0),
                                 context->value(e.bottom, 0, 0));
            } else {
                ViewElementSetStringValue(context, view, key, value);
            }
        }
        
        void ViewElement::onRecycleView(View * view) {
            view->off("*", (kk::TFunction<void,kk::CString,Event *> *) nullptr);
        }
        
        void ViewElement::changedKey(CString key) {
            LayoutElement::changedKey(key);
             _changedKeys.insert(key);
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
                    kk::PutProperty<ViewContext,Page *>(ctx, -1, "page", &ViewContext::page,&ViewContext::setPage);
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
