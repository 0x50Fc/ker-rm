//
//  package.cc
//  Ker
//
//  Created by zhanghailong on 2018/12/10.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#include <ui/package.h>
#include <ui/app.h>
#include <core/uri.h>
#include <core/crypto.h>
#include <sys/stat.h>

namespace kk {
    
    namespace ui {
        
        Package::Package(kk::CString URI,kk::CString appkey):_URI(URI),_state(PackageStateNone) {
           
            kk::URI u(URI);
            kk::Crypto C;
            kk::Weak<Package> pkg(this);
            
            if(appkey) {
                _appkey = appkey;
            } else {
                _appkey = C.MD5(URI);
            }
            
            if(kk::CStringEqual(u.scheme(), "http") || kk::CStringEqual(u.scheme(), "https")) {
                _path = GetDirectory(kAppDirectory);
                _path.append("/");
                _path.append(C.MD5(URI));
                _path.append("/");
                struct stat st;
                if(-1 != stat(_path.c_str(), &st)) {
                    _state = PackageStateLoaded;
                    UI::main()->queue()->async([pkg]()->void{
                        kk::Strong<Package> package = pkg.operator->();
                        if(package != nullptr) {
                            kk::Strong<kk::Event> e = new kk::Event();
                            package->emit("load", e);
                        }
                    });
                } else {
                    _state = PackageStateLoading;
                    kk::HTTPRequest * http = new HTTPRequest(UI::main()->queue());
                    _http = http;
                    _http->open("GET", URI, HTTPResponseTypeUnzip);
                    _http->on("progress", new kk::TFunction<void,kk::CString,Event *>([pkg](kk::CString name,kk::Event * event)->void{
                        kk::Strong<Package> package = pkg.operator->();
                        if(package != nullptr) {
                            kk::Strong<kk::Event> e = new kk::Event();
                            package->emit(name, e);
                        }
                    }));
                    _http->on("error", new kk::TFunction<void,kk::CString,Event *>([pkg](kk::CString name,kk::Event * event)->void{
                        kk::Strong<Package> package = pkg.operator->();
                        if(package != nullptr) {
                            kk::Strong<kk::Event> e = new kk::Event();
                            package->setState(PackageStateError);
                            package->emit(name, e);
                        }
                    }));
                    _http->on("done", new kk::TFunction<void,kk::CString,Event *>([pkg,http](kk::CString name,kk::Event * event)->void{
                        kk::Strong<Package> package = pkg.operator->();
                        if(package != nullptr) {
                            rename(http->responseFile().c_str(), package->_path.c_str());
                            kk::Strong<kk::Event> e = new kk::Event();
                            package->setState(PackageStateLoaded);
                            package->emit("load", e);
                        }
                    }));
                    _http->send();
                }
                
            } else {
                kk::String p = ResolveURI(u.path());
                _path = u.path();
                struct stat st;
                if(-1 != stat(_path.c_str(), &st)) {
                    _state = PackageStateLoaded;
                    UI::main()->queue()->async([pkg]()->void{
                        kk::Strong<Package> package = pkg.operator->();
                        if(package != nullptr) {
                            kk::Strong<kk::Event> e = new kk::Event();
                            package->emit("load", e);
                        }
                    });
                } else {
                    _state = PackageStateError;
                    UI::main()->queue()->async([pkg]()->void{
                        kk::Strong<Package> package = pkg.operator->();
                        if(package != nullptr) {
                            kk::Strong<kk::Event> e = new kk::Event();
                            e->setData(new TObject<kk::String, kk::String>({{"errmsg","未找到应用包"}}));
                            package->emit("error", e);
                        }
                    });
                }
            }
            
        }
        
        Package::~Package() {
            if(_http != nullptr) {
                _http->off();
                _http->cancel();
            }
        }
        
        kk::CString Package::URI() {
            return _URI.c_str();
        }
        
        PackageState Package::state() {
            return _state;
        }
        
        void Package::setState(PackageState state) {
            _state = state;
        }
        
        Uint64 Package::run(kk::Object * query) {
            kk::Strong<kk::ui::App> app = UI::main()->createApp(_path.c_str(),_appkey.c_str());
            app->run(query);
            return app->appid();
        }
        
        void Package::Openlib() {
            
            addAppOpenlib([](duk_context * ctx,App * app)->void{
                
                kk::PushClass<Package,kk::CString,kk::CString>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutMethod<Package,kk::Uint64,kk::Object *>(ctx, -1, "run", &Package::run);
                    kk::PutProperty<Package,kk::CString>(ctx, -1, "URI", &Package::URI);
                    kk::PutProperty<Package,kk::Uint>(ctx, -1, "state", &Package::state);
                    
                });
                
            });
        }
        
    }
    
}
