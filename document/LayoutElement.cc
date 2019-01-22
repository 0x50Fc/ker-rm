//
//  LayoutElement.cc
//  Ker
//
//  Created by zhanghailong on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "LayoutElement.h"
#include <core/jit.h>
#include "BlockElement.h"

namespace kk {

    kk::Float Unit::Auto = 0x1.fffffep+127f;
    
    Pixel::Pixel(kk::CString v):value(0),name("auto") {
        set(v);
    }
    
    Pixel::Pixel():value(0),name("auto") {
    }
    
    Pixel::Pixel(const Pixel & v):value(v.value),name(v.name) {

    }
    
    kk::Boolean Pixel::isAuto() {
        return name == "auto";
    }
    
    void Pixel::set(kk::CString v) {
        
        if(kk::CStringEqual(v, "auto")) {
            name = "auto";
            value = 0;
            return;
        }
        
        name.clear();
        value = 0;
        
        if(v != nullptr) {
            char * p = (char *) v;
            kk::String vv;
            while(*p) {
                if((*p >='0' && *p <= '9') || *p == '.' || *p == '-' ) {
                    vv.append(p,1);
                } else {
                    name.append(p,1);
                }
                p ++;
            }
            value = atof(vv.c_str());
        }
        
        if(name.empty()) {
            name = "px";
        }
    }
    
    void Pixel::set(const Pixel & v) {
        value = v.value;
        name = v.name;
    }
    
    kk::Boolean Pixel::is(kk::CString v) {
        if(v == nullptr) {
            return false;
        }
        char * p = (char *) v;
        while(*p){
            if(*p >='0' && * p <='9') {
                return true;
            }
            p ++;
        }
        return false;
    }
    
    Edge::Edge() {
        
    }
    
    Edge::Edge(kk::CString v) {
        set(v);
    }
    
    Edge::Edge(const Edge & v) {
        top.set(v.top);
        right.set(v.right);
        bottom.set(v.bottom);
        left.set(v.left);
    }
    
    void Edge::set(kk::CString v) {
        std::vector<kk::String> vs;
        kk::CStringSplit(v, " ", vs);
        auto i = vs.begin();
        auto e = vs.end();
        if(i != e) {
            top.set(i->c_str());
            i ++;
            if(i != e) {
                right.set(i->c_str());
                i ++;
                if(i != e) {
                    bottom.set(i->c_str());
                    i ++;
                    if(i != e) {
                        left.set(i->c_str());
                    } else {
                        left.set(right);
                    }
                } else {
                    bottom.set(top);
                    left.set(right);
                }
            } else {
                right.set(top);
                bottom.set(top);
                left.set(top);
            }
        } else {
            top.set(nullptr);
            right.set(nullptr);
            bottom.set(nullptr);
            left.set(nullptr);
        }
    }
    
    void Edge::set(const Edge & v) {
        top.set(v.top);
        right.set(v.right);
        bottom.set(v.bottom);
        left.set(v.left);
    }
    
    Point::Point():x(0),y(0) {
        
    }
    
    Point::Point(Float x,Float y):x(x),y(y) {
        
    }
    
    Size::Size():width(0),height(0) {
        
    }
    
    Size::Size(Float width,Float height):width(width),height(height) {
        
    }
    
    Rect::Rect() {
        
    }
    
    Rect::Rect(Float x,Float y,Float width,Float height):origin(x,y),size(width,height) {
        
    }
    
    VerticalAlign VerticalAlignFromCString(kk::CString v) {
        if(v == nullptr) {
            return VerticalAlignTop;
        }
        if(kk::CStringEqual(v, "middle")) {
            return VerticalAlignMiddle;
        }
        if(kk::CStringEqual(v, "bottom")) {
            return VerticalAlignBottom;
        }
        return VerticalAlignTop;
    }
    
