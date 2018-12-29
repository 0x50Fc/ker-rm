//
//  view.cpp
//  kk
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <ui/view.h>
#include <ui/app.h>

namespace kk {
    
    namespace ui {
    
        AttributedText::AttributedText() {
            
        }
        
        AttributedText::~AttributedText() {
            
        }
        
        void AttributedText::clear() {
            _spans.clear();
        }
        
        void AttributedText::appendImage(kk::ui::Image * image,kk::Uint width,kk::Uint height,kk::ui::Float top,kk::ui::Float left,kk::ui::Float bottom,kk::ui::Float right) {
            append(image,width,height,{top,left,bottom,right});
        }
        
        void AttributedText::append(kk::CString text,kk::ui::Font font,kk::ui::Color color) {
            AttributedTextSpan v = {AttributedTextSpanTypeText,text};
            v.font = font;
            v.color = color;
            _spans.push_back(v);
        }
        
        void AttributedText::append(kk::ui::Image * image,kk::Uint width,kk::Uint height,kk::ui::Edge margin) {
            AttributedTextSpan v = {AttributedTextSpanTypeImage};
            v.image = image;
            v.width = width;
            v.height = height;
            v.margin = margin;
            _spans.push_back(v);
        }
        
        std::vector<AttributedTextSpan> & AttributedText::spans() {
            return _spans;
        }
        

