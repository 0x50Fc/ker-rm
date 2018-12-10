//
//  package.cc
//  Ker
//
//  Created by hailong11 on 2018/12/10.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#include <ui/package.h>
#include <ui/app.h>

namespace kk {
    
    namespace ui {
        
        Package::Package(kk::CString URI):_URI(URI),_state() {
            
        }
        
        kk::CString Package::URI() {
            return _URI.c_str();
        }
        
        PackageState Package::state() {
            return _state;
        }
        
        void Package::run(kk::Object * query) {
            
        }
        
        void Package::setState(PackageState state) {
            _state = state;
        }
        
        void Package::Openlib() {
            
            addAppOpenlib([](duk_context * ctx,App * app)->void{
                
                kk::PushInterface<Package>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutMethod<Package,void,kk::Object *>(ctx, -1, "run", &Package::run);
                    kk::PutProperty<Package,kk::CString>(ctx, -1, "URI", &Package::URI);
                    kk::PutProperty<Package,kk::Uint>(ctx, -1, "state", &Package::state);
                    
                });
                
            });
        }
        
    }
    
}
