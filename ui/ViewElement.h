//
//  ViewElement.h
//  Ker
//
//  Created by zhanghailong on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef ui_ViewElement_h
#define ui_ViewElement_h

#include <document/LayoutElement.h>
#include <ui/view.h>
#include <ui/app.h>
#include <ui/page.h>

namespace kk {
    
    namespace ui {
        
        class ViewElement;
        
        class ViewContext : public kk::LayoutContext {
        public:
            ViewContext(App * app);
            virtual View * view();
            virtual void setView(View * view);
            virtual App * app();
            virtual Page * page();
            virtual void setPage(Page * page);
            
            virtual void obtainView(ViewElement * element);
            
            Ker_CLASS(ViewContext,kk::LayoutContext,"UIViewContext")
            
        protected:
            kk::Strong<View> _view;
            kk::Weak<App> _app;
            kk::Weak<Page> _page;
        };
        
        typedef void (*ViewElementSetFunction)(ViewContext * context,View * view,CString key, CString value);
        
        void ViewElementSetStringValue(ViewContext * context,View * view,CString key, CString value);
        void ViewElementSetPixelValue(ViewContext * context,View * view,CString key, CString value);
     
        class ViewElement : public kk::LayoutElement {
        public:
            ViewElement(Document * document,CString name, ElementKey elementId);
            virtual void onLayout(LayoutContext * context);
            virtual void setContentOffset(ViewContext * context,Float x,Float y);
            virtual void obtainView(ViewContext * context);
            virtual void recycleView();
            virtual View * view();
            virtual void changedKey(CString key);
            virtual void recycle();
            virtual kk::String reuse();
            
            virtual void setNeedLayout();
            
            Ker_CLASS(ViewElement,LayoutElement,"UIViewElement")
            
            static void Openlib();
            static void library(kk::CString name);
        protected:
            virtual void obtainViewKey(ViewContext * context);
            virtual ViewElement * parentViewElement();
            virtual void setViewKey(ViewContext * context,View * view,CString key, CString value);
            virtual kk::Boolean canRecycleView();
            virtual void onEvent(ViewContext * context,CString name,Event * event);
            virtual void onObtainView(ViewContext * context,View * view);
            virtual void onRecycleView(View * view);
            virtual void obtainChildrenView(ViewContext * context);
            virtual void recycleChildrenView();
            virtual void onViewLayout();
            virtual Strong<View> createView(ViewContext * context);
            virtual kk::Boolean isVisibleChildren(ViewElement * element);
            virtual kk::Boolean isVisible();
            kk::Boolean _obtaining;
            Point _contentOffset;
            kk::Strong<View> _view;
            std::set<kk::String> _changedKeys;
            
            kk::Int _autoLevelId;
            kk::Int _levelId;
            
            virtual void onDidAddChildren(Element * element);
            virtual void onWillRemoveChildren(Element * element);
            
        };
        
    }
    
}

#endif /* ViewElement_h */