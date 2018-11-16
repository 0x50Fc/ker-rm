//
//  page.h
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef ui_page_h
#define ui_page_h

#include <ui/app.h>
#include <ui/view.h>

namespace kk {
    
    namespace ui {
        
        class Page : public EventEmitter , public kk::Container {
        public:
            Page(App * app,View * view);
            virtual ~Page();
            virtual duk_context * jsContext();
            virtual App * app();
            virtual kk::DispatchQueue * queue();
            virtual View * view();
            kk::Object * get(kk::Object * object);
            virtual void set(kk::Object * object);
            virtual void remove(kk::Object * object);
            virtual void run(kk::CString path , kk::TObject<kk::String,kk::String> * query);
            virtual void setSize(Size & size);
            virtual void setOptions(kk::Object * data);
            virtual Size size();
            virtual Float width();
            virtual Float height();
            virtual void addLibrary(kk::CString name,kk::Any & value);
            static void Openlib();
            
            Ker_CLASS(Page,EventEmitter,"UIPage")
            
        protected:
            kk::Strong<App> _app;
            kk::Strong<View> _view;
            kk::Strong<kk::TObject<kk::String, kk::Any>> _librarys;
            duk_context * _jsContext;
            std::map<void *,kk::Strong<kk::Object>> _objects;
            Size _size;
        };
        
        void addPageOpenlib(std::function<void(duk_context *,Page *)> && func);
        
    }
}



#endif /* page_h */
