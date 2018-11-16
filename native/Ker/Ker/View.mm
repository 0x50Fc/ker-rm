//
//  Canvas.m
//  KK
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>
#import "KerViewProtocol.h"

#include <ui/ui.h>
#include <ui/view.h>
#include <ui/CGContext.h>
#include <core/dispatch.h>




namespace kk {
    
    namespace ui {
        
        extern kk::Strong<kk::ui::CG::Context> createCGContext(kk::Uint width,kk::Uint height);
        extern void displayCGContext(kk::ui::CG::Context * context,CFTypeRef view);
        extern kk::Strong<Image> createImageWithCGContext(kk::ui::CG::Context * context);
        
        class OSCanvas : public Canvas {
        public:
            
            OSCanvas(DispatchQueue * queue):OSCanvas(queue, nullptr) {
                
            }
            
            OSCanvas(DispatchQueue * queue, CFTypeRef view):Canvas(queue),_width(0),_height(0),_resize(false) {
                @autoreleasepool {
                    _view = view;
                    CFRetain(view);
                    if(_view != nil) {
                        UIView * v = (__bridge UIView *) _view;
                        __block CGRect bounds;
                        __block CGFloat scale;
                        if(queue == mainDispatchQueue()) {
                            bounds = v.bounds;
                            scale = v.layer.contentsScale;
                        } else {
                            dispatch_sync(dispatch_get_main_queue(), ^{
                                bounds = v.bounds;
                                scale = v.layer.contentsScale;
                            });
                        }
                        setWidth((kk::Uint) ceil( bounds.size.width * scale));
                        setHeight((kk::Uint) ceil( bounds.size.height * scale));
                    }
                }
            }
            
            virtual ~OSCanvas() {
                @autoreleasepool {
                    CFRelease(_view);
                }
            }
            
            virtual Strong<Object> getContext(kk::CString name) {
                
                if(name == nullptr ){
                    return nullptr;
                }
                
                if(_width == 0 || _height == 0) {
                    return nullptr;
                }
                
                if(strcmp(name, "2d") == 0) {
                    
                    kk::Strong<kk::ui::CG::Context> v = (kk::ui::CG::Context *) _context;
                    
                    if(v == nullptr || _resize) {
                        v = createCGContext(_width,_height);
                        _context = v;
                    } else {
                        v->clearRect(0, 0, _width, _height);
                    }
                    
                    if(_view != nil) {
                        kk::Weak<OSCanvas> canvas = this;
                        _queue->async([v,canvas]()->void{
                            kk::Strong<OSCanvas> c = canvas.operator->();
                            if(c != nullptr) {
                                displayCGContext(v.operator->(),c->_view);
                            }
                        });
                    }
                    
                }
                
                _resize = false;
                
                return _context;
            }
            
            virtual Uint width() {
                return _width;
            }
            
            virtual void setWidth(Uint v) {
                if(_width != v) {
                    _width = v;
                    _resize = true;
                }
            }
            
            virtual Uint height() {
                return _height;
            }
            
            virtual void setHeight(Uint v) {
                if(_height != v) {
                    _height = v;
                    _resize = true;
                }
            }
            
            virtual Strong<Image> toImage() {
                {
                    kk::ui::CG::Context * v =dynamic_cast<kk::ui::CG::Context *>(_context.get());
                    if(v) {
                        return createImageWithCGContext(v);
                    }
                }
                return nullptr;
            }
            
        protected:
            CFTypeRef _view;
            kk::Uint _width;
            kk::Uint _height;
            kk::Boolean _resize;
            kk::Strong<Object> _context;
        };
        
        class OSView : public View {
        public:
            
            OSView(CFTypeRef view,ViewConfiguration * configuration,Context * context):View(configuration,context) {
                @autoreleasepool {
                    _view = view;
                    CFRetain(view);
                    UIView * v = (__bridge UIView *) _view;
                    if([v respondsToSelector:@selector(KerViewObtain:)]) {
                        [(id<KerViewProtocol>) v KerViewObtain:this];
                    }
                }
            }
            
            virtual ~OSView() {
                @autoreleasepool {
                    UIView * v = (__bridge UIView *) _view;
                    if([v respondsToSelector:@selector(KerViewRecycle:)]) {
                        [(id<KerViewProtocol>) v KerViewRecycle:this];
                    }
                    CFRelease(_view);
                }
                kk::Log("[OSView] [dealloc]");
            }
            
            virtual void set(kk::CString name,kk::CString value) {
                @autoreleasepool {
                    UIView * v = (__bridge UIView *) _view;
                    if([v respondsToSelector:@selector(KerViewSetAttribute:value:)]) {
                        [(id<KerViewProtocol>) v KerViewSetAttribute:name value:value];
                    }
                }
            }
            
            virtual void setFrame(Rect & frame) {
                @autoreleasepool {
                    UIView * v = (__bridge UIView *) _view;
                    [v setFrame:CGRectMake(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height)];
                }
            }
            