    static void LayoutElementForEach(Element * p,std::function<void(LayoutElement *)> && item) {
        
        p = p->firstChild();
        
        while(p) {
            
            {
                BlockElement * e = dynamic_cast<BlockElement *>(p);
                if(e != nullptr) {
                    LayoutElementForEach(e,std::move(item));
                    p = p->nextSibling();
                    continue;
                }
            }
            
            {
                LayoutElement * e = dynamic_cast<LayoutElement *>(p);
                if(e != nullptr) {
                    item(e);
                    p = p->nextSibling();
                    continue;
                }
            }
            
            p = p->nextSibling();
        }
        
    }
    
    /**
     * 相对布局 "relative"
     */
    static void LayoutElementRelative(LayoutContext * context,LayoutElement * element) {
        
        if(element == nullptr) {
            return;
        }
        
        Size size = element->frame.size;
        Edge padding = element->padding;
        Float paddingLeft = context->value(padding.left, size.width, 0);
        Float paddingRight = context->value(padding.right, size.width, 0);
        Float paddingTop = context->value(padding.top, size.height, 0);
        Float paddingBottom = context->value(padding.bottom, size.height, 0);
        Size inSize = {size.width - paddingLeft - paddingRight,size.height - paddingTop - paddingBottom};
        
        Size contentSize;
        
        LayoutElementForEach(element,[&](LayoutElement * e)->void{
            
            Float mleft = context->value(e->margin.left, size.width, 0);
            Float mright = context->value(e->margin.right, size.width, 0);
            Float mtop = context->value(e->margin.top, size.height, 0);
            Float mbottom = context->value(e->margin.bottom, size.height, 0);
            
            Float width = context->value(e->width, inSize.width - mleft - mright, Unit::Auto);
            Float height = context->value(e->height, inSize.height - mtop - mbottom, Unit::Auto);
            
            Rect & v = e->frame;
            
            v.size.width = width;
            v.size.height = height;
            
            context->layout(e);
            
            if(width == Unit::Auto) {
                width = v.size.width = e->contentSize.width;
                Float min = context->value(e->minWidth, inSize.width, 0);
                Float max = context->value(e->maxWidth, inSize.width, Unit::Auto);
                if(v.size.width < min) {
                    width = v.size.width = min;
                }
                if(v.size.width > max) {
                    width = v.size.width = max;
                }
            }
            
            if(height == Unit::Auto) {
                height = v.size.height = e->contentSize.height;
                Float min = context->value(e->minHeight, inSize.height, 0);
                Float max = context->value(e->maxHeight, inSize.height, Unit::Auto);
                if(v.size.height < min) {
                    height = v.size.height = min;
                }
                if(v.size.height > max) {
                    height = v.size.height = max;
                }
            }
            
            Float left = context->value(e->left, inSize.width, Unit::Auto);
            Float right = context->value(e->right, inSize.width, Unit::Auto);
            Float top = context->value(e->top, inSize.height, Unit::Auto);
            Float bottom = context->value(e->bottom, inSize.height, Unit::Auto);
            
            if(left == Unit::Auto) {
                
                if(size.width == Unit::Auto) {
                    left = paddingLeft + mleft;
                } else if(right == Unit::Auto) {
                    left = paddingLeft + mleft + (inSize.width - width - mleft - mright) * 0.5f;
                } else {
                    left = paddingLeft + (inSize.width - right - mright - width);
                }
                
            } else {
                left = paddingLeft + left + mleft;
            }
            
            if(top == Unit::Auto) {
                
                if(size.height == Unit::Auto) {
                    top = paddingTop + mtop;
                } else if(bottom == Unit::Auto) {
                    top = paddingTop + mtop + (inSize.height - height - mtop - mbottom) * 0.5f;
                } else {
                    top = paddingTop + (inSize.height - height - mbottom - bottom);
                }
                
            } else {
                top = paddingTop + top + mtop;
            }
            
            v.origin.x = left ;
            v.origin.y = top ;
            
            if(left + paddingRight + mright + v.size.width > contentSize.width) {
                contentSize.width = left + paddingRight + mright + v.size.width;
            }
            
            if(top + paddingBottom + mbottom  + v.size.height > contentSize.height) {
                contentSize.height = top + paddingBottom + mbottom + v.size.height ;
            }
            
            e->onLayout(context);
            
        });
        
        element->contentSize = contentSize;
        
    }
    
