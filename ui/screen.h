//
//  screen.h
//  Ker
//
//  Created by zhanghailong on 2019/1/25.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef ui_screen_h
#define ui_screen_h

#include <ui/ui.h>

namespace kk {
    
    namespace ui {
        
        class Screen : public EventEmitter {
        public:
            Screen(kk::Uint width,kk::Uint height,kk::Float density,kk::Float scale);
            Screen();
            
            virtual kk::Uint width();
            virtual kk::Uint height();
            virtual kk::Float density();
            virtual kk::Float scale();
            virtual void set(kk::Uint width,kk::Uint height,kk::Float density,kk::Float scale);
            
            Ker_CLASS(Screen, EventEmitter, "UIScreen");
            
            static void Openlib();
            
        protected:
            kk::Uint _width;
            kk::Uint _height;
            kk::Float _density;
            kk::Float _scale;
        };
    }
    
}


#endif /* screen_h */
