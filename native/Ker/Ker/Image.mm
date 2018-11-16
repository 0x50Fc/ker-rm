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


namespace kk {
    
    namespace ui {
        
        class OSImage : public Image , public CGImageGetter {
        public:
            
            OSImage():_state(ImageStateNone) {
                
            }
            
            OSImage(CGImageRef image):_state(ImageStateLoaded) {
                _image = image;
                if(_image) {
                    CGImageRetain(_image);
                }
            }
            
            virtual ~OSImage() {
                if(_image) {
                    CGImageRelease(_image);
                }
            }
            
            virtual ImageState state() {
                return _state;
            }
            
            virtual kk::Uint width() {
                if(_image) {
                    return (kk::Uint) CGImageGetWidth(_image);
                }
                return 0;
            }
            
            virtual kk::Uint height() {
                if(_image) {
                    return (kk::Uint) CGImageGetHeight(_image);
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
                    
                    size_t width = CGImageGetWidth(_image);
                    size_t height = CGImageGetHeight(_image);
                    
                    CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
                    CGContextRef ctx = CGBitmapContextCreate(data, width, height, 8, width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
                    CGContextTranslateCTM(ctx, 0, height);
                    CGContextScaleCTM(ctx, 1, -1);
                    CGColorSpaceRelease(rgbSpace);
                    
                    CGContextDrawImage(ctx, CGRectMake(0, 0, width, height), _image);
                    
                    CGContextRelease(ctx);
                    
                }
                
            }
            
            virtual Boolean isCopyPixels() {
                return true;
            }
            
            virtual void setImage(CGImageRef image) {
                if(image) {
                    CGImageRetain(image);
                }
                if(_image) {
                    CGImageRelease(_image);
                }
                _image = image;
            }
            
            virtual void setState(ImageState state) {
                _state = state;
            }
            
            virtual CGImageRef CGImage() {
                return _image;
            }
            
        protected:
            CGImageRef _image;
            ImageState _state;
            kk::String _src;
        };
        
        kk::Strong<Image> ImageCreate(Context * context,kk::CString src) {
            
            kk::Strong<Image> v = new OSImage();
            
            v->setSrc(src);
            
            return v;
        }
    
        
        static std::function<void(Context * ,Image *)> _ImageLoader = [](Context * context,Image * image)->void{
            
            static kk::Strong<kk::DispatchQueue> _queue = nullptr;
            
            if(_queue == nullptr) {
                _queue = kk::createDispatchQueue("kk::ui::Image",kk::DispatchQueueTypeSerial);
            }
            
            kk::Weak<kk::DispatchQueue> current = context->queue();
            
            kk::Weak<Image> v(image);
            kk::Weak<Context> ctx(context);
            
            _queue->async([v,current,ctx]()->void{
                
                @autoreleasepool{
                    
                    kk::Strong<OSImage> image = dynamic_cast<OSImage *>(v.operator->());
                    kk::Strong<kk::DispatchQueue> queue = current.operator->();
                    kk::Strong<Context> context = ctx.operator->();
                    
                    if(image != nullptr && queue != nullptr && context != nullptr) {
                    
                        NSString * basePath = [NSString stringWithCString:context->basePath() encoding:NSUTF8StringEncoding];
                        NSString * path = [NSString stringWithCString:image->src() encoding:NSUTF8StringEncoding];
                        
                        if([path hasPrefix:@"http://"] || [path hasPrefix:@"https://"]) {
                            queue->async([v]()->void{
                                kk::Strong<OSImage> image = dynamic_cast<OSImage *>( v.operator->() );
                                if(image != nullptr) {
                                    image->setState(ImageStateError);
                                }
                            });
                        } else {
                            path = [basePath stringByAppendingPathComponent:path];
                            UIImage * i = [UIImage imageWithContentsOfFile:path];
                            if(i != nil) {
                                image->setImage(i.CGImage);
                                queue->async([v]()->void{
                                    kk::Strong<OSImage> image = dynamic_cast<OSImage *>( v.operator->() );
                                    if(image != nullptr) {
                                        image->setState(ImageStateLoaded);
                                    }
                                });
                            } else {
                                queue->async([v]()->void{
                                    kk::Strong<OSImage> image = dynamic_cast<OSImage *>( v.operator->() );
                                    if(image != nullptr) {
                                        image->setState(ImageStateError);
                                    }
                                });
                            }
                        }
                    }
                    
                }
                
            });
            
        };
        
        std::function<void(Context * ,Image *)> & getImageLoader() {
            return _ImageLoader;
        }
        
        void setImageLoader(std::function<void(Context * ,Image *)> && func) {
            _ImageLoader = func;
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
