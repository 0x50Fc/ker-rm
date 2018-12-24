//
// Created by zhanghailong on 2018/12/18.
//

#ifndef KER_KERCGCONTEXT_H
#define KER_KERCGCONTEXT_H

#include <jni.h>
#include <ui/CGContext.h>

namespace kk {

    namespace ui {

        class OSCGContext : public CG::Context {
        public:
            OSCGContext(JNIEnv * env, jobject object,kk::Uint width,kk::Uint height);
            virtual ~OSCGContext();
            virtual jobject object();

            virtual kk::Strong<CG::LinearGradient> createLinearGradient(Float x0,Float y0, Float x1, Float y1);

            virtual kk::Strong<CG::RadialGradient> createRadialGradient(Float x0,Float y0,Float r0, Float x1, Float y1, Float r1);

            virtual kk::Strong<CG::Pattern> createPattern(Image * image,CG::PatternType type);

            virtual kk::Strong<CG::ImageData> createImageData(Uint width,Uint height);

            virtual kk::Strong<CG::ImageData> getImageData(Uint x,Uint y,Uint width,Uint height);

            virtual void putImageData(CG::ImageData * data,Uint x,Uint y,Uint dirtyX,Uint dirtyY,Uint dirtyWidth,Uint dirtyHeight);

            virtual void save();

            virtual void restore();

            virtual void rect(Float x, Float y,Float width,Float height);

            virtual void fillRect(Float x, Float y,Float width,Float height);

            virtual void strokeRect(Float x, Float y,Float width,Float height);

            virtual void clearRect(Float x, Float y,Float width,Float height);

            virtual void clear();

            virtual void fill();

            virtual void stroke();

            virtual void beginPath();

            virtual void moveTo(Float x,Float y);

            virtual void closePath();

            virtual void lineTo(Float x,Float y);

            virtual void clip();

            virtual void quadraticCurveTo(Float cpx,Float cpy,Float x,Float y);

            virtual void bezierCurveTo(Float cp1x,Float cp1y,Float cp2x,Float cp2y,Float x,Float y);

            virtual void arc(Float x,Float y,Float r, Float sAngle,Float eAngle,Boolean counterclockwise);

            virtual void arcTo(Float x1,Float y1,Float x2,Float y2,Float r);

            virtual Boolean isPointInPath(Float x,Float y);

            virtual void scale(Float sx,Float sy);

            virtual void rotate(Float angle);

            virtual void translate(Float dx,Float dy);

            virtual void transform(Float a,Float b,Float c,Float d,Float e,Float f);

            virtual void setTransform(Float a,Float b,Float c,Float d,Float e,Float f);

            virtual void fillText(kk::CString text,Float x,Float y,Float maxWidth);

            virtual void strokeText(kk::CString text,Float x,Float y,Float maxWidth);

            virtual Float measureText(kk::CString text);

            virtual void drawImage(Image * image,Float sx,Float sy,Float swidth,Float sheight,Float x,Float y,Float width,Float height);

            virtual void setFillStyle(CG::Style * style);
            virtual CG::Style * fillStyle();

            virtual void setFillColor(Color color);
            virtual Color fillColor();

            virtual void setStrokeStyle(CG::Style * style);
            virtual CG::Style * strokeStyle();

            virtual void setStrokeColor(Color color);
            virtual Color strokeColor();

            virtual void setShadowColor(Color v);
            virtual Color shadowColor();

            virtual void setShadowBlur(Float v);
            virtual Float shadowBlur();

            virtual void setShadowOffsetX(Float v);
            virtual Float shadowOffsetX();

            virtual void setShadowOffsetY(Float v);
            virtual Float shadowOffsetY();

            virtual void setLineCap(CG::LineCapType v);
            virtual CG::LineCapType lineCap();

            virtual void setLineJoin(CG::LineJoinType v);
            virtual CG::LineJoinType lineJoin();

            virtual void setLineWidth(Float v);
            virtual Float lineWidth();

            virtual void setMiterLimit(Float v);
            virtual Float miterLimit();

            virtual void setFont(Font v);
            virtual Font font();

            virtual void setTextAlign(TextAlign v);
            virtual TextAlign textAlign();

            virtual void setTextBaseline(TextBaseline v);
            virtual TextBaseline textBaseline();

            virtual void setGlobalAlpha(Float v);
            virtual Float globalAlpha();

            virtual void setGlobalCompositeOperation(CG::GlobalCompositeOperation v);
            virtual CG::GlobalCompositeOperation globalCompositeOperation();

        protected:
            jobject _object;
            Strong<CG::Style> _fillStyle;
            Strong<CG::Style> _strokeStyle;
            Color _fillColor;
            Color _strokeColor;
            Color _shadowColor;
            Font _font;
            Float _lineWidth;
            TextAlign _textAlign;
            Float _shadowBlur;
            Float _shadowOffsetX;
            Float _shadowOffsetY;
            TextBaseline _baseline;
            kk::Uint _width;
            kk::Uint _height;
            CG::LineCapType _lineCap;
            CG::LineJoinType _lineJoin;
            Float _miterLimit;
            Float _globalAlpha;
            CG::GlobalCompositeOperation _globalCompositeOperation;
        };

        kk::Strong<kk::ui::CG::Context> createCGContext(kk::Uint width,kk::Uint height);

        void displayCGContext(kk::ui::CG::Context * context,jobject view);

    }

}



#endif //DEMO_KERCGCONTEXT_H
