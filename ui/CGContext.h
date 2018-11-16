//
//  CGContext.h
//  kk
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef ui_CGContext_h
#define ui_CGContext_h


#include <ui/ui.h>
#include <core/event.h>

namespace kk {
    
    namespace ui {
        
        namespace CG {
            
            class Style : public kk::Object {
            };
            
            enum PatternType {
                PatternTypeRepeat,
                PatternTypeRepeatX,
                PatternTypeRepeatY,
                PatternTypeNoRepeat
            };
           
            PatternType PatternTypeFromString(kk::CString string);
            kk::CString StringFromPatternType(PatternType v);
            
            class Pattern : public Style {
            public:
                Pattern(Image * image,PatternType type);
                virtual Image * image();
                virtual PatternType type();
            protected:
                kk::Strong<Image> _image;
                PatternType _type;
            };
            
            struct ColorStop {
                Float rate;
                Color color;
            };
            
            class Gradient: public Style  {
            public:
                virtual void addColorStop(Float loc,Color color) ;
            protected:
                std::vector<Float> _locations;
                std::vector<Color> _colors;
            };
            
            class RadialGradient: public Gradient {
            public:
            };
            
            class LinearGradient: public Gradient {
            public:
            };
            
            enum LineCapType {
                LineCapTypeButt,
                LineCapTypeRound,
                LineCapTypeSquare,
            };
            
            LineCapType LineCapTypeFromString(kk::CString string);
            kk::CString StringFromLineCapType(LineCapType v);
            
            enum LineJoinType {
                LineJoinTypeMiter,
                LineJoinTypeRound,
                LineJoinTypeBevel,
            };
            
            LineJoinType LineJoinTypeFromString(kk::CString string);
            kk::CString StringFromLineJoinType(LineJoinType v);
            
            
            enum GlobalCompositeOperation {
                GlobalCompositeOperationSourceOver,
                GlobalCompositeOperationSourceAtop,
                GlobalCompositeOperationSourceIn,
                GlobalCompositeOperationSourceOut,
                GlobalCompositeOperationDestinationOver,
                GlobalCompositeOperationDestinationAtop,
                GlobalCompositeOperationDestinationIn,
                GlobalCompositeOperationDestinationOut,
                GlobalCompositeOperationDestinationLighter,
                GlobalCompositeOperationDestinationCopy,
                GlobalCompositeOperationDestinationXOR,
            };
            
            GlobalCompositeOperation GlobalCompositeOperationFromString(kk::CString string);
            kk::CString StringFromGlobalCompositeOperation(GlobalCompositeOperation v);
            
            class ImageData : public kk::Object, public Image {
            public:
                ImageData(Uint width,Uint height);
                virtual ~ImageData();
                virtual Uint width();
                virtual Uint height();
                virtual Ubyte * data();
                virtual Uint size();
                virtual Boolean isCopyPixels();
                virtual void copyPixels(void * data);
                virtual ImageState state();
                virtual kk::CString src();
                virtual void setSrc(kk::CString src);
            protected:
                Ubyte * _data;
                Uint _width;
                Uint _height;
            };
            
            class Context : public Object {
            public:
                
                virtual kk::Strong<LinearGradient> createLinearGradient(Float x0,Float y0, Float x1, Float y1) = 0;
                
                virtual kk::Strong<RadialGradient> createRadialGradient(Float x0,Float y0,Float r0, Float x1, Float y1, Float r1) = 0;
                
                virtual kk::Strong<Pattern> createPattern(Image * image,PatternType type) = 0;
                
                virtual kk::Strong<Pattern> createPattern(Image * image,kk::CString type);
                
                virtual kk::Strong<ImageData> createImageData(Uint width,Uint height) = 0;
                
                virtual kk::Strong<ImageData> getImageData(Uint x,Uint y,Uint width,Uint height) = 0;
                
                virtual void putImageData(ImageData * data,Uint x,Uint y,Uint dirtyX,Uint dirtyY,Uint dirtyWidth,Uint dirtyHeight) = 0;
                
                virtual void save() = 0;
                
                virtual void restore() = 0;
                
                virtual void rect(Float x, Float y,Float width,Float height) = 0;
                
                virtual void fillRect(Float x, Float y,Float width,Float height) = 0;
                
                virtual void strokeRect(Float x, Float y,Float width,Float height) = 0;
                
                virtual void clearRect(Float x, Float y,Float width,Float height) = 0;
                
