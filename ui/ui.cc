//
//  ui.cc
//  kk
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <ui/ui.h>
#include <ui/view.h>
#include <core/jit.h>

namespace kk {
    
    namespace ui {
    
        TextAlign TextAlignFromString(kk::CString string) {
            if(CStringEqual(string, "end")) {
                return TextAlignEnd;
            }
            if(CStringEqual(string, "center")) {
                return TextAlignCenter;
            }
            if(CStringEqual(string, "left")) {
                return TextAlignLeft;
            }
            if(CStringEqual(string, "right")) {
                return TextAlignRight;
            }
            return TextAlignStart;
        }
        
        kk::CString StringFromTextAlign(TextAlign v) {
            switch (v) {
                case TextAlignEnd:
                    return "end";
                case TextAlignCenter:
                    return "center";
                case TextAlignLeft:
                    return "left";
                case TextAlignRight:
                    return "right";
                default:
                    return "start";
            }
        }
        
        TextBaseline TextBaselineFromString(kk::CString string) {
            if(CStringEqual(string, "top")) {
                return TextBaselineTop;
            }
            if(CStringEqual(string, "hanging")) {
                return TextBaselineHanging;
            }
            if(CStringEqual(string, "middle")) {
                return TextBaselineMiddle;
            }
            if(CStringEqual(string, "ideographic")) {
                return TextBaselineIdeographic;
            }
            if(CStringEqual(string, "bottom")) {
                return TextBaselineBottom;
            }
            return TextBaselineAlphabetic;
        }
        
        kk::CString StringFromTextBaseline(TextBaseline v) {
            
            switch (v) {
                case TextBaselineTop:
                    return "top";
                case TextBaselineHanging:
                    return "hanging";
                case TextBaselineMiddle:
                    return "middle";
                case TextBaselineIdeographic:
                    return "ideographic";
                case TextBaselineBottom:
                    return "bottom";
                default:
                    return "alphabetic";
            }
        }
        
        Size::Size():width(0),height(0) {
            
        }
        
        Size::Size(Float width,Float height):width(width),height(height) {
            
        }
        
        
        Palette::Palette(std::initializer_list<std::pair<const kk::String,Color>> &&v):_values(v){
            
        }
        
        void Palette::set(kk::CString name,Color v) {
            _values[name] = v;
        }
        
        Color Palette::get(kk::CString name) {
            Color v = {0,0,0,0};
            std::map<kk::String,Color>::iterator i = _values.find(name);
            if(i != _values.end()) {
                return i->second;
            }
            return v;
        }
        
        Palette Palette::Default = {
            {"black",(0x000000)},
            {"red",(0xff0000)},
            {"white",(0xffffff)},
            {"green",(0x00ff00)},
            {"blue",(0x0000ff)},
            {"magenta",(0xff00ff)},
            {"yellow",(0xffff00)},
        };
        
        Edge::Edge():top(0),right(0),bottom(0),left(0) {
            
        }
        
        Edge::Edge(Float top,Float right,Float bottom,Float left):top(top),right(right),bottom(bottom),left(left) {
            
        }
        
        Edge::Edge(kk::CString v):Edge() {
            std::vector<kk::String> vs;
            kk::CStringSplit(v, " ", vs);
            Float a;
            auto i = vs.begin();
            if(i != vs.end()) {
                a = atof((*i).c_str());
                top = a;
                i ++;
                if(i != vs.end()) {
                    a = atof((*i).c_str());
                    right = a;
                    i ++;
                    if(i != vs.end()) {
                        a = atof((*i).c_str());
                        bottom = a;
                        i ++;
                        if(i != vs.end()) {
                            a = atof((*i).c_str());
                            left = a;
                        } else {
                            left = right;
                        }
                    } else {
                        bottom = top;
                        left = right;
                    }
                } else {
                    right = bottom = left = top;
                }
            }
        }
        
        Color::Color():r(0),g(0),b(0),a(0) {
            
        }

        Color::Color(Float r,Float g,Float b,Float a):r(r),g(g),b(g),a(a) {
            
        }
        
