//
//  ImageElement.c
//  Ker
//
//  Created by zhanghailong on 2019/1/22.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "ImageElement.h"


namespace kk {
    
    namespace ui {
        
        ImageElement::ImageElement(Document * document,CString name, ElementKey elementId):ViewElement(document,name,elementId) {
            
        }
        
        ImageElement::~ImageElement() {
            if(_image != nullptr) {
                _image->off("load", (EventFunction *) _onEvent);
            }
        }
        
        kk::Boolean ImageElement::layout(LayoutContext * context) {
            return true;
        }
        
        void ImageElement::changedKey(CString key) {
            ViewElement::changedKey(key);
            if(kk::CStringEqual(key, "src")) {
                if(_view != nullptr) {
                    reloadImage();
                }
            }
        }
        
        void ImageElement::reloadImage() {
            
            if(_view != nullptr && _app != nullptr) {
                
                unloadImage();
                loadImage(_app, _view);
                
            }
            
        }
        
        void ImageElement::loadImage(App * app,View * view) {
            
            kk::CString src = get("src");
            
            if(app != nullptr && src != nullptr && !kk::CStringEqual(src, "")) {
                
                kk::Strong<Image> v = app->createImage(src);
                
                _image = (Image *) v;
                
                ImageState state = _image->state();
                
                if(state != ImageStateLoaded && state != ImageStateError) {
                    
                    if(_onEvent == nullptr) {
                        _onEvent = new EventFunction([this](kk::CString name,kk::Event * event)->void{
                            this->updateImage();
                        });
                    }
                    
                    v->on("load", (EventFunction *) _onEvent);
                    
                } else if(state == ImageStateLoaded) {
                    this->updateImage();
                }
                
            }
            
        }
        
        void ImageElement::updateImage() {
            if(_view != nullptr && _image != nullptr) {
                _view->setImage(_image);
            }
        }
        
        void ImageElement::unloadImage() {
            if(_image != nullptr) {
                if(_onEvent != nullptr) {
                    _image->off("load", (EventFunction *) _onEvent);
                }
                _image = nullptr;
            }
        }
        
        
        void ImageElement::onObtainView(ViewContext * context,View * view) {
            ViewElement::onObtainView(context, view);
            
            _app = context->app();
            
            if(_app != nullptr) {
                loadImage(_app, view);
            }
            
        }
        
        void ImageElement::onRecycleView(View * view) {
            
            unloadImage();
            
            ViewElement::onRecycleView(view);
        }
        
        void ImageElement::Openlib() {
            
            library("image");
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<ImageElement>(ctx, [](duk_context * ctx)->void{
                    
                });
                
            });
            
        }
        
        void ImageElement::library(kk::CString name) {
            Document::library(name, [](Document * document,kk::CString name,ElementKey elementId)->Element *{
                return new ImageElement(document,name,elementId);
            });
        }
        
    }
    
}

