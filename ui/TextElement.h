//
//  TextElement.h
//  Ker
//
//  Created by zhanghailong on 2019/1/16.
//  Copyright © 2019年 kkmofang.cn. All rights reserved.
//

#ifndef document_TextElement_h
#define document_TextElement_h

#include <ui/ViewElement.h>

namespace kk {
    
    
    namespace ui {

        struct TextFont {
            kk::String family;
            kk::Pixel size;
            FontStyle style;
            FontWeight weight;
            TextFont();
            TextFont(const TextFont &v);
            virtual void set(kk::CString v);
            virtual void set(const TextFont &v);
            virtual Font font(LayoutContext * context);
        };
        
        class SpanElement : public kk::StyleElement {
        public:
            SpanElement(Document * document,CString name, ElementKey elementId);
            virtual void changedKey(CString key);
            TextFont font;
            Color color;
        };
        
        class ImgElement : public kk::StyleElement {
        public:
            ImgElement(Document * document,CString name, ElementKey elementId);
            virtual void changedKey(CString key);
            virtual Image * getImage(ViewContext * context);
            virtual kk::Uint getWidth(ViewContext * context);
            virtual kk::Uint getHeight(ViewContext * context);
            virtual Edge getMargin(ViewContext * context);
            kk::Strong<Image> image;
            kk::Pixel width;
            kk::Pixel height;
            kk::Edge margin;
        };
        
        class TextElement : public ViewElement, public kk::Layout {
        public:
            TextElement(Document * document,CString name, ElementKey elementId);
            
            virtual kk::Boolean layout(LayoutContext * context);
            virtual void changedKey(CString key);
            virtual AttributedText * text(ViewContext * context);
            
            Ker_CLASS(TextElement,ViewElement,"UITextElement")
            
            TextFont font;
            Color color;
            
            static void Openlib();
            static void library(kk::CString name);
        protected:
            virtual void onDidAddChildren(Element * element);
            virtual void onWillRemoveChildren(Element * element);
            virtual void onObtainView(ViewContext * context,View * view);
            
            kk::Strong<AttributedText> _text;
        };
        
    }
    
}

#endif /* TextElement_h */
