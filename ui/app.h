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
        class Page;
        
        class AppOpenCommand : public Command {
        public:
            kk::String uri;
            kk::Boolean animated;
        };
        
        class AppBackCommand : public Command {
        public:
            kk::Uint delta;
            kk::Boolean animated;
        };
        
        class App : public Context {
        public:
            
            App(kk::CString basePath,kk::CString platform,kk::CString userAgent,kk::CString appkey);
            
            virtual ~App();
            
            virtual void open(kk::CString uri,kk::Boolean animated);
            
            virtual void back(kk::Uint delta,kk::Boolean animated);
            
            virtual kk::Strong<View> createView(kk::CString name,ViewConfiguration * configuration);
            
            virtual kk::Strong<View> getView(kk::Uint64 viewId);
            
            virtual kk::Strong<View> createView(kk::Native * native,Rect & frame);
            
            virtual void removeView(kk::Uint64 viewId);
            
            virtual kk::Strong<Canvas> createCanvas(View * view,DispatchQueue * queue);
            
            virtual void removeCanvas(kk::Uint64 canvasId);
            
            virtual kk::Strong<Page> createPage(View * view);
            
            virtual kk::Strong<Page> getPage(kk::Uint64 pageId);
            
            virtual void removePage(kk::Uint64 pageId);
            
            virtual Size getAttributedTextContentSize(AttributedText * text,Float maxWidth);
            
            virtual kk::Strong<Package> createPackage(kk::CString URI);
            
            virtual kk::CString appkey();
            
            virtual void execCommand(Command * command);
            
            virtual void dispatchCommand(Command * command);
            
            virtual void setOnCommand(std::function<void(App * ,Command *)> && func);
            
            static void Openlib();
            
            Ker_CLASS(App,Context,"UIApp")
    
        protected:
            kk::String _appkey;
            std::function<void(App * ,Command *)> _onCommand;
            kk::Uint64 _autoId;
            std::map<kk::Uint64,kk::Weak<View>> _views;
            std::map<kk::Uint64,kk::Weak<Canvas>> _canvas;
            std::map<kk::Uint64,kk::Weak<Page>> _pages;
        };
        
        
        void addAppOpenlib(std::function<void(duk_context *,App *)> && func);
        
    }
}


#endif /* app_h */
