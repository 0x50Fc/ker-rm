//
//  app.cc
//  KK
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <ui/app.h>
#include <ui/page.h>
#include <ui/view.h>
#include <ui/screen.h>
#include <ui/CGContext.h>
#include <ui/package.h>
#include <ui/ViewElement.h>
#include <ui/TextElement.h>
#include <ui/PageElement.h>
#include <ui/ImageElement.h>
#include <core/crypto.h>
#include <core/uri.h>
#include <core/net.h>
#include <core/http.h>
#include <core/ssl.h>
#include <core/WebSocket.h>
#include <core/stream.h>
#include <core/file.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <document/element.h>
#include <document/StyleElement.h>
#include <document/LayoutElement.h>
#include <document/BlockElement.h>
#include <audio/audio.h>
#include <audio/speex.h>

#ifdef KER_DEBUG
#include <core/debugger.h>
#endif

namespace kk {
    
    namespace ui {
    
        App::App(kk::Uint64 appid,kk::CString basePath,kk::CString platform,kk::CString userAgent,kk::CString appkey):Context(basePath,UI::main()->queue()),_appkey(appkey),_appid(appid) {

            _queue->sync([this,userAgent,platform]()->void{
                
                PushWeakObject(_jsContext, this);
                duk_put_global_string(_jsContext, "app");
                
                duk_push_string(_jsContext, platform);
                duk_put_global_string(_jsContext, "platform");
                
                duk_push_string(_jsContext, userAgent);
                duk_put_global_string(_jsContext, "userAgent");
                
                duk_push_undefined(_jsContext);
                duk_put_global_string(_jsContext, "page");
                
            });
            
        }
        
        void App::openlib() {
            kk::Openlib<App *>::openlib(_jsContext, this);
            PushObject(_jsContext, _screen);
            duk_put_global_string(_jsContext, "screen");
        }
        
        Screen * App::screen() {
            return _screen;
        }
        
        void App::setScreen(Screen * v) {
            _screen = v;
        }
        
        App::~App() {
  
            UI::main()->removeApp(_appid);
            
            kk::Log("[App] [dealloc]");
        }
        
        kk::CString App::appkey() {
            return _appkey.c_str();
        }
        
        kk::Uint64 App::appid() {
            return _appid;
        }
        
        void App::open(kk::CString uri,kk::Boolean animated) {
            
            if(uri == nullptr) {
                return;
            }
            
            kk::URI u(uri);
            kk::Strong<Page> page = createPage(u.scheme());
            kk::TObject<kk::String,kk::String> & queryObject = u.queryObject();
        
            page->opening();
            page->run(u.path(), &queryObject);
            page->open(animated);
            
        }
        
        void App::back(kk::Uint delta,kk::Boolean animated) {
            kk::Strong<AppBackCommand> cmd = new AppBackCommand();
            cmd->appid = _appid;
            cmd->delta = delta;
            cmd->animated = animated;
            execCommand(cmd);
        }
        
        kk::Strong<View> App::createView(kk::CString name,ViewConfiguration * configuration) {
            kk::Strong<View> v = new View(name, configuration,this,UI::main()->newId());
            _views[v->viewId()] = (View *) v;
            return v;
        }
        
        kk::Strong<View> App::createView(kk::Native * native,Rect & frame) {
            kk::Strong<View> v = new View(native,frame,this,UI::main()->newId());
            _views[v->viewId()] = (View *) v;
            return v;
        }
        
        kk::Strong<View> App::getView(kk::Uint64 viewId) {
            kk::Strong<View> v;
            auto i = _views.find(viewId);
            if(i != _views.end()) {
                v = (View *) i->second;
            }
            return v;
        }
        
        void App::removeView(kk::Uint64 viewId) {
            auto i = _views.find(viewId);
            if(i != _views.end()) {
                _views.erase(i);
            }
        }
        
        kk::Strong<Canvas> App::createCanvas(View * view,DispatchQueue * queue) {
            kk::Strong<Canvas> v = new Canvas(view,queue == nullptr ? this->queue() : queue,this,UI::main()->newId());
            _canvas[v->canvasId()] = (Canvas *) v;
            return v;
        }
        
        void App::removeCanvas(kk::Uint64 canvasId) {
            auto i = _canvas.find(canvasId);
            if(i != _canvas.end()) {
                _canvas.erase(i);
            }
        }
        
        kk::Strong<Page> App::createPage(kk::CString type) {
            kk::Strong<Page> v = new Page(this,UI::main()->newId(),type);
            _pages[v->pageId()] = (Page *) v;
            return v;
        }
        
        kk::Strong<Page> App::getPage(kk::Uint64 pageId) {
            auto i = _pages.find(pageId);
            if(i != _pages.end()) {
                return (Page *) i->second;
            }
            return nullptr;
        }
        
        void App::removePage(kk::Uint64 pageId) {
            auto i = _pages.find(pageId);
            if(i != _pages.end()) {
                _pages.erase(i);
            }
        }
        
        Size App::getAttributedTextContentSize(AttributedText * text,Float maxWidth) {
            return ::kk::ui::getAttributedTextContentSize(this, text, maxWidth);
        }
        
        void App::execCommand(Command * command) {
            UI::main()->execCommand(this, command);
        }
        
        void App::dispatchCommand(Command * command) {
            {
                ViewCommand * v = dynamic_cast<ViewCommand *>(command);
                if(v != nullptr){
                    auto i = _views.find(v->viewId);
                    if(i != _views.end()) {
                        i->second->dispatchCommand(v);
                    }
                    return;
                }
            }
        }
        
