//
//  Image.m
//  KK
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>

#include <ui/ui.h>
#include <core/dispatch.h>
#import "UI.h"
#import "UIImage+Ker.h"

namespace kk {
    extern ::dispatch_queue_t DispatchQueueGCD(DispatchQueue * queue);
}


namespace kk {
    
    namespace ui {
        
        NativeImage::NativeImage(kk::DispatchQueue * queue,kk::CString src):_src(src),_state(ImageStateNone),_width(0),_height(0),_queue(queue) {
            
            @autoreleasepool {
                
                NSString * URI = [NSString stringWithCString:src encoding:NSUTF8StringEncoding];
                
                UIImage * image = [UIImage ker_imageByCacheWithURI:URI];
                
                if(image != nil) {
                    setNative((__bridge kk::Native *) image);
                    kk::Weak<NativeImage> v = this;
                    queue->async([v]()->void{
                        kk::Strong<NativeImage> vv = v.operator->();
                        if(vv != nullptr) {
                            v->setState(ImageStateLoaded);
                        }
                    });
                } else {
                    
                    NativeImage * i = this;
                    
                    i->retain();
                    
                    [UIImage ker_imageWithURI:URI callback:^(UIImage *image, NSError *err) {
                        
                        if(image) {
                            i->setNative((__bridge kk::Native *) image);
                            i->setState(ImageStateLoaded);
                        } else {
                            i->setState(ImageStateError);
                        }
                        
                        i->release();
                        
                    } queue:kk::DispatchQueueGCD(_queue)];
                    
                }
                
            }
            
        }
        
        kk::Native * NativeImage::native() {
            if(_native != nullptr) {
                return _native->native();
            }
            return nullptr;
        }
        
        void NativeImage::setNative(kk::Native * native) {
            kk::Uint width = 0;
            kk::Uint height = 0;
            if(native) {
                @autoreleasepool {
                    UIImage * image = (__bridge UIImage *) native;
                    width = (kk::Uint) image.size.width;
                    width = (kk::Uint) image.size.height;
                }
            }
            setNative(native,width,height);
        }
        
        void NativeImage::setNative(kk::Native * native,kk::Uint width,kk::Uint height) {
            if(native == nullptr) {
                _native = nullptr;
                _width = 0;
                _height = 0;
            } else {
                _native = new kk::NativeObject(native);
                _width = width;
                _height = height;
            }
        }
        
        ImageState NativeImage::state() {
            return _state;
        }
        
        kk::Uint NativeImage::width() {
            return _width;
        }
        
        kk::Uint NativeImage::height() {
            return _height;
        }
        
        kk::CString NativeImage::src() {
            return _src.c_str();
        }
        
        void NativeImage::copyPixels(void * data) {
            
            if(_native != nullptr) {
                
                CGImageRef image = [(__bridge UIImage *) _native->native() CGImage];
                
                size_t width = CGImageGetWidth(image);
                size_t height = CGImageGetHeight(image);
                
                CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
                CGContextRef ctx = CGBitmapContextCreate(data, width, height, 8, width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
                CGContextTranslateCTM(ctx, 0, height);
                CGContextScaleCTM(ctx, 1, -1);
                CGColorSpaceRelease(rgbSpace);
                
                CGContextDrawImage(ctx, CGRectMake(0, 0, width, height), image);
                
                CGContextRelease(ctx);
                
            }
            
        }
        
        Boolean NativeImage::isCopyPixels() {
            return _width > 0 && _height > 0 && _native != nullptr;
        }
        
        void NativeImage::setState(ImageState state) {
            if(_state != state) {
                _state = state;
                if(state == ImageStateError) {
                    Strong<Event> e = new Event();
                    emit("error", e);
                } else if(state == ImageStateLoaded) {
                    Strong<Event> e = new Event();
                    emit("load", e);
                }
            }
        }
        
        kk::DispatchQueue * NativeImage::queue() {
            return _queue;
        }
        
        CGImageRef GetCGImage(Image * image) {
            {
                CGImageGetter * v = dynamic_cast<CGImageGetter *>(image);
                if(v != nullptr) {
                    return v->CGImage();
                }
            }
            {
                NativeImage * v = dynamic_cast<NativeImage *>(image);
                if(v != nullptr) {
                    return [(__bridge UIImage *) v->native() CGImage];
                }
            }
            return nullptr;
        }
        
        Strong<Image> createCGImage(kk::DispatchQueue * queue, CGImageRef image) {
            
            Strong<NativeImage> v = new NativeImage(queue,"");
            
            @autoreleasepool {
                v->setNative((__bridge kk::Native *) [UIImage imageWithCGImage:image]);
            }
            
            queue->async([v]()->void{
                v->setState(ImageStateLoaded);
            });
            
            return (NativeImage *) v;
        }
        
    }
    
}
