//
//  BarElement.c
//  Ker
//
//  Created by zhanghailong on 2019/1/28.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "BarElement.h"

namespace kk {
    
    namespace ui {
        
        BarElement::BarElement(Document * document,CString name, ElementKey elementId):ViewElement(document,name,elementId)  {
            
        }
        
        void BarElement::obtainView(ViewContext * context) {
            
            if(_view == nullptr) {
                Strong<View> vv = createView(context);
                _view = (View *) vv;
                
                Page * page = context->page();
                
                if(page != nullptr) {
                    
                    kk::CString target = get("target");
                    
                    if(kk::CStringEqual(target, "left")) {
                        page->setLeftView(_view);
                    } else if(kk::CStringEqual(target, "title")) {
                        page->setTitleView(_view);
                    } else {
                        page->setRightView(_view);
                    }
                }
                
                onObtainView(context,_view);
                onViewLayout();
            }
            
            if(_view != nullptr) {
                obtainViewKey(context);
                obtainChildrenView(context);
            }
            
        }
        
        void BarElement::onViewLayout() {
            frame.origin.x = 0;
            frame.origin.y = 0;
            ViewElement::onViewLayout();
        }

        void BarElement::Openlib() {
            
            library("bar");
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<BarElement>(ctx, [](duk_context * ctx)->void{
                    
                });
                
            });
            
        }
        
        void BarElement::library(kk::CString name) {
            Document::library(name, [](Document * document,kk::CString name,ElementKey elementId)->Element *{
                return new BarElement(document,name,elementId);
            });
        }
        
    }
    
}
