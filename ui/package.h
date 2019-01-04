//
//  package.h
//  Ker
//
//  Created by hailong11 on 2018/12/10.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#ifndef ui_package_h
#define ui_package_h


#include <ui/ui.h>
#include <core/event.h>
#include <core/http.h>

namespace kk {
    
    namespace ui {
        
        class App;
        
        typedef kk::Uint PackageState;
        
        enum {
            PackageStateNone,
            PackageStateLoading,
            PackageStateLoaded,
            PackageStateError
        };
        
        class Package : public EventEmitter {
        public:
            
            Package(kk::CString URI);
            
            virtual ~Package();
            
            virtual kk::CString URI();
            virtual PackageState state();
            virtual void setState(PackageState state);
            virtual kk::Uint64 run(kk::Object * query);
            
            Ker_CLASS(Package,EventEmitter,"UIPackage")
            
            static void Openlib();
            
        protected:
            PackageState _state;
            kk::String _URI;
            kk::String _path;
            kk::String _appkey;
            kk::Strong<HTTPRequest> _http;
        };
        
        
    }
}


#endif /* package_h */