                virtual void fill() = 0;
                
                virtual void stroke() = 0;
                
                virtual void beginPath() = 0;
                
                virtual void moveTo(Float x,Float y) = 0;
                
                virtual void closePath() = 0;
                
                virtual void lineTo(Float x,Float y) = 0;
                
                virtual void clip() = 0;
                
                virtual void quadraticCurveTo(Float cpx,Float cpy,Float x,Float y) = 0;
                
                virtual void bezierCurveTo(Float cp1x,Float cp1y,Float cp2x,Float cp2y,Float x,Float y) = 0;
                
                virtual void arc(Float x,Float y,Float r, Float sAngle,Float eAngle,Boolean counterclockwise) = 0;
                
                virtual void arcTo(Float x1,Float y1,Float x2,Float y2,Float r) = 0;
                
                virtual Boolean isPointInPath(Float x,Float y) = 0;
                
                virtual void scale(Float sx,Float sy) = 0;
                
                virtual void rotate(Float angle) = 0;
                
                virtual void translate(Float dx,Float dy) = 0;
                
                virtual void transform(Float a,Float b,Float c,Float d,Float e,Float f) = 0;
                
                virtual void setTransform(Float a,Float b,Float c,Float d,Float e,Float f) = 0;
                
                virtual void fillText(kk::CString text,Float x,Float y,Float maxWidth) = 0;
                
                virtual void strokeText(kk::CString text,Float x,Float y,Float maxWidth) = 0;
                
                virtual Float measureText(kk::CString text) = 0;
                
                virtual void drawImage(Image * image,Float sx,Float sy,Float swidth,Float sheight,Float x,Float y,Float width,Float height) = 0;
                
                
                virtual void setFillStyleAny(Any v);
                virtual Any fillStyleAny();
                
                virtual void setFillStyle(Style * style) = 0;
                virtual Style * fillStyle() = 0;
                
                virtual void setFillColor(Color color) = 0;
                virtual Color fillColor() = 0;
                
                virtual void setStrokeStyleAny(Any v);
                virtual Any strokeStyleAny();
                
                virtual void setStrokeStyle(Style * style) = 0;
                virtual Style * strokeStyle() = 0;
                
                virtual void setStrokeColor(Color color) = 0;
                virtual Color strokeColor() = 0;
                
                virtual void setShadowColor(Color v) = 0;
                virtual Color shadowColor() = 0;
                
                virtual void setShadowBlur(Float v) = 0;
                virtual Float shadowBlur() = 0;
                
                virtual void setShadowOffsetX(Float v) = 0;
                virtual Float shadowOffsetX() = 0;
                
                virtual void setShadowOffsetY(Float v) = 0;
                virtual Float shadowOffsetY() = 0;
                
                virtual void setLineCap(LineCapType v) = 0;
                virtual LineCapType lineCap() = 0;
                
                virtual void setLineCapString(kk::CString v);
                virtual kk::CString lineCapString();
                
                virtual void setLineJoin(LineJoinType v) = 0;
                virtual LineJoinType lineJoin() = 0;
                
                virtual void setLineJoinString(kk::CString v);
                virtual kk::CString lineJoinString();
                
                virtual void setLineWidth(Float v) = 0;
                virtual Float lineWidth() = 0;
                
                virtual void setMiterLimit(Float v) = 0;
                virtual Float miterLimit() = 0;
                
                virtual void setFont(Font v) = 0;
                virtual Font font() = 0;
                
                virtual void setTextAlign(TextAlign v) = 0;
                virtual TextAlign textAlign() = 0;
                
                virtual void setTextAlignString(kk::CString v);
                virtual kk::CString textAlignString();
                
                virtual void setTextBaseline(TextBaseline v) = 0;
                virtual TextBaseline textBaseline() = 0;
                
                virtual void setTextBaselineString(kk::CString v);
                virtual kk::CString textBaselineString();
                
                virtual void setGlobalAlpha(Float v) = 0;
                virtual Float globalAlpha() = 0;
                
                virtual void setGlobalCompositeOperation(GlobalCompositeOperation v) = 0;
                virtual GlobalCompositeOperation globalCompositeOperation() = 0;
                
                virtual void setGlobalCompositeOperationString(kk::CString v);
                virtual kk::CString globalCompositeOperationString();
                
                Ker_CLASS(Context, Object, "UICGContext");
                
                static void Openlib();
                
            };
        }
        
    }
    
    
}

#endif /* CGContext_h */
