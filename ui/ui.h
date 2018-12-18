//
//  ui.h
//  kk
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef ui_ui_h
#define ui_ui_h

#include <core/kk.h>
#include <core/event.h>
#include <core/dispatch.h>
#include <core/jit.h>
#include <core/timer.h>
#include <core/sqlite.h>

namespace kk {
    
    namespace ui {
        
        typedef kk::Float Float;
        
        struct Point {
            Float x,y;
            operator kk::Any() {
                return kk::Any(new TObject<kk::String, kk::Float>({{"x",x},{"y",y}}));
            }
        };
        
        struct Size {
            Float width,height;
            Size();
            Size(Float width,Float height);
            operator kk::Any() {
                return kk::Any(new TObject<kk::String, kk::Float>({{"width",width},{"height",height}}));
            }
        };
        
        struct Rect {
            Point origin;
            Size size;
            operator kk::Any() {
                return kk::Any(new TObject<kk::String, kk::Float>({{"x",origin.x},{"y",origin.y},{"width",size.width},{"height",size.height}}));
            }
        };
        
        struct Edge {
            Float top,left,bottom,right;
            Edge();
            Edge(Float top,Float right,Float bottom,Float left);
            Edge(kk::CString v);
            Edge(Any &v):Edge((kk::CString) v){};
            operator kk::Any() {
                return kk::Any(new TObject<kk::String, kk::Float>({{"top",top},{"left",left},{"bottom",bottom},{"right",right}}));
            }
        };
        
        struct Color {
        public:
            Color();
            Color(Float r,Float g,Float b,Float a);
            Color(kk::CString v);
            Color(kk::Uint v);
            Color(Any &v):Color((kk::CString) v){};
            Float r,g,b,a;
            operator kk::Any() {
                char data[64];
                if(a == 1.0) {
                    snprintf(data, sizeof(data), "#%02x%02x%02x",(kk::Uint)(r * 255),(kk::Uint)(g * 255),(kk::Uint)(b * 255));
                } else {
                    snprintf(data, sizeof(data), "rgba(%g,%g,%g,%g)",(r * 255),(g * 255),(b * 255),a);
                }
                return kk::Any((CString)data);
            }
            Color & operator=(Any &v) {
                Color c((kk::CString)v);
                r = c.r;
                g = c.g;
                b = c.b;
                a = c.a;
                return * this;
            }
        };
        
        class Palette {
        public:
            Palette(std::initializer_list<std::pair<const kk::String,Color>> &&v);
            virtual void set(kk::CString name,Color v);
            virtual Color get(kk::CString name);
            static Palette Default;
        protected:
            std::map<kk::String,Color> _values;
        };
        
        enum FontStyle {
            FontStyleNormal,FontStyleItalic
        };
        
        enum FontWeight {
            FontWeightNormal,FontWeightBold
        };
        
        struct Font {
        public:
            Font():size(14),style(FontStyleNormal),weight(FontWeightNormal){}
            Font(Float size):size(size),style(FontStyleNormal),weight(FontWeightNormal){}
            Font(kk::CString family,Float size,FontStyle style,FontWeight weight):family(family),size(size),style(style),weight(weight){}
            Font(kk::CString v);
            Font(kk::Any &v):Font((kk::CString) v){};
            kk::String family;
            Float size;
            FontStyle style;
            FontWeight weight;
            operator kk::Any(){
                std::vector<kk::String> items;
                
                if(family != "") {
                    items.push_back(family);
                }
                
                if(weight == FontWeightBold) {
                    items.push_back("bold");
                }
                
                if(style == FontStyleItalic) {
                    items.push_back("italic");
                }
                
                char fmt[32];
                
                snprintf(fmt, sizeof(fmt), "%gpx",size);
                
                items.push_back(fmt);
                
                return kk::Any(CStringJoin(items, " "));
            }
        };
        
        enum TextAlign {
            TextAlignStart,
            TextAlignEnd,
            TextAlignCenter,
            TextAlignLeft,
            TextAlignRight
        };
        
        TextAlign TextAlignFromString(kk::CString string);
        kk::CString StringFromTextAlign(TextAlign v);
        
        enum TextBaseline {
            TextBaselineAlphabetic,
            TextBaselineTop,
            TextBaselineHanging,
            TextBaselineMiddle,
            TextBaselineIdeographic,
            TextBaselineBottom,
        };
        
        TextBaseline TextBaselineFromString(kk::CString string);
        kk::CString StringFromTextBaseline(TextBaseline v);
        
        struct Transform {
            Float a, b;
            Float c, d;
            Float tx, ty;
        };
        
        extern Transform TransformIdentity;
        
        Transform TransformTranslate(Transform t, Float tx, Float ty);
        
        Transform TransformScale(Transform t, Float sx, Float sy);
        
        Transform TransformRotate(Transform t, Float angle);
        
        Transform TransformFromString(kk::CString v);
        
        enum ImageState {
            ImageStateNone,ImageStateLoading,ImageStateError,ImageStateLoaded
        };
        
        class Image : public kk::EventEmitter {
        public:
            virtual ImageState state() = 0;
            virtual kk::Uint width() = 0;
            virtual kk::Uint height() = 0;
            virtual kk::CString src() = 0;
            virtual void copyPixels(void * data) = 0;
            virtual Boolean isCopyPixels() = 0;
            
            Ker_CLASS(Image,EventEmitter,"Image")
        };
        
        
        class Context;
        
        class Worker : public Object {
        public:
            Worker(Context * main,kk::CString path);
            virtual ~Worker();
            virtual void postMessage(Any data);
            virtual void terminate();
            virtual Context * context();
           
            Ker_CLASS(Worker,Object,"Worker")
            
            static void Openlib();
            
        protected:
            
            virtual void onBackgroundMessage(Any & data);
            virtual void onMessage(Any & data);
            
            kk::Weak<Context> _main;
            kk::DispatchQueue * _queue;
            Context * _context;
        };
        
        class Canvas;
        
        class Context : public EventEmitter, public Container {
        public:
            Context(kk::CString basePath,kk::DispatchQueue * queue);
            virtual ~Context();
            virtual kk::CString basePath();
            virtual kk::DispatchQueue * queue();
            virtual duk_context * jsContext();
            virtual kk::String absolutePath(kk::CString path);
            virtual kk::String getTextContent(kk::CString path);
            virtual void set(kk::Object * object);
            virtual kk::Object * get(kk::Object * object);
            virtual void remove(kk::Object * object);
            virtual void exec(kk::CString path,TObject<String, Any> * librarys);
            virtual void exec(kk::CString path,JSObject * librarys);
            virtual kk::Strong<Worker> createWorker(kk::CString path);
            virtual kk::Strong<Canvas> createCanvas();
            virtual kk::Strong<Image> createImage(kk::CString src);
            virtual kk::Strong<Sqlite> createSqlite(kk::CString path);
            virtual kk::Strong<Context> parent();
            virtual void setParent(Context * v);
            static void Openlib();
            
            Ker_CLASS(Context, EventEmitter, "UIContext");
            
        protected:
            kk::Weak<Context> _parent;
            kk::String _basePath;
            kk::Strong<kk::DispatchQueue> _queue;
            duk_context * _jsContext;
            std::map<void *,kk::Strong<kk::Object>> _objects;
        };
        
        kk::Strong<Image> ImageCreate(Context * context,kk::CString src);
        
        std::function<void(Context * ,Image *)> & getImageLoader();
        
        void setImageLoader(std::function<void(Context * ,Image *)> && func);
        
    }
    
    
}

#endif /* ui_h */