        Color::Color(kk::CString string):Color() {
            
            if(string != nullptr) {
                
                if(kk::CStringHasPrefix(string, "rgba(")) {
#ifdef KK_UI_FLOAT64
                    sscanf(string, "rgba(%lf,%lf,%lf,%lf)",&r,&g,&b,&a);
#else
                    sscanf(string, "rgba(%f,%f,%f,%f)",&r,&g,&b,&a);
#endif
                    r = r / 255.0f;
                    g = g / 255.0f;
                    b = b / 255.0f;
                    
                } else if(kk::CStringHasPrefix(string, "rgb(")) {
                    
#ifdef KK_UI_FLOAT64
                    sscanf(string, "rgba(%lf,%lf,%lf)",&r,&g,&b);
#else
                    sscanf(string, "rgba(%f,%f,%f)",&r,&g,&b);
#endif
                    r = r / 255.0f;
                    g = g / 255.0f;
                    b = b / 255.0f;
                    a = 1.0f;
                    
                } else if(kk::CStringHasPrefix(string, "#")) {
                    
                    size_t n = strlen(string);
                    
                    if(n == 4) {
                        Uint r = 0,g = 0,b = 0;
                        sscanf(string, "#%1x%1x%1x",&r,&g,&b);
                        this->r = (Float)(r << 4 | r) / 255.0f;
                        this->g = (Float)(g << 4 | r) / 255.0f;
                        this->b = (Float)(b << 4 | r) / 255.0f;
                        this->a = 1.0f;
                    } else if(n == 7) {
                        Uint r = 0,g = 0,b = 0;
                        sscanf(string, "#%2x%2x%2x",&r,&g,&b);
                        this->r = (Float)(r) / 255.0f;
                        this->g = (Float)(g) / 255.0f;
                        this->b = (Float)(b) / 255.0f;
                        this->a = 1.0f;
                    } else if(n == 9) {
                        Uint r = 0,g = 0,b = 0, a = 0;
                        sscanf(string, "#%2x%2x%2x%2x",&r,&g,&b,&a);
                        this->r = (Float)(r) / 255.0f;
                        this->g = (Float)(g) / 255.0f;
                        this->b = (Float)(b) / 255.0f;
                        this->a = (Float)(a) / 255.0f;
                    }
                    
                } else {
                    Color v = Palette::Default.get(string);
                    r = v.r;
                    g = v.g;
                    b = v.g;
                    a = v.a;
                }
            }
            
        }
        
        Font::Font(kk::CString v):Font() {
            std::vector<kk::String> items;
            kk::CStringSplit(v, " ", items);
            auto i = items.begin();
            while(i != items.end()) {
                kk::String & v = * i;
                if(CStringHasSuffix(v.c_str(), "px")) {
                    size = atof(v.c_str());
                } else if(v == "bold") {
                    weight = FontWeightBold;
                } else if(v == "italic") {
                    style = FontStyleItalic;
                } else {
                    family = v;
                }
                i ++;
            }
        }
        
        Color::Color(kk::Uint v):Color() {
            r = (0x0ff & (v >> 24)) / 255.0;
            g = (0x0ff & (v >> 16)) / 255.0;
            b = (0x0ff & (v >> 8)) / 255.0;
            a = (0x0ff & (v )) / 255.0;
        }
        

        Transform TransformIdentity = {
            1,0,
            0,1,
            0,0
        };
        
        Transform TransformTranslate(Transform t, Float tx, Float ty) {
            return {
                t.a,t.b,
                t.c,t.d,
                t.tx + tx,t.ty + ty
            };
        }
        
        Transform TransformScale(Transform t, Float sx, Float sy) {
            return {
                t.a * sx,t.b,
                t.c,t.d * sy,
                t.tx,t.ty
            };
        }
        
        Transform TransformRotate(Transform t, Float a) {
            Float ca = cos(a);
            Float sa = sin(a);
            return {
                t.a + ca,t.b + sa,
                t.c - sa,t.d + ca,
                t.tx,t.ty
            };
        }
        
