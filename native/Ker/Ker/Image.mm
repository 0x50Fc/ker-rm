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
        
        class OSImage : public Image , public CGImageGetter {
        public:
            
            OSImage(kk::CString src):_state(ImageStateNone),_src(src),_image(nil) {
                
            }
            
            OSImage(CFTypeRef image):_state(ImageStateLoaded) {
                _image = image;
                if(_image) {
                    CFRetain(_image);
                }
            }
            
            virtual ~OSImage() {
                if(_image) {
                    CFRelease(_image);
                }
            }
            
            virtual ImageState state() {
                return _state;
            }
            
            virtual kk::Uint width() {
                if(_image) {
                    return (kk::Uint) [(__bridge UIImage *) _image size].width;
                }
                return 0;
            }
            
            virtual kk::Uint height() {
                if(_image) {
                    return (kk::Uint) [(__bridge UIImage *) _image size].height;
                }
                return 0;
            }
            
            virtual kk::CString src() {
                return _src.c_str();
            }
            
            virtual void setSrc(kk::CString src) {
                _src = src;
            }
            
            virtual void copyPixels(void * data) {
                
                if(_image != nullptr) {
                    
                    CGImageRef image = CGImage();
                    
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
            
            virtual Boolean isCopyPixels() {
                return _image != nullptr;
            }
            
            virtual void setImage(CFTypeRef image) {
                if(image) {
                    CFRetain(image);
                }
                if(_image) {
                    CFRelease(_image);
                }
                _image = image;
            }
            
            virtual void setState(ImageState state) {
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
            
            virtual CGImageRef CGImage() {
                return [(__bridge UIImage *) _image CGImage];
            }
            
        protected:
            CFTypeRef _image;
            ImageState _state;
            kk::String _src;
        };
        
        kk::Strong<Image> ImageCreate(Context * context,kk::CString src) {
            
            if(src == nullptr) {
                return nullptr;
            }
            
            OSImage * i = new OSImage(src);
            
            kk::Strong<Image> v = i;
            
            @autoreleasepool {
                
                NSString * basePath = [NSString stringWithCString:context->basePath() encoding:NSUTF8StringEncoding];
                NSString * URI = [NSString stringWithCString:src encoding:NSUTF8StringEncoding];
                
                UIImage * image = [UIImage ker_imageByCacheWithURI:URI basePath:basePath];
                
                if(image != nil) {
                    i->setImage((__bridge CFTypeRef) image);
                    i->setState(ImageStateLoaded);
                } else {
                    
                    i->retain();
                    
                    [UIImage ker_imageWithURI:URI basePath:basePath callback:^(UIImage *image, NSError *err) {
                        
                        if(image) {
                            i->setImage((__bridge CFTypeRef) image);
                            i->setState(ImageStateLoaded);
                        } else {
                            i->setState(ImageStateError);
                        }
                        
                        i->release();
                        
                    } queue:kk::DispatchQueueGCD(context->queue())];
                    
                }
            }
            
            return v;
        }
        
        CGImageRef GetCGImage(Image * image) {
            CGImageGetter * v = dynamic_cast<CGImageGetter *>(image);
            if(v != nullptr) {
                return v->CGImage();
            }
            return nullptr;
        }
        
        Strong<Image> createCGImage(CGImageRef image) {
            return new OSImage(image);
        }
        
    }
    
}
