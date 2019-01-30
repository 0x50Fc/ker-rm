//
//  app.h
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

        class PageCommand : public Command {
        public:
            kk::Uint64 appid;
            kk::Uint64 pageId;
        };
        
        class PageSetOptionsCommand : public PageCommand {
        public:
            kk::Strong<kk::Object> data;
        };
        
        class PageCloseCommand : public PageCommand {
        public:
            kk::Boolean animated;
        };
        
        class PageSetLeftViewCommand : public PageCommand {
        public:
            kk::Uint64 viewId;
        };
        
        class PageSetRightViewCommand : public PageCommand {
        public:
            kk::Uint64 viewId;
        };
        
        class PageSetTitleViewCommand : public PageCommand {
        public:
            kk::Uint64 viewId;
        };
        
        
        class Page : public EventEmitter , public kk::Container, public JSWeakObject {
        public:
            Page(App * app,kk::Uint64 pageId,kk::CString type);
            virtual ~Page();
            virtual App * app();
            virtual kk::DispatchQueue * queue();
            virtual View * view();
            virtual void setView(View * v);
            virtual View * leftView();
            virtual void setLeftView(View * v);
            virtual View * rightView();
            virtual void setRightView(View * v);
            virtual View * titleView();
            virtual void setTitleView(View * v);
            
            kk::Object * get(kk::Object * object);
            virtual void set(kk::Object * object);
            virtual void remove(kk::Object * object);
            virtual void run(kk::CString path,Object * query);
            virtual void setSize(Size & size);
            virtual void setOptions(kk::Object * data);
            virtual void close(kk::Boolean animated);
            virtual void unload();
            virtual Size size();
            virtual Float width();
            virtual Float height();
            virtual void addLibrary(kk::CString name,kk::Any & value);
            virtual kk::Uint64 pageId();
            virtual void ready();
            virtual void opening();
            virtual void open(kk::Boolean animated);
            virtual kk::CString type();
            static void Openlib();
            
            Ker_CLASS(Page,EventEmitter,"UIPage")
            
        protected:
            kk::Weak<App> _app;
            kk::Strong<kk::TFunction<void,kk::CString,kk::Event *>> _func;
            kk::Strong<View> _view;
            kk::Strong<View> _leftView;
            kk::Strong<View> _rightView;
            kk::Strong<View> _titleView;
            kk::Strong<kk::TObject<kk::String, kk::Any>> _librarys;
            std::map<void *,kk::Strong<kk::Object>> _objects;
            Size _size;
            kk::Uint64 _pageId;
            kk::String _type;
        };
        
        void addPageOpenlib(std::function<void(duk_context *,Page *)> && func);
        
    }
}



#endif /* page_h */