            virtual void setContentSize(Size & size) {
                @autoreleasepool {
                    UIView * v = (__bridge UIView *) _view;
                    
                    if([v respondsToSelector:@selector(KerViewContentView)]) {
                        v = [(id<KerViewProtocol>) v KerViewContentView];
                    }
                    
                    if([v isKindOfClass:[UIScrollView class]]) {
                        [(UIScrollView *) v setContentSize:CGSizeMake(size.width, size.height)];
                    }
                }
            }
            
            virtual void setContentOffset(Point & offset) {
                @autoreleasepool {
                    
                    UIView * v = (__bridge UIView *) _view;
                    
                    if([v respondsToSelector:@selector(KerViewContentView)]) {
                        v = [(id<KerViewProtocol>) v KerViewContentView];
                    }
                    
                    if([v isKindOfClass:[UIScrollView class]]) {
                        [(UIScrollView *) v setContentOffset:CGPointMake(offset.x, offset.y)];
                    }
                }
            }
            
            virtual Point contentOffset() {
                @autoreleasepool {
                    
                    UIView * v = (__bridge UIView *) _view;
                    
                    if([v respondsToSelector:@selector(KerViewContentView)]) {
                        v = [(id<KerViewProtocol>) v KerViewContentView];
                    }
                    
                    if([v isKindOfClass:[UIScrollView class]]) {
                        CGPoint p = [(UIScrollView *) v contentOffset];
                        return { (Float) p.x, (Float)  p.y};
                    }
                    
                    return {0,0};
                }
            }
            
            virtual kk::Strong<Canvas> createCanvas(Worker * worker) {
                
                if(worker == nullptr || worker->context() == nullptr) {
                    return new OSCanvas(mainDispatchQueue(),_view);
                }
                
                kk::Strong<Worker> w = worker;
                kk::Weak<OSView> view = this;
                
                worker->context()->queue()->async([w,view]()->void{
                    
                    kk::Strong<OSView> vw = view.operator->();
                    kk::Strong<Worker> v = w.operator->();
                    
                    if(v != nullptr && v->context() != nullptr && vw != nullptr) {
                        
                        kk::Any c = new OSCanvas(v->context()->queue(),vw->_view);
                        
                        JITContext::current()->forEach(v.get(), [&c](duk_context * ctx,void * heapptr)->void{
                            
                            duk_push_heapptr(ctx, heapptr);
                            
                            duk_get_prop_string(ctx, -1 ,"oncanvas");
                            
                            if(duk_is_function(ctx, -1)) {
                                
                                PushAny(ctx, c);
                                
                                if(duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
                                    
                                    kk::Error(ctx, -1, "[kk::ui::OSView::createCanvas] ");
                                }
                            }
                            
                            duk_pop_2(ctx);
                            
                        });
                        
                    }
                });
        
                return nullptr;
                
            }
            
            virtual void addSubview(View * view,SubviewPosition position) {
                OSView * ov = dynamic_cast<OSView *>(view);
                if(ov == nullptr) {
                    return ;
                }
                @autoreleasepool {
                    UIView * v = (__bridge UIView *) _view;
                    if([v respondsToSelector:@selector(KerViewContentView)]) {
                        v = [(id<KerViewProtocol>) v KerViewContentView];
                    }
                    UIView * b = (__bridge UIView *) ov->_view;
                    if(position == SubviewPositionFront) {
                        [v addSubview:b];
                    } else {
                        [v insertSubview:b atIndex:0];
                    }
                }
                View::addSubview(view, position);
            }
            
            virtual void removeView() {
                @autoreleasepool {
                    UIView * v = (__bridge UIView *) _view;
                    [v removeFromSuperview];
                }
                View::removeView();
            }
            
            virtual void evaluateJavaScript(kk::CString code) {
                if(code == nullptr) {
                    return;
                }
                @autoreleasepool {
                    UIView * v = (__bridge UIView *) _view;
                    if([v respondsToSelector:@selector(evaluateJavaScript:completionHandler:)]) {
                        [(WKWebView *) v evaluateJavaScript:[NSString stringWithCString:code encoding:NSUTF8StringEncoding] completionHandler:nil];
                    }
                }
            }
            
        protected:
            CFTypeRef _view;
        };
        
        kk::Strong<View> createView(kk::CString name,ViewConfiguration * configuration,Context * context) {
            
            if(name == nullptr) {
                return nullptr;
            }
            
            @autoreleasepool {
                
                ::Class isa = NSClassFromString([NSString stringWithCString:name encoding:NSUTF8StringEncoding]);
                
                if(isa == nullptr) {
                    isa = [UIView class];
                }
                
                UIView * view = [isa KerViewCreateWithConfiguration:configuration];
                
                return new OSView((__bridge CFTypeRef) view,configuration,context);
                
            }
            
        }
        
        kk::Strong<View> createView(CFTypeRef view,ViewConfiguration * configuration,Context * context) {
            return new OSView(view,configuration,context);
        }
        
        kk::Strong<Canvas> createCanvas(DispatchQueue * queue) {
            return new OSCanvas(queue);
        }
        
    }
    
}