        Transform TransformFromString(kk::CString v) {
            Transform r = TransformIdentity;
            std::vector<kk::String> items;
            kk::CStringSplit(v, " ", items);
            char x[64],y[64];
            auto i = items.begin();
            while(i != items.end()) {
                kk::String & v = * i;
                if(kk::CStringHasPrefix(v.c_str(), "translate(")) {
                    sscanf(v.c_str(), "translate(%[^\\,\\)]63s,%[^\\,\\)]63s)",x,y);
                    r = TransformTranslate(r, atof(x), atof(y));
                } else if(kk::CStringHasPrefix(v.c_str(), "scale(")) {
                    sscanf(v.c_str(), "scale(%[^\\,\\)]63s,%[^\\,\\)]63s)",x,y);
                    r = TransformScale(r, atof(x), atof(y));
                } else if(kk::CStringHasPrefix(v.c_str(), "rotate(")) {
                    sscanf(v.c_str(), "rotate(%[^\\,\\)]63s)",x);
                    r = TransformRotate(r, atof(x) * M_PI / 180.0 );
                }
                i ++;
            }
            return r;
        }
        
        static void Context_duk_fatal_function(void *udata, const char *msg) {
            kk::Log("[UI] [FATAL] %s",msg);
        }
        
        Context::Context(kk::CString basePath,kk::DispatchQueue * queue):EventEmitter(), _basePath(basePath),_queue(queue) {
            _jsContext = duk_create_heap(nullptr, nullptr, nullptr, nullptr, Context_duk_fatal_function);
            kk::Openlib<>::openlib(_jsContext);
            kk::Openlib<kk::Container *>::openlib(_jsContext, this);
        }
        
        Context::~Context() {
            JITContext::current()->remove(_jsContext);
            duk_destroy_heap(_jsContext);
        }
        
        void Context::set(kk::Object * object) {
            auto i = _objects.find(object);
            if(i != _objects.end()) {
                _objects[object] = object;
            }
        }
        
        void Context::remove(kk::Object * object) {
            auto i = _objects.find(object);
            if(i != _objects.end()) {
                _objects.erase(i);
            }
        }
        
        kk::Object * Context::get(kk::Object * object) {
            auto i = _objects.find(object);
            if(i != _objects.end()) {
                return i->second;
            }
            return nullptr;
        }
        
        kk::CString Context::basePath() {
            return _basePath.c_str();
        }
        
        kk::DispatchQueue * Context::queue() {
            return _queue;
        }
        
        duk_context * Context::jsContext() {
            return _jsContext;
        }
        
        kk::String Context::getTextContent(kk::CString path) {

            kk::String v;
            kk::String p = absolutePath(path);
            
            FILE * fd = fopen(p.c_str(), "r");
            
            if(fd != nullptr) {
                char data[20480];
                size_t n;
                
                while((n = fread(data, 1, sizeof(data), fd)) > 0) {
                    v.append(data,0,n);
                }
                
                fclose(fd);
            }
            
            return v;
        }
        
        kk::String Context::absolutePath(kk::CString path) {
            
            kk::String v = _basePath;
            
            if(!kk::CStringHasSuffix(v.c_str(),"/")) {
                v.append("/");
            }
            
            v.append(path);
            
            return v;
            
        }
        
        void Context::exec(kk::CString path,TObject<String, Any> * librarys) {
            
            kk::String code("(function(");
            
            if(librarys != nullptr) {
                auto i = librarys->begin();
                while(i != librarys->end()) {
                    if(i != librarys->begin()) {
                        code.append(",");
                    }
                    code.append(i->first);
                    i ++;
                }
            }
            
            code.append("){");
            
            code.append(getTextContent(path));
            
            code.append("})");
            
            duk_context * ctx = jsContext();
            
            duk_push_string(ctx, path);
            duk_compile_string_filename(ctx, 0, code.c_str());
            
            if(duk_is_function(ctx, -1)) {
                
                if(duk_pcall(ctx, 0) == DUK_EXEC_SUCCESS) {
                    
                    duk_idx_t n =0;

                    if(librarys != nullptr) {

                        auto i = librarys->begin();

                        while(i != librarys->end()) {

                            PushAny(ctx, i->second);

                            i ++;
                            n ++;
                        }

                    }

                    if(duk_pcall(ctx, n) != DUK_EXEC_SUCCESS) {
                        Error(ctx, -1, "[Context::exec] ");
                    }
                    
                } else {
                    Error(ctx, -1, "[Context::exec] ");
                }
    
            }
            
            duk_pop(ctx);
            
        }
        
