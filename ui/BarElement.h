//
//  BarElement.h
//  Ker
//
//  Created by zhanghailong on 2019/1/28.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef ui_BarElement_h
#define ui_BarElement_h

#include <ui/app.h>
#include <ui/ViewElement.h>

namespace kk {
    
    namespace ui {
        
        class BarElement : public ViewElement {
        public:
            BarElement(Document * document,CString name, ElementKey elementId);
            
            virtual void obtainView(ViewContext * context);
            
            Ker_CLASS(BarElement,ViewElement,"BarElement")
            
            static void Openlib();
            static void library(kk::CString name);
            
        protected:
    
            virtual void onViewLayout();
        };
        
    }
    
}

#endif /* BarElement_h */