        void AttributedText::Openlib() {
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushClass<AttributedText>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutMethod<AttributedText,void>(ctx, -1, "clear", &AttributedText::clear);
                    kk::PutMethod<AttributedText,void,kk::CString,kk::ui::Font,kk::ui::Color>(ctx, -1, "appendText", &AttributedText::append);
                    kk::PutMethod<AttributedText,void,kk::ui::Image *,kk::Uint,kk::Uint,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "appendImage", &AttributedText::appendImage);
                });
                
            });
            
        }
        
        Canvas::Canvas(View * view,DispatchQueue * queue,App * app,kk::Uint64 canvasId):_view(view),_queue(queue),_app(app),_canvasId(canvasId),_width(0),_height(0),_resize(false) {
            if(view != nullptr) {
                Rect & frame = view->frame();
                setWidth(frame.size.width);
                setHeight(frame.size.height);
            }
        }
        
        Canvas::Canvas(DispatchQueue * queue):Canvas(nullptr,queue,nullptr,0) {
            
        }
        
        Canvas::~Canvas() {
            App * app = _app;
            if(app != nullptr) {
                kk::Uint64 id = _canvasId;
                app->queue()->async([app,id]()->void{
                    app->removeCanvas(id);
                });
            }
        }
        
        
        Uint Canvas::width() {
            return _width;
        }
        
        void Canvas::setWidth(Uint v) {
            if(_width != v) {
                _width = v;
                _resize = true;
            }
        }
        
        Uint Canvas::height() {
            return _height;
        }
        
        void Canvas::setHeight(Uint v) {
            if(v != _height) {
                _height = v;
                _resize = true;
            }
        }
        
        DispatchQueue * Canvas::queue() {
            return _queue;
        }
        
        App * Canvas::app() {
            return _app;
        }
        
        View * Canvas::view() {
            return _view;
        }
        
        kk::Uint64 Canvas::canvasId() {
            return _canvasId;
        }
        
        Strong<Object> Canvas::getContext(kk::CString name) {
            
            if(name == nullptr ){
                return nullptr;
            }
            
            if(_width == 0 || _height == 0) {
                return nullptr;
            }
            
            if(strcmp(name, "2d") == 0) {
                
                kk::Strong<kk::ui::CG::Context> v = (kk::ui::CG::Context *) _context;
                
                if(v == nullptr || _resize) {
                    v = createCGContext();
                    _context = v;
                } else {
                    v->clear();
                }
                
                if(_view != nullptr) {
                    kk::Weak<Canvas> canvas = this;
                    _queue->async([v,canvas]()->void {
                        kk::Strong<Canvas> c = canvas.operator->();
                        if(c != nullptr) {
                            kk::Strong<App> app = c->app();
                            if(app != nullptr) {
                                kk::Strong<CanvasDisplayCGContextCommand> cmd = new CanvasDisplayCGContextCommand();
                                cmd->canvasId = c->canvasId();
                                cmd->viewId = c->view()->viewId();
                                cmd->context = v.operator->();
                                app->execCommand(cmd);
                            }
                        }
                    });
                }
                
            }
            
            _resize = false;
            
            return _context;
            
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
        
        
        View::View(kk::CString name,ViewConfiguration * configuration,App * app,kk::Uint64 viewId):_configuration(configuration),_app(app),_viewId(viewId) {
            
            {
                kk::Strong<ViewCreateCommand> cmd = new ViewCreateCommand();
                cmd->name = name;
                cmd->configuration = configuration;
                cmd->viewId = viewId;
                app->execCommand(cmd);
            }
        }
        
        View::View(kk::Native * native,Rect & frame,App * app,kk::Uint64 viewId):_app(app),_viewId(viewId),_frame(frame) {
            
            {
                kk::Strong<ViewNativeCreateCommand> cmd = new ViewNativeCreateCommand();
                cmd->native = native;
                cmd->viewId = viewId;
                app->execCommand(cmd);
            }
            
        }
        
        View::~View() {
            App * app = _app;
            if(app != nullptr){
                {
                    kk::Strong<ViewDeleteCommand> cmd = new ViewDeleteCommand();
                    cmd->viewId = _viewId;
                    app->execCommand(cmd);
                }
                app->removeView(_viewId);
            }
        }
        
        Rect & View::frame() {
            return _frame;
        }
        
        kk::Strong<Canvas> View::createCanvas(Worker * worker) {
            
            if(_app == nullptr) {
                return nullptr;
            }
            
            if(worker == nullptr || worker->context() == nullptr) {
                return _app->createCanvas(this, _app->queue());
            }
            
            DispatchQueue * queue = worker->context()->queue();
            kk::Strong<Canvas> canvas = _app->createCanvas(this, queue);
            kk::Strong<Worker> w = worker;
            
            queue->async([canvas,w]()->void{
                
                Worker * worker = w.operator->();
                Canvas * c = canvas.operator->();
                
                duk_context * ctx = worker->context()->jsContext();
                
                duk_get_global_string(ctx, "oncanvas");
                
                if(duk_is_function(ctx, -1)) {
                    
                    kk::PushObject(ctx, c);
                    
                    if(::duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
                        Error(ctx, -1, "[View::createCanvas]");
                    }
                    
                }
                
                duk_pop(ctx);
                
            });
            
            return nullptr;
        }
        
        void View::evaluateJavaScript(kk::CString code) {
            kk::Strong<ViewEvaluateJavaScriptCommand> cmd = new ViewEvaluateJavaScriptCommand();
            cmd->viewId = _viewId;
            if(code != nullptr) {
                cmd->code = code;
            }
            _app->execCommand(cmd);
        }
        
        void View::setContent(kk::CString content,kk::CString contentType,kk::CString basePath) {
            kk::Strong<ViewSetContentCommand> cmd = new ViewSetContentCommand();
            cmd->viewId = _viewId;
            if(content != nullptr){
                cmd->content = content;
            }
            if(contentType != nullptr) {
                cmd->contentType = contentType;
            }
            if(basePath != nullptr) {
                cmd->basePath = basePath;
            }
            _app->execCommand(cmd);
        }
        
        void View::setAttributedText(AttributedText * text) {
            kk::Strong<ViewSetAttributedTextCommand> cmd = new ViewSetAttributedTextCommand();
            cmd->viewId = _viewId;
            cmd->text = text;
            _app->execCommand(cmd);
        }
        
        void View::set(kk::CString name,kk::CString value) {
            kk::Strong<ViewSetCommand> cmd = new ViewSetCommand();
            cmd->viewId = _viewId;
            if(name != nullptr) {
                cmd->name = name;
            }
            if(value != nullptr) {
                cmd->value = value;
            }
            _app->execCommand(cmd);
        }
        
        void View::setFrame(Rect & frame) {
            _frame = frame;
            kk::Strong<ViewSetFrameCommand> cmd = new ViewSetFrameCommand();
            cmd->viewId = _viewId;
            cmd->frame = frame;
            _app->execCommand(cmd);
        }
        
        void View::setContentSize(Size & size) {
            kk::Strong<ViewSetContentSizeCommand> cmd = new ViewSetContentSizeCommand();
            cmd->viewId = _viewId;
            cmd->size = size;
            _app->execCommand(cmd);
        }
        
        void View::setContentOffset(Point & offset,kk::Boolean animated) {
            _contentOffset = offset;
            kk::Strong<ViewSetContentOffsetCommand> cmd = new ViewSetContentOffsetCommand();
            cmd->viewId = _viewId;
            cmd->offset = offset;
            cmd->animated = animated;
            _app->execCommand(cmd);
        }
        
        kk::Uint64 View::viewId() {
            return _viewId;
        }
        
        void View::addSubview(View * view,SubviewPosition position) {
            if(view == nullptr){
                return;
            }
            view->View::removeView();
            view->_parent = this;
            _subviews[view] = view;
            
            kk::Strong<ViewAddsubviewCommand> cmd = new ViewAddsubviewCommand();
            cmd->viewId = _viewId;
            cmd->subviewId = view->_viewId;
            cmd->position = position;
            _app->execCommand(cmd);
        }
        
        void View::removeView() {
            
            View * p = _parent;
            
            if(p != nullptr) {
                
                kk::Strong<App> app = (App *) _app;
                kk::Uint64 viewId = _viewId;
                
                _parent = nullptr;

                auto i = p->_subviews.find(this);
                if(i != p->_subviews.end()) {
                    p->_subviews.erase(i);
                }
                
                kk::Strong<ViewRemoveCommand> cmd = new ViewRemoveCommand();
                cmd->viewId = viewId;
                app->execCommand(cmd);
            }
        }
        
        void View::removeAllSubviews() {
            
            kk::Strong<ViewRemoveCommand> cmd = new ViewRemoveCommand();
            
            auto i = _subviews.begin();
            
            while(i != _subviews.end()) {
                
                View * view = i->second;
                view->_parent = nullptr;
                
                cmd->viewId = view->viewId();
                
                _app->execCommand(cmd);
                
                i ++;
            }
            
            _subviews.clear();
            
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
        
        App * View::app() {
            return _app;
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
        
        void View::setContentOffset(Float x,Float y,kk::Boolean animated) {
            Point p = {x,y};
            setContentOffset(p, animated);
        }
        
        
        Point View::contentOffset() {
            return _contentOffset;
        }
        
        void View::dispatchCommand(ViewCommand * command) {
            {
                ViewSetContentOffsetCommand * v = dynamic_cast<ViewSetContentOffsetCommand *>(command);
                if(v != nullptr) {
                    _contentOffset = v->offset;
                    return ;
                }
            }
        }
        
        void View::Openlib() {
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<View>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutMethod<View,void,kk::CString,kk::CString>(ctx, -1, "set", &View::set);
                    kk::PutMethod<View,void,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "setFrame", &View::setFrame);
                    kk::PutMethod<View,void,kk::ui::Float,kk::ui::Float>(ctx, -1, "setContentSize", &View::setContentSize);
                    kk::PutMethod<View,void,kk::ui::Float,kk::ui::Float,kk::Boolean>(ctx, -1, "setContentOffset", &View::setContentOffset);
                    kk::PutMethod<View,void,View *,SubviewPosition>(ctx, -1, "addSubview", &View::addSubview);
                    kk::PutMethod<View,void>(ctx, -1, "removeView", &View::removeView);
                    kk::PutMethod<View,void,CString>(ctx, -1, "evaluateJavaScript", &View::evaluateJavaScript);
                    kk::PutMethod<View,void,kk::CString,kk::CString,kk::CString>(ctx, -1, "setContent", &View::setContent);
                    kk::PutMethod<View,void,kk::ui::AttributedText *>(ctx, -1, "setAttributedText", &View::setAttributedText);
                    kk::PutMethod<View,void,kk::ui::Image *>(ctx, -1, "setImage", &View::setImage);
                    kk::PutProperty<View,Point>(ctx, -1, "contentOffset", &View::contentOffset);
                    kk::PutStrongMethod<View,Canvas,Worker *>(ctx, -1, "createCanvas", &View::createCanvas);
                    
                });
                
            });
            
        }
        
        void View::setImage(Image * image) {
            if(_image != nullptr && _onImageLoadFunc != nullptr) {
                _image->off("load", (kk::TFunction<void,kk::CString, Event *> *) _onImageLoadFunc);
            }
            _image = image;
            if(image == nullptr) {
                kk::Strong<ViewSetImageCommand> cmd = new ViewSetImageCommand();
                cmd->viewId = _viewId;
                cmd->image = nullptr;
                _app->execCommand(cmd);
            } else if(image->state() == ImageStateLoaded) {
                kk::Strong<ViewSetImageCommand> cmd = new ViewSetImageCommand();
                cmd->viewId = _viewId;
                cmd->image = image;
                _app->execCommand(cmd);
            } else if(image->state() != ImageStateError) {
                if(_onImageLoadFunc == nullptr) {
                    kk::Weak<View> v = this;
                    kk::Weak<App> app = (App *) _app;
                    kk::Uint64 viewId = _viewId;
                    _onImageLoadFunc = new kk::TFunction<void, kk::CString,Event *>([v,image,app,viewId](kk::CString name,Event *event)->void{
                        kk::Strong<View> vv = v.operator->();
                        if(vv != nullptr && vv->_image == image && app.operator->() != nullptr) {
                            kk::Strong<ViewSetImageCommand> cmd = new ViewSetImageCommand();
                            cmd->viewId = viewId;
                            cmd->image = image;
                            app->execCommand(cmd);
                        }
                    });
                }
                _image->on("load", (kk::TFunction<void, kk::CString,Event *> *) _onImageLoadFunc);
            } else {
                kk::Strong<ViewSetImageCommand> cmd = new ViewSetImageCommand();
                cmd->viewId = _viewId;
                cmd->image = nullptr;
                _app->execCommand(cmd);
            }
        }
        
        void WebViewConfiguration::addUserScript(kk::CString code,WebViewUserScriptInjectionTime injectionTime) {
            _userScripts.push_back({code,injectionTime});
        }
        
        void WebViewConfiguration::addUserAction(kk::CString pattern,WebViewActionPolicy policy) {
            _userActions.push_back({pattern,policy});
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
                    kk::PutMethod<WebViewConfiguration,void,kk::CString,kk::Uint>(ctx, -1, "addUserAction", &WebViewConfiguration::addUserAction);
                    
                });
                
            });
            
        }
        
    }
    
}
