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

namespace kk {
    
    namespace ui {
        
        typedef kk::Uint PackageState;
        
        enum {
            PackageStateNone,
            PackageStateLoading,
            PackageStateLoaded,
            PackageStateError
        };
        
        class Package : public EventEmitter {
        public:
            
            Package(Context * context,kk::CString URI);
            
            virtual kk::CString URI();
            virtual PackageState state();
            virtual void setState(PackageState state);
            virtual void run(kk::Object * query);
            
            Ker_CLASS(Package,EventEmitter,"UIPackage")
            
            static void Openlib();
            
        protected:
            PackageState _state;
            kk::String _URI;
            kk::Strong<Context> _context;
        };
        
        
        kk::Strong<Package> createPackage(Context * context,kk::CString URI);
        
    }
}


#endif /* package_h */
