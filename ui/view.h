//
//  view.h
//  kk
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef ui_view_h
#define ui_view_h

#include <ui/ui.h>
#include <ui/CGContext.h>
#include <core/event.h>
#include <core/dispatch.h>

namespace kk {
    
    namespace ui {
        
        enum AttributedTextSpanType {
            AttributedTextSpanTypeText,
            AttributedTextSpanTypeImage
        };
        
        struct AttributedTextSpan {
            AttributedTextSpanType type;
            kk::String text;
            kk::Strong<Image> image;
            kk::ui::Font font;
            kk::ui::Color color;
            kk::Uint width;
            kk::Uint height;
            Edge margin;
        };
        
        class AttributedText: public Object {
        public:
            AttributedText();
            virtual ~AttributedText();
            virtual void clear();
            virtual void append(kk::CString text,kk::ui::Font font,kk::ui::Color color);
            virtual void append(kk::ui::Image * image,kk::Uint width,kk::Uint height,kk::ui::Edge margin);
            virtual void appendImage(kk::ui::Image * image,kk::Uint width,kk::Uint height,kk::ui::Float top,kk::ui::Float left,kk::ui::Float bottom,kk::ui::Float right);
            virtual std::vector<AttributedTextSpan> & spans();
 
            Ker_CLASS(AttributedText, Object, "UIAttributedText");
            
            
            static void Openlib();
            
        protected:
            std::vector<AttributedTextSpan> _spans;
        };
        
        extern Size getAttributedTextContentSize(Context * context,AttributedText * text,kk::Float maxWidth);
        
        extern kk::CString kCanvasCGContext;
        extern kk::CString kCanvasWebGLContext;
        
        class Canvas : public EventEmitter {
        public:
            Canvas(DispatchQueue * queue);
            virtual Strong<Object> getContext(kk::CString name) = 0;
            virtual Uint width();
            virtual void setWidth(Uint v);
            virtual Uint height();
            virtual void setHeight(Uint v);
            virtual Strong<Image> toImage() = 0;
            virtual DispatchQueue * queue();
            
            Ker_CLASS(Canvas, EventEmitter, "UICanvas");
            
            static void Openlib();
        protected:
            kk::Strong<DispatchQueue> _queue;
            kk::Uint _width;
            kk::Uint _height;
        };
        
        kk::Strong<Canvas> createCanvas(DispatchQueue * queue);
        
        typedef kk::Uint SubviewPosition;
        
        enum {
            SubviewPositionFront,SubviewPositionBack
        };
        
        class ViewConfiguration : public kk::Object {
            
        };
        
        class View : public EventEmitter {
        public:
            View(ViewConfiguration * configuration,Context * context);
            virtual void set(kk::CString name,kk::CString value) = 0;
            virtual void setFrame(Rect & frame) = 0;
            virtual void setFrame(Float x,Float y,Float width,Float height);
            virtual void setContentSize(Size & size) = 0;
            virtual void setContentSize(Float width,Float height);
            virtual void setContentOffset(Point & offset,kk::Boolean animated) = 0;
            virtual void setContentOffset(Float x,Float y,kk::Boolean animated);
            virtual Point contentOffset() = 0;
            virtual kk::Strong<Canvas> createCanvas(Worker * worker) = 0;
            virtual void addSubview(View * view,SubviewPosition position);
            virtual void removeView();
            virtual void removeAllSubviews();
            virtual kk::Strong<View> obtainView(kk::CString reuse);
            virtual void recycleView(View * view,kk::CString reuse);
            virtual void removeRecycleViews();
            
            virtual void evaluateJavaScript(kk::CString code) = 0;
            virtual ViewConfiguration * configuration();
            virtual void setAttributedText(AttributedText * text) = 0;
            virtual void setImage(Image * image);
            virtual void setGravity(kk::CString gravity) = 0;
            
            virtual Context * context();
            
            Ker_CLASS(View, EventEmitter, "UIView");
            
            static void Openlib();
        protected:
            std::map<kk::String,std::list<kk::Strong<View>>> _obtainViews;
            std::map<View *,Strong<View>> _subviews;
            Weak<View> _parent;
            Strong<ViewConfiguration> _configuration;
            Weak<Context> _context;
            Strong<Image> _image;
        };
        
        kk::Strong<View> createView(kk::CString name,ViewConfiguration * configuration,Context * context);
        
        typedef kk::Uint WebViewActionPolicy;
        
        enum {
            WebViewActionPolicyCancel,WebViewActionPolicyAllow
        };
        
        typedef kk::Uint WebViewUserScriptInjectionTime;
    
        enum {
            WebViewUserScriptInjectionTimeAtDocumentStart,
            WebViewUserScriptInjectionTimeAtDocumentEnd
        };
        
        struct WebViewUserScript {
            kk::String code;
            WebViewUserScriptInjectionTime injectionTime;
        };
        
        struct WebViewUserAction {
            kk::String pattern;
            kk::String name;
            WebViewActionPolicy policy;
        };
        
        class WebViewConfiguration : public ViewConfiguration {
        public:
            virtual void addUserScript(kk::CString code,WebViewUserScriptInjectionTime injectionTime);
            virtual void addUserAction(kk::CString pattern,kk::CString name,WebViewActionPolicy policy);
            virtual std::vector<WebViewUserScript> & userScripts();
            virtual std::vector<WebViewUserAction> & userActions();
            
            Ker_CLASS(WebViewConfiguration, Object, "UIWebViewConfiguration");
            
            static void Openlib();
        protected:
            std::vector<WebViewUserScript> _userScripts;
            std::vector<WebViewUserAction> _userActions;
        };

        
    }
    
    
}

#endif /* view_h */
