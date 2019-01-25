//
//  screen.c
//  Ker
//
//  Created by zhanghailong on 2019/1/25.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "screen.h"

namespace kk {
    
    namespace ui {
     
        
        Screen::Screen(kk::Uint width,kk::Uint height,kk::Float density,kk::Float scale):_width(width),_height(height),_density(density),_scale(scale) {
            
        }
        
        Screen::Screen():_width(0),_height(0),_density(1.0f),_scale(1.0f) {
            
        }
        
        kk::Uint Screen::width() {
            return _width;
        }
        
        kk::Uint Screen::height() {
            return _height;
        }
        
        kk::Float Screen::density() {
            return _density;
        }
        
        kk::Float Screen::scale() {
            return _scale;
        }
        
        void Screen::set(kk::Uint width,kk::Uint height,kk::Float density,kk::Float scale) {
            _width = width;
            _height = height;
            _density = density;
            _scale = scale;
            kk::Strong<Event> e = new Event();
            emit("change", e);
        }
        
        void Screen::Openlib() {
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<Screen>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutProperty<Screen,kk::Uint>(ctx, -1, "width", &Screen::width);
                    kk::PutProperty<Screen,kk::Uint>(ctx, -1, "height", &Screen::height);
                    kk::PutProperty<Screen,kk::Float>(ctx, -1, "density", &Screen::density);
                    kk::PutProperty<Screen,kk::Float>(ctx, -1, "scale", &Screen::scale);
                    
                });
                
            });
            
        }
    
    }
    
}
