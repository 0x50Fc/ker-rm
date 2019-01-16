//
//  LayoutElement.h
//  Ker
//
//  Created by zhanghailong on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef document_LayoutElement_h
#define document_LayoutElement_h

#include <document/StyleElement.h>

namespace kk {
    

    struct Unit {
        kk::Float scale;
        kk::Float rate;
        
        static kk::Float Auto;
    };
    
    struct Pixel {
        kk::Float value;
        kk::String name;
        Pixel();
        Pixel(kk::CString v);
        Pixel(const Pixel & v);
        virtual void set(kk::CString v);
        virtual void set(const Pixel & v);
        virtual kk::Boolean isAuto();
        static kk::Boolean is(kk::CString v);
    };
    
    struct Edge {
        Pixel top;
        Pixel right;
        Pixel bottom;
        Pixel left;
        Edge();
        Edge(kk::CString v);
        Edge(const Edge & v);
        virtual void set(kk::CString v);
        virtual void set(const Edge & v);
    };
    
    struct Point {
        Float x,y;
        Point();
        Point(Float x,Float y);
    };
    
    struct Size {
        Float width,height;
        Size();
        Size(Float width,Float height);
    };
    
    struct Rect {
        Point origin;
        Size size;
        Rect();
        Rect(Float x,Float y,Float width,Float height);
    };
    
    enum VerticalAlign {
        VerticalAlignTop,VerticalAlignMiddle,VerticalAlignBottom
    };
    
    VerticalAlign VerticalAlignFromCString(kk::CString v);
    
    class LayoutElement;
    
    class LayoutContext : public Object {
    public:
        LayoutContext();
        virtual void setSize(kk::Float width,kk::Float height);
        virtual void setUnit(kk::CString name,kk::Float scale,kk::Float rate);
        virtual void setLayout(kk::CString name,kk::TFunction<void,LayoutContext *,LayoutElement *> * func);
        virtual void setLayout(kk::CString name,JSObject * func);
        virtual kk::Float value(kk::Float value, kk::Float base,kk::Float defaultValue,kk::CString unit);
        virtual kk::Float value(Pixel & v,kk::Float base,kk::Float defaultValue);
        
        virtual void layout(LayoutElement * element);
        
        Ker_CLASS(LayoutContext,Object,"LayoutContext")
        
    protected:
    
        kk::Float _width;
        kk::Float _height;
        std::map<kk::CString,Unit> _units;
        std::map<kk::CString,kk::Strong<kk::Object>> _layouts;
    };
    
    class Layout {
    public:
        virtual kk::Boolean layout(LayoutContext * context) = 0;
    };
    
    class LayoutElement : public StyleElement {
    public:
        LayoutElement(Document * document,CString name, ElementKey elementId);
        virtual void changedKey(CString key);
        virtual void setFrame(Float x,Float y,Float width,Float height);
        Rect frame;
        Size contentSize;
        Pixel left;
        Pixel top;
        Pixel right;
        Pixel bottom;
        Pixel width;
        Pixel minWidth;
        Pixel maxWidth;
        Pixel height;
        Pixel minHeight;
        Pixel maxHeight;
        Edge padding;
        Edge margin;
        kk::Boolean hidden;
        VerticalAlign verticalAlign;
        
        Ker_CLASS(LayoutElement,StyleElement,"LayoutElement")
        static void Openlib();
        
        virtual void onLayout(LayoutContext * context);
        
    protected:

        
    };
    
}


#endif /* LayoutElement_h */
