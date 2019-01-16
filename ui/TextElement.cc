//
//  TextElement.cc
//  Ker
//
//  Created by zhanghailong on 2019/1/16.
//  Copyright © 2019年 kkmofang.cn. All rights reserved.
//

#include "TextElement.h"

namespace kk {

    namespace ui {
        
        TextFont::TextFont():size("28rpx"),style(FontStyleNormal),weight(FontWeightNormal) {
            
        }
        
        TextFont::TextFont(const TextFont &v):TextFont() {
            set(v);
        }
        
        void TextFont::set(kk::CString v) {
            family.clear();
            weight = FontWeightNormal;
            style = FontStyleNormal;
            size.set("28rpx");
            std::vector<kk::String> items;
            kk::CStringSplit(v, " ", items);
            auto i = items.begin();
            while(i != items.end()) {
                kk::String & v = * i;
                if(Pixel::is(v.c_str())) {
                    size.set(v.c_str());
                } else if(v == "bold") {
                    weight = FontWeightBold;
                } else if(v == "italic") {
                    style = FontStyleItalic;
                } else {
                    family = v;
                }
                i ++;
            }
        }
        
        void TextFont::set(const TextFont &v) {
            family = v.family;
            size = v.size;
            style = v.style;
            weight = v.weight;
        }
        
        Font TextFont::font(LayoutContext * context) {
            Font v;
            v.family = family;
            v.weight = weight;
            v.style = style;
            v.size = context->value(size, 0, 0);
            return v;
        }
        
        SpanElement::SpanElement(Document * document,CString name, ElementKey elementId):kk::StyleElement(document,name,elementId)  {
            
        }
        
        void SpanElement::changedKey(CString key) {
            StyleElement::changedKey(key);
            if(kk::CStringEqual(key, "font")) {
                font.set(get(key));
            } else if(kk::CStringEqual(key, "color")) {
                color = Color(get(key));
            }
        }
        
        ImgElement::ImgElement(Document * document,CString name, ElementKey elementId):kk::StyleElement(document,name,elementId) {
            
        }
        
        ImgElement::~ImgElement() {
            if(_onLoad != nullptr && image != nullptr) {
                image->off("load", (EventFunction *) _onLoad);
            }
            _onLoad = nullptr;
        }
        
        void ImgElement::changedKey(CString key) {
            StyleElement::changedKey(key);
            if(kk::CStringEqual(key, "src")) {
                image = nullptr;
            } else if(kk::CStringEqual(key, "margin")) {
                margin.set(get(key));
            } else if(kk::CStringEqual(key, "width")) {
                width.set(get(key));
            } else if(kk::CStringEqual(key, "height")) {
                height.set(get(key));
            }
        }
        
        void ImgElement::setImage(Image * v){
            if(_onLoad != nullptr && image != nullptr) {
                image->off("load", (EventFunction *) _onLoad);
            }
            image = v;
            if(image != nullptr) {
                if(_onLoad == nullptr) {
                    _onLoad = new EventFunction([this](CString name,Event * event)->void{
                        Strong<ElementEvent> e = new ElementEvent(this);
                        this->emit("layout", e);
                    });
                }
                image->on("load", (EventFunction *) _onLoad);
            }
        }
        
        Image * ImgElement::getImage(ViewContext * context) {
            if(image == nullptr) {
                kk::CString v = get("src");
                if(v != nullptr) {
                    App * app = context->app();
                    if(app != nullptr) {
                        Strong<Image> i = app->createImage(v);
                        setImage(i);
                    }
                }
            }
            return image;
        }
        
        kk::Uint ImgElement::getWidth(ViewContext * context) {
            return (kk::Uint) context->value(width, 0, 0);
        }
        
        kk::Uint ImgElement::getHeight(ViewContext * context) {
            return (kk::Uint) context->value(height, 0, 0);
        }
        
        Edge ImgElement::getMargin(ViewContext * context) {
            Edge v;
            v.left = context->value(margin.left, 0, 0);
            v.right = context->value(margin.right, 0, 0);
            v.top = context->value(margin.top, 0, 0);
            v.bottom = context->value(margin.bottom, 0, 0);
            return v;
        }

        TextElement::TextElement(Document * document,CString name, ElementKey elementId):ViewElement(document,name,elementId) {
            
        }
        