        void Context::exec(kk::CString path,JSObject * librarys) {
            
            kk::String code("(function(");
            
            std::vector<std::shared_ptr<Any>> vs;
            
            if(librarys != nullptr) {
                
                duk_context * ctx = librarys->jsContext();
                
                duk_push_heapptr(ctx, librarys->heapptr());
                
                if(duk_is_object(ctx, -1)) {
                    
                    duk_enum(ctx, -1, DUK_ENUM_INCLUDE_SYMBOLS);
                    
                    while(duk_next(ctx, -1, 1)) {
                        Any name;
                        GetAny(ctx, -2, name);
                        kk::CString cname = name;
                        if(cname) {
                            if(vs.size() != 0) {
                                code.append(",");
                            }
                            code.append(cname);
                            vs.push_back(GetAnyPtr(ctx, -1));
                        }
                        
                        duk_pop_2(ctx);
                    }
                }
                
                duk_pop(ctx);
                
            }
            
            code.append("){");
            
            code.append(getTextContent(path));
            
            code.append("})");
            
            duk_context * ctx = jsContext();
            
            duk_push_string(ctx, path);
            duk_compile_string_filename(ctx, 0, code.c_str());
            
            if(duk_is_function(ctx, -1)) {
                
                if(duk_pcall(ctx, 0) == DUK_EXEC_SUCCESS) {
                    
                    auto i = vs.begin();
                    
                    while(i != vs.end()) {
                        std::shared_ptr<Any> & v = (*i);
                        PushAny(ctx, * v);
                        i ++;
                    }
                    
                    if(duk_pcall(ctx, (duk_idx_t) vs.size()) != DUK_EXEC_SUCCESS) {
                        Error(ctx, -1, "[Context::exec] ");
                    }
                    
                } else {
                    Error(ctx, -1, "[Context::exec] ");
                }
                
            }
            
            duk_pop(ctx);
            
        }
        
        kk::Strong<Sqlite> Context::createSqlite(kk::CString path) {
            kk::String p = absolutePath(path);
            kk::Strong<Sqlite> v = new Sqlite();
            v->open(p.c_str());
            return v;
        }
        
        Worker::Worker(Context * main,kk::CString path):_main(main),_context(nullptr),_queue(nullptr) {
            
            _queue = createDispatchQueue("kk::ui::Worker", DispatchQueueTypeSerial);
            _queue->retain();
            
            kk::String p = path;
            
            kk::Weak<Worker> weak = this;
            
            _queue->sync([this,p,main,weak]()->void{
                
                Context * v = this->_context = new Context(main->basePath(),this->_queue);
                
                v->retain();
                
                duk_context * ctx = v->jsContext();
                PushObject(ctx, v);
                duk_put_global_string(ctx, "app");
                
                kk::PushFunction<void,Any>(ctx, new TFunction<void,Any>([weak](Any object)->void{
                    
                    kk::Strong<Worker> v = weak.operator->();
                    
                    if(v != nullptr) {
                        v->onMessage(object);
                    }
                    
                }));
                
                duk_put_global_string(ctx, "postMessage");
                
                if(!p.empty()) {
                    v->exec(p.c_str(), (TObject<String, Any> *) nullptr);
                }
                
            });
            
            main->set(this);
            
        }
        
        Context * Worker::context() {
            return _context;
        }
        