    /**
     * 流式布局 "flex" 左到右 上到下
     */
    static void _LayoutElementLine(LayoutContext * context,std::vector<LayoutElement *> & elements,Size inSize,Float lineHeight) {
        
        auto i = elements.begin();
        auto e = elements.end();
        
        while(i != e) {
            
            LayoutElement * element = * i;
            
            VerticalAlign v = element->verticalAlign;
            
            if(v == VerticalAlignBottom) {
                Rect & r = element->frame;
                Float mbottom = context->value(element->margin.bottom, inSize.height, 0);
                Float mtop = context->value(element->margin.top, inSize.height, 0);
                r.origin.y = r.origin.y + (lineHeight - mtop - mbottom - r.size.height);
            } else if(v == VerticalAlignMiddle) {
                Rect & r = element->frame;
                Float mbottom = context->value(element->margin.bottom, inSize.height, 0);
                Float mtop = context->value(element->margin.top, inSize.height, 0);
                r.origin.y = r.origin.y + (lineHeight - mtop - mbottom - r.size.height) * 0.5;
            }
            
            element->onLayout(context);
            
            i ++;
        }
        
    }

    static void _LayoutElementFlex(LayoutContext * context,LayoutElement * element,kk::Boolean nowarp) {
        Size size = element->frame.size;
        Edge padding = element->padding;
        Float paddingLeft = context->value(padding.left, size.width, 0);
        Float paddingRight = context->value(padding.right, size.width, 0);
        Float paddingTop = context->value(padding.top, size.height, 0);
        Float paddingBottom = context->value(padding.bottom, size.height, 0);
        Size inSize = {size.width - paddingLeft - paddingRight,size.height - paddingTop - paddingBottom};
        
        Float y = paddingTop;
        Float x = paddingLeft;
        Float maxWidth = paddingLeft + paddingRight;
        Float lineHeight = 0;
        
        std::vector<LayoutElement *> lineElements;
        
        LayoutElementForEach(element,[&](LayoutElement * e)->void{
            
            if(e->hidden) {
                return;
            }
            
            Float mleft = context->value(e->margin.left, inSize.width, 0);
            Float mright = context->value(e->margin.right, inSize.width, 0);
            Float mtop = context->value(e->margin.top, inSize.height, 0);
            Float mbottom = context->value(e->margin.bottom, inSize.height, 0);
            
            Float width = context->value(e->width, inSize.width - mleft - mright, Unit::Auto);
            Float height = context->value(e->height, inSize.height - mtop - mbottom, Unit::Auto);
            
            Rect & v = e->frame;
            
            v.size.width = width;
            v.size.height = height;
            
            context->layout(e);
            
            if(width == Unit::Auto) {
                width = v.size.width = e->contentSize.width;
                Float min = context->value(e->minWidth, inSize.width, 0);
                Float max = context->value(e->maxWidth, inSize.width, Unit::Auto);
                if(v.size.width < min) {
                    width = v.size.width = min;
                }
                if(v.size.width > max) {
                    width = v.size.width = max;
                }
            }
            
            if(height == Unit::Auto) {
                height = v.size.height = e->contentSize.height;
                Float min = context->value(e->minHeight, inSize.height, 0);
                Float max = context->value(e->maxHeight, inSize.height, Unit::Auto);
                if(v.size.height < min) {
                    height = v.size.height = min;
                }
                if(v.size.height > max) {
                    height = v.size.height = max;
                }
            }
            
            if(!nowarp && x + mleft + mright + paddingRight + v.size.width > size.width) {
                if(lineElements.size() > 0) {
                    _LayoutElementLine(context,lineElements,inSize,lineHeight);
                    lineElements.clear();
                }
                y += lineHeight;
                lineHeight = 0;
                x = paddingLeft;
            }
            
            Float left = x + mleft;
            Float top = y + mtop;
            
            x += width + mleft + mright;
            
            if(lineHeight < height + mtop + mbottom) {
                lineHeight = height + mtop + mbottom;
            }
            
            v.origin.x = left;
            v.origin.y = top;
            
            if(left + paddingRight + mright > maxWidth) {
                maxWidth = left + paddingRight + mright;
            }
            
            lineElements.push_back(e);
            
        });
        
        if(lineElements.size() > 0) {
            _LayoutElementLine(context,lineElements,inSize,lineHeight);
        }
        
        element->contentSize = {maxWidth,y + lineHeight + paddingBottom};
        
    }
    
