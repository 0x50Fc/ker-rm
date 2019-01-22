//
//  PageElement.h
//  Ker
//
//  Created by zhanghailong on 2019/1/21.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef ui_PageElement_h
#define ui_PageElement_h

#include <ui/ViewElement.h>
#include <ui/page.h>

namespace kk {
    
    namespace ui {
        
        class PageElement : public ViewElement, public kk::Layout {
        public:
            PageElement(Document * document,CString name, ElementKey elementId);
            virtual ~PageElement();
            virtual kk::Boolean layout(LayoutContext * context);
            virtual void changedKey(CString key);
            virtual Page * page();
            virtual void emit(kk::CString name,Event * event);
            
            Ker_CLASS(PageElement,ViewElement,"PageElement")

            static void Openlib();
            static void library(kk::CString name);
        protected:
            
            virtual void onObtainView(ViewContext * context,View * view);
            virtual void onRecycleView(View * view);
            virtual void reloadPage();
            virtual void loadPage(App * app,View * view);
            virtual void unloadPage();
            
            kk::Strong<Page> _page;
            kk::Strong<EventFunction> _onEvent;
        };
        
    }
    
}

#endif /* PageElement_h */