        void Worker::onMessage(Any & data) {
            
            JITContext::current()->forEach(this, [data](duk_context * ctx,void * heapptr)->void{
                
                duk_push_heapptr(ctx, heapptr);
                
                duk_get_prop_string(ctx, -1, "onmessage");
                
                if(duk_is_function(ctx, -1)) {
                    
                    PushAny(ctx, (Any &) data);
                    
                    if(duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
                        kk::Error(ctx, -1, "[kk::ui::Worker::onMessage] ");
                    }
                }
                
                duk_pop_2(ctx);
                
            });
        }
        
        void Worker::onBackgroundMessage(Any & data) {
            Any v = data.copy();
            kk::Weak<Worker> weak = this;
            _main->queue()->async([v,weak]()->void{
                kk::Strong<Worker> w = weak.operator->();
                if(w != nullptr) {
                    w->onMessage((Any &) v);
                }
            });
        }
        
        Worker::~Worker() {
            if(_context != nullptr) {
                _context->release();
                _context = nullptr;
            }
            if(_queue != nullptr) {
                _queue->release();
                _queue = nullptr;
            }
        }
        
        void Worker::postMessage(Any data) {
            
            if(_context != nullptr) {
                
                Any v = data.copy();
                Context * ctx = _context;
                
                _context->queue()->async([v,ctx]()->void{
                    
                    kk::Strong<Context> c = ctx;
                    
                    duk_context * jsContext = ctx->jsContext();
                    
                    duk_get_global_string(jsContext, "onmessage");
                    
                    if(duk_is_function(jsContext, -1)) {
                        
                        PushAny(jsContext, (Any &) v);
                        
                        if(duk_pcall(jsContext, 1) != DUK_EXEC_SUCCESS) {
                            kk::Error(jsContext, -1, "[kk::ui::Worker::postMessage] ");
                        }
                    }
                    
                    duk_pop(jsContext);

                });
            }
            
        }
        
        void Worker::terminate() {
            if(_context != nullptr) {
                _context->release();
                _context = nullptr;
            }
            if(_queue != nullptr) {
                _queue->release();
                _queue = nullptr;
            }
            if(_main != nullptr) {
                _main->remove(this);
                _main = nullptr;
            }
        }
        
        kk::Strong<Worker> Context::createWorker(kk::CString cpath) {
            return new Worker(this,cpath);
        }
        
        kk::Strong<Canvas> Context::createCanvas() {
            return kk::ui::createCanvas((DispatchQueue *) _queue);
        }
        
        kk::Strong<Image> Context::createImage(kk::CString src) {
            return ImageCreate(this,src);
        }
        
        void Context::Openlib(){
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<Context>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutMethod<Context,void,kk::CString,JSObject *>(ctx, -1, "exec", &Context::exec);
                    
                    kk::PutMethod<Context,void,Object *>(ctx, -1, "set", &Context::set);
                    
                    kk::PutMethod<Context,void,Object *>(ctx, -1, "remove", &Context::remove);
                    
                    kk::PutMethod<Context,kk::String,kk::CString>(ctx, -1, "getTextContent", &Context::getTextContent);
                    
                    kk::PutStrongMethod<Context,Worker,kk::CString>(ctx, -1, "createWorker", &Context::createWorker);
                    
                    kk::PutStrongMethod<Context,Canvas>(ctx, -1, "createCanvas", &Context::createCanvas);
                    
                    kk::PutStrongMethod<Context,Image,kk::CString>(ctx, -1, "createImage", &Context::createImage);
                    
                    kk::PutStrongMethod<Context,Sqlite,kk::CString>(ctx, -1, "createSqlite", &Context::createSqlite);
                    
                });
                
                kk::PushInterface<Image>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutProperty<Image,kk::CString>(ctx, -1, "src", &Image::src);
                    
                    kk::PutProperty<Image,kk::Uint>(ctx, -1, "width", &Image::width);
                    
                    kk::PutProperty<Image,kk::Uint>(ctx, -1, "height", &Image::height);
                    
                    
                });
                
            });
            
        }
        
    }
    
}