    static void LayoutElementFlex(LayoutContext * context,LayoutElement * element) {
        _LayoutElementFlex(context,element,false);
    }
    
    /**
     * 水平布局 "horizontal" 左到右
     */
    static void LayoutElementHorizontal(LayoutContext * context,LayoutElement * element) {
        _LayoutElementFlex(context,element,true);
    }

    
    LayoutContext::LayoutContext():_width(0),_height(0) {
        _units["px"] = { 1.0f, 0.0f};
        _units["%"] = { 0.0f, 0.01f};
        
        setLayout("", new TFunction<void,LayoutContext * ,LayoutElement *>(LayoutElementRelative));
        setLayout("flex", new TFunction<void,LayoutContext * ,LayoutElement *>(LayoutElementFlex));
        setLayout("horizontal", new TFunction<void,LayoutContext * ,LayoutElement *>(LayoutElementHorizontal));
        setLayout("relative", new TFunction<void,LayoutContext * ,LayoutElement *>(LayoutElementRelative));
        
    }
    
    void LayoutContext::setSize(kk::Float width,kk::Float height) {
        _width = width;
        _height = height;
        _units["vw"] = { _width * 0.01f, 0.0f};
        _units["vh"] = { _height * 0.01f, 0.0f};
        _units["rpx"] = { MIN(width,height) / 750.0f, 0.0f};
    }
    
    void LayoutContext::setUnit(kk::CString unit,kk::Float scale,kk::Float rate) {
        
        if(unit == nullptr) {
            return;
        }
        
        _units[unit] = { scale, rate };
    }
    
    void LayoutContext::setLayout(kk::CString name,TFunction<void,LayoutContext*,LayoutElement *> * func) {
        if(name == nullptr ) {
            return;
        }
        if(func == nullptr) {
            auto i = _layouts.find(name);
            if(i != _layouts.end()) {
                _layouts.erase(i);
            }
        } else {
            _layouts[name] = func;
        }
    }
    
    void LayoutContext::setLayout(kk::CString name,JSObject * func) {
        if(name == nullptr) {
            return;
        }
        if(func == nullptr) {
            auto i = _layouts.find(name);
            if(i != _layouts.end()) {
                _layouts.erase(i);
            }
        } else {
            _layouts[name] = func;
        }
    }
    
    void LayoutContext::layout(LayoutElement * element) {
        
        if(element == nullptr) {
            return ;
        }
        
        {
            Layout * v = dynamic_cast<Layout *>(element);
            if(v != nullptr) {
                if(v->layout(this)) {
                    return;
                }
            }
        }
        
        kk::CString v = element->get("layout");
        
        if(v == nullptr) {
            v = "";
        }
        
        auto i = _layouts.find(v);
        if(i != _layouts.end()) {
            {
                TFunction<void,LayoutContext * ,LayoutElement *> * func = i->second;
                if(func != nullptr) {
                    (*func)(this,element);
                    return;
                }
            }
            {
                JSObject * func = i->second;
                if(func != nullptr) {
                    func->invoke<void,LayoutContext *,LayoutElement *>(nullptr, this,element);
                    return;
                }
            }
        }
    }
    
