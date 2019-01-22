//
//  ImageElement.h
//  Ker
//
//  Created by zhanghailong on 2019/1/22.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef kk_ImageElement_h
#define kk_ImageElement_h

#include <ui/app.h>
#include <ui/ViewElement.h>

namespace kk {
    
    namespace ui {
        
        class ImageElement : public ViewElement, public kk::Layout {
        public:
            ImageElement(Document * document,CString name, ElementKey elementId);
            virtual ~ImageElement();
            virtual kk::Boolean layout(LayoutContext * context);
            virtual void changedKey(CString key);
            
            Ker_CLASS(ImageElement,ViewElement,"ImageElement")
            
            static void Openlib();
            static void library(kk::CString name);
        protected:
            
            virtual void onObtainView(ViewContext * context,View * view);
            virtual void onRecycleView(View * view);
            virtual void reloadImage();
            virtual void loadImage(App * app,View * view);
            virtual void unloadImage();
            virtual void updateImage();
            
            kk::Weak<App> _app;
            kk::Strong<Image> _image;
            kk::Strong<EventFunction> _onEvent;
        };
        
    }
    
}

#endif /* ImageElement_h */
