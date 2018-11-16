//
//  view.cpp
//  kk
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <ui/view.h>

namespace kk {
    
    namespace ui {
    
        
        Canvas::Canvas(DispatchQueue * queue):_queue(queue),_width(0),_height(0) {
            
        }
        
        Uint Canvas::width() {
            return _width;
        }
        
        void Canvas::setWidth(Uint v) {
            _width = v;
        }
        
        Uint Canvas::height() {
            return _height;
        }
        
        void Canvas::setHeight(Uint v) {
            _height = v;
        }
        
        DispatchQueue * Canvas::queue() {
            return _queue;
        }
        
        void Canvas::Openlib() {
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<Canvas>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutStrongMethod<Canvas,Object,kk::CString>(ctx, -1, "getContext", &Canvas::getContext);
                    kk::PutStrongMethod<Canvas,Image>(ctx, -1, "toImage", &Canvas::toImage);
                    kk::PutProperty<Canvas,Uint>(ctx, -1, "width", &Canvas::width,&Canvas::setWidth);
                    kk::PutProperty<Canvas,Uint>(ctx, -1, "height", &Canvas::height,&Canvas::setHeight);
                });
                
            });
            
        }
        
        
        View::View(ViewConfiguration * configuration,Context * context):_configuration(configuration),_context(context) {
            
        }
        
        void View::addSubview(View * view,SubviewPosition position) {
            view->View::removeView();
            view->_parent = this;
            _subviews[view] = view;
        }
        
        void View::removeView() {
            if(_parent != nullptr) {
                auto i = _parent->_subviews.find(this);
                if(i != _parent->_subviews.end()) {
                    _parent->_subviews.erase(i);
                }
                _parent = nullptr;
            }
        }
        
        void View::setFrame(Float x,Float y,Float width,Float height) {
            Rect r = {{x,y},{width,height}};
            setFrame(r);
        }
        
        void View::setContentSize(Float width,Float height) {
            Size s = {width,height};
            setContentSize(s);
        }
        
        kk::Strong<View> View::obtainView(kk::CString reuse) {
            
            auto i = _obtainViews.find(reuse);
            
            if(i != _obtainViews.end()) {
                auto q = i->second;
                auto n = q.begin();
                if(n != q.end()) {
                    kk::Strong<View> v = * n;
                    q.erase(n);
                    return v;
                }
            }
            
            return nullptr;
        }
        
        void View::recycleView(View * view,kk::CString reuse) {
            
            auto i = _obtainViews.find(reuse);
            
            if(i != _obtainViews.end()) {
                i->second.push_back(view);
            } else {
                _obtainViews[reuse] = {view};
            }
            
        }
        
        Context * View::context() {
            return _context;
        }
        
        void View::removeRecycleViews() {
            
            auto i = _obtainViews.begin();
            
            while(i != _obtainViews.end()) {
                
                auto q = i->second;
                auto n = q.begin();
                
                while(n != q.end()) {
                    View * view = *n;
                    view->removeView();
                    n ++;
                }
                
                i ++;
            }
            
        }
        
        ViewConfiguration * View::configuration() {
            return _configuration;
        }
        
        void View::Openlib() {
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<View>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutMethod<View,void,kk::CString,kk::CString>(ctx, -1, "set", &View::set);
                    kk::PutMethod<View,void,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "setFrame", &View::setFrame);
                    kk::PutMethod<View,void,kk::ui::Float,kk::ui::Float>(ctx, -1, "setContentSize", &View::setContentSize);
                    kk::PutMethod<View,void,View *,SubviewPosition>(ctx, -1, "addSubview", &View::addSubview);
                    kk::PutMethod<View,void>(ctx, -1, "removeView", &View::removeView);
                    kk::PutMethod<View,void,CString>(ctx, -1, "evaluateJavaScript", &View::evaluateJavaScript);
                    kk::PutProperty<View,Point>(ctx, -1, "contentOffset", &View::contentOffset);
                    kk::PutStrongMethod<View,Canvas,Worker *>(ctx, -1, "createCanvas", &View::createCanvas);
                    
                });
                
            });
            
        }
        
        void WebViewConfiguration::addUserScript(kk::CString code,WebViewUserScriptInjectionTime injectionTime) {
            _userScripts.push_back({code,injectionTime});
        }
        
        void WebViewConfiguration::addUserAction(kk::CString pattern,kk::CString name,WebViewActionPolicy policy) {
            _userActions.push_back({pattern,name,policy});
        }
        
        std::vector<WebViewUserScript> & WebViewConfiguration::userScripts() {
            return _userScripts;
        }
        
        std::vector<WebViewUserAction> & WebViewConfiguration::userActions() {
            return _userActions;
        }
        
        void WebViewConfiguration::Openlib() {
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushClass<WebViewConfiguration>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutMethod<WebViewConfiguration,void,kk::CString,kk::Uint>(ctx, -1, "addUserScript", &WebViewConfiguration::addUserScript);
                    kk::PutMethod<WebViewConfiguration,void,kk::CString,kk::CString,kk::Uint>(ctx, -1, "addUserAction", &WebViewConfiguration::addUserAction);
                    
                });
                
            });
            
        }
        
    }
    
}