    kk::Float LayoutContext::value(kk::Float value, kk::Float base,kk::Float defaultValue, kk::CString unit){
        
        if(unit == nullptr) {
            return value;
        }
        
        if(kk::CStringEqual(unit, "auto")) {
            return defaultValue;
        }
        
        auto i = _units.find(unit);
        
        if(i == _units.end()) {
            return value;
        }
        
        auto & u = i->second;
        
        Float v = value * u.scale;
        Float b = value * u.rate * base;
        
        if(v == NAN) {
            v = 0;
        }
        
        if(b == NAN) {
            b = 0;
        }
        
        return v + b;
    }

    kk::Float LayoutContext::value(Pixel & v,kk::Float base,kk::Float defaultValue) {
        return value(v.value, base, defaultValue, v.name.c_str());
    }
    
    LayoutElement::LayoutElement(Document * document,CString name, ElementKey elementId):StyleElement(document,name,elementId),verticalAlign(VerticalAlignTop),hidden(false) {
        
    }
    
    void LayoutElement::changedKey(CString key) {
        StyleElement::changedKey(key);
        if(kk::CStringEqual(key, "left")) {
            left.set(get(key));
        } else if(kk::CStringEqual(key, "top")) {
            top.set(get(key));
        } else if(kk::CStringEqual(key, "right")) {
            right.set(get(key));
        } else if(kk::CStringEqual(key, "bottom")) {
            bottom.set(get(key));
        } else if(kk::CStringEqual(key, "margin")) {
            margin.set(get(key));
        } else if(kk::CStringEqual(key, "padding")) {
            padding.set(get(key));
        } else if(kk::CStringEqual(key, "vertical-align")) {
            verticalAlign = VerticalAlignFromCString(get(key));
        } else if(kk::CStringEqual(key, "width")) {
            width.set(get(key));
        } else if(kk::CStringEqual(key, "min-width")) {
            minWidth.set(get(key));
        } else if(kk::CStringEqual(key, "max-width")) {
            maxWidth.set(get(key));
        } else if(kk::CStringEqual(key, "height")) {
            height.set(get(key));
        } else if(kk::CStringEqual(key, "min-height")) {
            minHeight.set(get(key));
        } else if(kk::CStringEqual(key, "max-height")) {
            maxHeight.set(get(key));
        } else if(kk::CStringEqual(key, "hidden")) {
            hidden = kk::CStringEqual(get(key), "true");
        }
    }
    
    void LayoutElement::onLayout(LayoutContext * context) {
        
    }
    
    void LayoutElement::setFrame(Float x,Float y,Float width,Float height) {
        frame.origin.x = x;
        frame.origin.y = y;
        frame.size.width = width;
        frame.size.height = height;
    }
    
    void LayoutElement::setContentSize(Float width,Float height) {
        contentSize.width = width;
        contentSize.height = height;
    }

    static Element * LayoutElementCreate(Document * document,kk::CString name,ElementKey elementId) {
        return new LayoutElement(document,name,elementId);
    }
    
    void LayoutElement::Openlib() {
     
        Document::library("layout",LayoutElementCreate);
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushInterface<LayoutElement>(ctx, [](duk_context * ctx)->void{
                kk::PutMethod<LayoutElement,void,Float,Float,Float,Float>(ctx, -1, "setFrame", &LayoutElement::setFrame);
                kk::PutMethod<LayoutElement,void,Float,Float>(ctx, -1, "setContentSize", &LayoutElement::setContentSize);
            });
            
            kk::PushClass<LayoutContext>(ctx, [](duk_context * ctx)->void{
                
                kk::PutMethod<LayoutContext,void,Float,Float>(ctx, -1, "setSize", &LayoutContext::setSize);
                kk::PutMethod<LayoutContext,void,kk::CString,Float,Float>(ctx, -1, "setUnit", &LayoutContext::setUnit);
                kk::PutMethod<LayoutContext,void,kk::CString,kk::JSObject *>(ctx, -1, "setLayout", &LayoutContext::setLayout);
                kk::PutMethod<LayoutContext,void,LayoutElement *>(ctx, -1, "layout", &LayoutContext::layout);
                
            });
            
        });
        
        
    }
    
}

