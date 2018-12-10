//
//  app.h
//  KK
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef ui_app_h
#define ui_app_h

#include <ui/ui.h>
#include <ui/view.h>

namespace kk {
    
    namespace ui {
        
        class Package;
        
        class App : public Context {
        public:
            
            App(kk::CString basePath,kk::CString platform,kk::CString userAgent,kk::CString appkey);
            
            virtual ~App();
            
            virtual void open(kk::CString uri,kk::Boolean animated);
            
            virtual void back(kk::Uint delta,kk::Boolean animated);
            
            virtual kk::Strong<View> createView(kk::CString name,ViewConfiguration * configuration);
            
            virtual Size getAttributedTextContentSize(AttributedText * text,Float maxWidth);
            
            virtual kk::Strong<Package> createPackage(kk::CString URI);
            
            virtual kk::CString appkey();
            
            static void Openlib();
            
            Ker_CLASS(App,Context,"UIApp")
    
        protected:
            kk::String _appkey;
        };
        
        
        void addAppOpenlib(std::function<void(duk_context *,App *)> && func);
        
    }
}


#endif /* app_h */
