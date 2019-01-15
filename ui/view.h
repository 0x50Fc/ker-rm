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
        
        class App;
        class View;
        
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
        
        class CanvasCommand : public Command {
        public:
            kk::Uint64 canvasId;
        };
        
        class CanvasDisplayCGContextCommand : public CanvasCommand {
        public:
            kk::Uint64 viewId;
            kk::Strong<kk::ui::CG::Context> context;
        };
        
        class Canvas : public EventEmitter {
        public:
            Canvas(View * view,DispatchQueue * queue,App * app,kk::Uint64 canvasId);
            Canvas(DispatchQueue * queue);
            virtual ~Canvas();
            virtual Strong<Object> getContext(kk::CString name);
            virtual Uint width();
            virtual void setWidth(Uint v);
            virtual Uint height();
            virtual void setHeight(Uint v);
            virtual Strong<Image> toImage();
            virtual DispatchQueue * queue();
            virtual App * app();
            virtual View * view();
            virtual kk::Uint64 canvasId();
            
            Ker_CLASS(Canvas, EventEmitter, "UICanvas");
            
            static void Openlib();
        protected:
            
            virtual kk::Strong<kk::ui::CG::Context> createCGContext();
            
            kk::Strong<Object> _context;
            kk::Strong<DispatchQueue> _queue;
            kk::Weak<App> _app;
            kk::Weak<View> _view;
            kk::Uint _width;
            kk::Uint _height;
            kk::Uint64 _canvasId;
            kk::Boolean _resize;
        };
        
        kk::Strong<Canvas> createCanvas(DispatchQueue * queue);
        
        typedef kk::Uint SubviewPosition;
        
        enum {
            SubviewPositionFront,SubviewPositionBack
        };
        
        class ViewConfiguration : public kk::Object {
            
        };
        
        class ViewCommand : public Command {
        public:
            kk::Uint64 viewId;
        };
        
        class ViewCreateCommand : public ViewCommand {
        public:
            kk::String name;
            kk::Strong<ViewConfiguration> configuration;
        };
        
        class ViewNativeCreateCommand : public ViewCommand {
        public:
            kk::Native * native;
        };
        
        class ViewDeleteCommand : public ViewCommand {
        public:
        };
        
        class ViewSetCommand : public ViewCommand {
        public:
            kk::String name;
            kk::String value;
        };
        
        class ViewSetFrameCommand : public ViewCommand {
        public:
            Rect frame;
        };
        
        class ViewAddsubviewCommand : public ViewCommand {
        public:
            kk::Uint64 subviewId;
            SubviewPosition position;
        };
        
        class ViewRemoveCommand : public ViewCommand {
        };
        
        class ViewSetContentSizeCommand : public ViewCommand {
        public:
            Size size;
        };
        
        class ViewSetContentOffsetCommand : public ViewCommand {
        public:
            Point offset;
            kk::Boolean animated;
        };
        
        class ViewEvaluateJavaScriptCommand : public ViewCommand {
        public:
            kk::String code;
        };
        
        class ViewSetContentCommand : public ViewCommand {
        public:
            kk::String content;
            kk::String contentType;
            kk::String basePath;
        };
        
        class ViewSetAttributedTextCommand : public ViewCommand {
        public:
            kk::Strong<AttributedText> text;
        };
        
        class ViewSetImageCommand : public ViewCommand {
        public:
            kk::Strong<Image> image;
        };
        
        class View : public EventEmitter {
        public:
            View(kk::CString name,ViewConfiguration * configuration,App * app,kk::Uint64 viewId);
            View(kk::Native * native,Rect & frame, App * app,kk::Uint64 viewId);
            virtual ~View();
            virtual void set(kk::CString name,kk::CString value);
            virtual void setFrame(Rect & frame);
            virtual void setFrame(Float x,Float y,Float width,Float height);
            virtual Rect & frame();
            virtual void setContentSize(Size & size);
            virtual void setContentSize(Float width,Float height);
            virtual void setContentOffset(Point & offset,kk::Boolean animated);
            virtual void setContentOffset(Float x,Float y,kk::Boolean animated);
            virtual Point contentOffset();
            virtual kk::Strong<Canvas> createCanvas(Worker * worker);
            virtual void addSubview(View * view,SubviewPosition position);
            virtual void removeView();
            virtual void removeAllSubviews();
            virtual kk::Strong<View> obtainView(kk::CString reuse);
            virtual void recycleView(View * view,kk::CString reuse);
            virtual void removeRecycleViews();
            
            virtual void evaluateJavaScript(kk::CString code);
            virtual ViewConfiguration * configuration();
            virtual void setContent(kk::CString content,kk::CString contentType,kk::CString basePath);
            
            virtual void setAttributedText(AttributedText * text);
            virtual void setImage(Image * image);
            
            virtual kk::Uint64 viewId();
            
            virtual App * app();
            
            virtual void dispatchCommand(ViewCommand * command);
            
            virtual View * parent();
            
            Ker_CLASS(View, EventEmitter, "UIView");
            
            static void Openlib();
        protected:
            std::map<kk::String,std::list<kk::Strong<View>>> _obtainViews;
            std::map<View *,Strong<View>> _subviews;
            Weak<View> _parent;
            Strong<ViewConfiguration> _configuration;
            Weak<App> _app;
            kk::Uint64 _viewId;
            Point _contentOffset;
            kk::Strong<Image> _image;
            kk::Strong<kk::TFunction<void, kk::CString, Event *>> _onImageLoadFunc;
            Rect _frame;
        };
        
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
            WebViewActionPolicy policy;
        };
        
        class WebViewConfiguration : public ViewConfiguration {
        public:
            virtual void addUserScript(kk::CString code,WebViewUserScriptInjectionTime injectionTime);
            virtual void addUserAction(kk::CString pattern,WebViewActionPolicy policy);
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