        void App::run(kk::Object * query) {
            kk::Strong<kk::TObject<kk::String, kk::Any>> librarys = new kk::TObject<kk::String, kk::Any>({{"query",query}});
            exec("main.js", (kk::TObject<kk::String, kk::Any> *) librarys);
        }
        
        kk::Strong<Database> App::openDataBase(kk::CString path) {
            if(path == nullptr) {
                return nullptr;
            }
            kk::String u;
            u.append("ker-data:///");
            u.append(_appkey);
            u.append("/");
            u.append(path);
            kk::String p = kk::ResolvePath(u.c_str());
            kk::String dir = CStringPathDirname(p.c_str());
            File::mkdir(dir.c_str());
            kk::Strong<Sqlite> v = new Sqlite();
            v->open(p.c_str());
            return new SqliteDatabase(v);
        }
        
        kk::Strong<File> App::openFile(kk::CString directory,kk::CString path,kk::CString type) {
            if(kk::CStringEqual(directory, kTemporaryDirectory)) {
                kk::String u;
                u.append("ker-tmp:///");
                u.append(path);
                return File::openURI(u.c_str(), type);
            } else if(kk::CStringEqual(directory, kDataDirectory)) {
                kk::String u;
                u.append("ker-data:///");
                u.append(_appkey);
                u.append("/");
                u.append(path);
                return File::openURI(u.c_str(), type);
            } else {
                kk::String p = absolutePath(path);
                kk::String u = ResolveURI(p.c_str());
                return File::openURI(u.c_str(), type);
            }
        }
        
        kk::Strong<File> App::openTempFile(kk::CString prefix,kk::CString suffix,kk::CString type) {
            kk::String u;
            u.append("ker-tmp:///");
            if(prefix) {
                u.append(prefix);
            }
            char fmt[] = "XXXXXXXXXXXXXXXX";
            u.append(mktemp(fmt));
            if(suffix) {
                u.append(suffix);
            }
            return File::openURI(u.c_str(), type);
        }
        
        Storage * App::storage(){
            if(_storage == nullptr) {
                kk::String p = _appkey;
                p.append("_");
                _storage = new SqliteStorage(UI::main()->database(),p.c_str());
            }
            return _storage;
        }
        
        void App::Openlib() {
            
#ifdef KER_DEBUG
            kk::Debugger::start(9091);
#endif
            
            UI::main();
            
            kk::OpenBaselib();
            kk::Event::Openlib();
            kk::EventEmitter::Openlib();
            kk::Timer::Openlib();
            kk::Sqlite::Openlib();
            kk::Database::Openlib();
            kk::SqliteDatabase::Openlib();
            kk::Crypto::Openlib();
            kk::Storage::Openlib();
            kk::TCPConnection::Openlib();
            kk::SSLConnection::Openlib();
            kk::HTTPRequest::Openlib();
            kk::WebSocket::Openlib();
            kk::File::Openlib();
            kk::URI::Openlib();
            kk::ui::Context::Openlib();
            kk::ui::View::Openlib();
            kk::ui::Canvas::Openlib();
            kk::ui::AttributedText::Openlib();
            kk::ui::WebViewConfiguration::Openlib();
            kk::ui::Context::Openlib();
            kk::ui::Page::Openlib();
            kk::ui::CG::Context::Openlib();
            kk::ui::Package::Openlib();
            kk::ui::Screen::Openlib();
            kk::Document::Openlib();
            kk::ElementEvent::Openlib();
            kk::Element::Openlib();
            kk::BlockElement::Openlib();
            kk::StyleElement::Openlib();
            kk::LayoutElement::Openlib();
            kk::ui::ViewElement::Openlib();
            kk::ui::TextElement::Openlib();
            kk::ui::PageElement::Openlib();
            kk::ui::ImageElement::Openlib();
            kk::Stream::Openlib();
            kk::audio::Audio::Openlib();
            kk::audio::SpeexCodec::Openlib();
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<App>(ctx, [](duk_context * ctx)->void{
                    
                    duk_push_string(ctx, kDataDirectory);
                    duk_put_prop_string(ctx, -3, "kDataDirectory");
                    
                    duk_push_string(ctx, kTemporaryDirectory);
                    duk_put_prop_string(ctx, -3, "kTemporaryDirectory");
                    
                    kk::PutMethod<App,void,kk::CString,kk::Boolean>(ctx, -1, "open", &App::open);
                    kk::PutMethod<App,void,kk::Uint,kk::Boolean>(ctx, -1, "back", &App::back);
                    kk::PutMethod<App,Size,AttributedText *,Float>(ctx, -1, "getAttributedTextContentSize", &App::getAttributedTextContentSize);
                    kk::PutStrongMethod<App,kk::Database,kk::CString>(ctx, -1, "openDataBase", &App::openDataBase);
                    kk::PutStrongMethod<App,File,kk::CString,kk::CString,kk::CString>(ctx, -1, "openFile", &App::openFile);
                    kk::PutStrongMethod<App,File,kk::CString,kk::CString,kk::CString>(ctx, -1, "openTempFile", &App::openTempFile);
                    
                    kk::PutStrongMethod<App,View,kk::CString,ViewConfiguration *>(ctx,-1,"createView",&App::createView);
                    kk::PutProperty<App,kk::CString>(ctx, -1, "appkey", &App::appkey);
                    kk::PutProperty<App,kk::Storage *>(ctx, -1, "storage", &App::storage);
                    
                });
                
                
            });
            
        }
        
        void addAppOpenlib(std::function<void(duk_context *,App *)> && func) {
            kk::Openlib<App *>::add(std::move(func));
        }
        
    }
    
}