        void TextElement::changedKey(CString key) {
            if(kk::CStringEqual(key, "#text") || kk::CStringEqual(key, "value")) {
                _text = nullptr;
                return;
            } else if(kk::CStringEqual(key, "font")) {
                font.set(get(key));
                return;
            } else if(kk::CStringEqual(key, "color")) {
                color = Color(get(key));
                return;
            }
            ViewElement::changedKey(key);
        }
        
        kk::Boolean TextElement::layout(LayoutContext * context) {
            
            contentSize.width = context->value(width, 0, 0);
            contentSize.height = context->value(height, 0, 0);
            
            if(width.isAuto() || height.isAuto()) {
                
                ViewContext * v = dynamic_cast<ViewContext *>(context);
                
                if(v != nullptr) {
                    
                    App * app = v->app();
                    
                    if(app != nullptr) {
                        
                        kk::Float m = context->value(width, 0, Unit::Auto);
                        
                        if(!maxWidth.isAuto()) {
                            m = context->value(maxWidth, 0, Unit::Auto);
                        }
                        
                        kk::ui::Size size = getAttributedTextContentSize(app, text(v), m);
                        
                        if(width.isAuto()) {
                            contentSize.width = size.width;
                        }
                        
                        if(height.isAuto()) {
                            contentSize.height = size.height;
                        }
                        
                    }

                }
                
            }
            return true;
        }
        
        void TextElement::onDidAddChildren(Element * element) {
            ViewElement::onDidAddChildren(element);
            _text = nullptr;
        }
        
        void TextElement::onWillRemoveChildren(Element * element) {
            ViewElement::onWillRemoveChildren(element);
            _text = nullptr;
        }
        
        AttributedText * TextElement::text(ViewContext * context) {
            if(_text == nullptr) {
                _text = new AttributedText();
                Element * p = firstChild();
                if(p == nullptr) {
                    kk::CString v = get("#text");
                    if(v == nullptr) {
                        v = get("value");
                    }
                    if(v != nullptr) {
                        _text->append(v, font.font(context), color);
                    }
                } else{
                    
                    while(p) {
                        
                        {
                            SpanElement * e = dynamic_cast<SpanElement *>(p);
                            
                            if(e != nullptr) {
                                
                                kk::CString v = p->get("#text");
                                
                                if(v != nullptr) {
                                    TextFont & f = p->get("font") == nullptr ? font : e->font;
                                    Color & c = p->get("color") == nullptr ? color : e->color;
                                    _text->append(v, f.font(context), c);
                                }
                                
                                p = p->nextSibling();
                                continue;
                            }
                        }
                        
                        {
                            ImgElement * e = dynamic_cast<ImgElement *>(p);
                            
                            if(e != nullptr) {
                                
                                Image * image = e->getImage(context);
                                
                                if(image != nullptr) {
                                    _text->append(image, e->getWidth(context), e->getHeight(context), e->getMargin(context));
                                }
                               
                                p = p->nextSibling();
                                continue;
                            }
                        }
                        
                        
                        p = p->nextSibling();
                    }
                    
                }
            }
            return _text;
        }
        
        void TextElement::onObtainView(ViewContext * context,View * view) {
            ViewElement::onObtainView(context, view);
            view->setAttributedText(text(context));
        }
        
        void TextElement::Openlib() {
            
            Document::library("span", [](Document * document,kk::CString name,ElementKey elementId)->Element *{
                return new SpanElement(document,name,elementId);
            });
            
            Document::library("img", [](Document * document,kk::CString name,ElementKey elementId)->Element *{
                return new ImgElement(document,name,elementId);
            });
            
            library("text");
            
            kk::Openlib<>::add([](duk_context * ctx)->void{
                
                kk::PushInterface<TextElement>(ctx, [](duk_context * ctx)->void{
                    
                    kk::PutMethod<TextElement,AttributedText *,ViewContext *>(ctx, -1, "text", &TextElement::text);
                    
                });
                
            });
            
        }
        
        static Element * TextElementCreate(Document * document,kk::CString name,ElementKey elementId) {
            return new TextElement(document,name,elementId);
        }
        
        void TextElement::library(kk::CString name) {
            Document::library(name, TextElementCreate);
        }
        
    }
}

