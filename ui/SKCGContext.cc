//
//  SKCGContext.cc
//  Ker
//
//  Created by zhanghailong on 2019/1/17.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include <ui/CGContext.h>
#include <SkCanvas.h>
#include <SkPath.h>
#include <SkPaint.h>
#include <SkBitmap.h>

#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif

#define DEGREES(a) ((a) * 180 / M_PI)

namespace kk {


    namespace ui {


        namespace CG {

            
            class SKCGContext : public Context {

            public:

                SKCGContext(kk::Uint width,kk::Uint height)
                    :Context(),_width(width),_height(height),
                        _lineWidth(0),_textAlign(TextAlignLeft),
                        _shadowBlur(0),_shadowOffsetX(0),_shadowOffsetY(0),
                        _baseline(TextBaselineAlphabetic),_lineCap(LineCapTypeButt),_lineJoin(LineJoinTypeMiter),_miterLimit(0),
                        _globalAlpha(1),_globalCompositeOperation(GlobalCompositeOperationSourceOver)
                {

                    _info = SkImageInfo::Make(_width,_height,kBGRA_8888_SkColorType,kUnpremul_SkAlphaType);
                    _data = malloc(_info.minRowBytes() * _info.height());
                    _skCanvas = SkCanvas::MakeRasterDirect(_info,_data, _info.minRowBytes(), nullptr);
                    _skCanvas->clear(0);

                }

                virtual ~SKCGContext() {
                    free(_data);
                }

                virtual kk::Strong<LinearGradient> createLinearGradient(Float x0,Float y0, Float x1, Float y1) {
                    return nullptr;
                }

                virtual kk::Strong<RadialGradient> createRadialGradient(Float x0,Float y0,Float r0, Float x1, Float y1, Float r1) {
                    return nullptr;
                }

                virtual kk::Strong<Pattern> createPattern(Image * image,PatternType type) {
                    return nullptr;
                }

                virtual kk::Strong<ImageData> createImageData(Uint width,Uint height) {

                    if(width == 0 || height == 0) {
                        return nullptr;
                    }

                    return new ImageData(width,height);
                }

                virtual kk::Strong<ImageData> getImageData(Uint x,Uint y,Uint width,Uint height) {

                    if(width == 0 || height == 0) {
                        return nullptr;
                    }

                    kk::Strong<ImageData> image = createImageData(width, height);

                    if(image != nullptr) {
                        SkImageInfo info = SkImageInfo::Make(width,height,_info.colorType(),_info.alphaType());
                        _skCanvas->readPixels(_info,image->data(),info.minRowBytes(),x,y);
                    }

                    return image;
                }

                virtual void putImageData(ImageData * image,Uint x,Uint y,Uint dirtyX,Uint dirtyY,Uint dirtyWidth,Uint dirtyHeight) {

                    if(image == nullptr) {
                        return;
                    }

                    if(dirtyWidth == 0) {
                        dirtyWidth = _width - dirtyX;
                    }

                    if(dirtyHeight == 0) {
                        dirtyHeight = _height - dirtyY;
                    }

                    Uint mWidth = image->width();
                    Uint width = MIN(mWidth - x,dirtyWidth);
                    Uint height = MIN(image->height() - y,dirtyHeight);

                    SkImageInfo info = SkImageInfo::Make(width,height,_info.colorType(),_info.alphaType());

                    _skCanvas->writePixels(info,image->data(),info.minRowBytes(),x,y);

                }


                virtual void save() {
                    _skCanvas->save();
                }

                virtual void restore() {
                    _skCanvas->restore();
                }

                virtual void rect(Float x, Float y,Float width,Float height) {
                    _skPath.addRect(x,y,width,height,SkPath::Direction::kCW_Direction);
                }

                virtual void fillRect(Float x, Float y,Float width,Float height) {
                    rect(x, y, width, height);
                    fill();
                }

                virtual void strokeRect(Float x, Float y,Float width,Float height) {
                    rect(x, y, width, height);
                    stroke();
                }

                virtual void clearRect(Float x, Float y,Float width,Float height) {
                    _skCanvas->save();
                    SkRect r = SkRect::MakeLTRB(x,y,width,height);
                    _skCanvas->clipRect(r,SkClipOp::kDifference);
                    _skCanvas->clear(0);
                    _skCanvas->restore();
                }

                virtual void clear() {
                    _skCanvas->clear(0);
                }

                virtual void fill() {
                    SkColor v = _fillColor.intValue();
                    U8CPU a = 0x0ff & (v >> 24);
                    _skPaint.setColor(v);
                    _skPaint.setAlpha((U8CPU) (a * _globalAlpha));
                    _skPaint.setStyle(SkPaint::Style::kFill_Style);
                    _skCanvas->drawPath(_skPath,_skPaint);
                }

                virtual void stroke() {
                    SkColor v = _strokeColor.intValue();
                    U8CPU a = 0x0ff & (v >> 24);
                    _skPaint.setColor(v);
                    _skPaint.setAlpha((U8CPU) (a * _globalAlpha));
                    _skPaint.setStyle(SkPaint::Style::kStroke_Style);
                    _skCanvas->drawPath(_skPath,_skPaint);

                }

                virtual void beginPath() {
                    _skPath.reset();
                }

                virtual void closePath() {
                    _skPath.close();
                }

                virtual void moveTo(Float x,Float y) {
                    _skPath.moveTo(x,y);
                }

                virtual void lineTo(Float x,Float y) {
                    _skPath.lineTo(x,y);
                }

                virtual void clip() {
                    _skCanvas->clipPath(_skPath,SkClipOp::kDifference);
                }

                virtual void quadraticCurveTo(Float cpx,Float cpy,Float x,Float y) {
                    _skPath.quadTo(cpx,cpy,x,y);
                }

                virtual void bezierCurveTo(Float cp1x,Float cp1y,Float cp2x,Float cp2y,Float x,Float y) {
                    _skPath.cubicTo(cp1x,cp1y,cp2x,cp2y,x,y);
                }

                virtual void arc(Float x,Float y,Float r, Float sAngle,Float eAngle,Boolean counterclockwise) {
                    _skPath.addRect(x - r,y - r,x + r,y + r, SkPath::Direction::kCW_Direction);
                }

                virtual void arcTo(Float x1,Float y1,Float x2,Float y2,Float r) {

                }

                virtual Boolean isPointInPath(Float x,Float y) {

                    return false;
                }

                virtual void scale(Float sx,Float sy) {
                    _skCanvas->scale(sx,sy);
                }

                virtual void rotate(Float angle) {
                    _skCanvas->rotate(DEGREES(angle));
                }

                virtual void translate(Float dx,Float dy) {
                    _skCanvas->translate(dx,dy);
                }

                virtual void transform(Float a,Float b,Float c,Float d,Float e,Float f) {
                    SkMatrix m ;
                    m.set(0,a);
                    m.set(1,b);
                    m.set(2,c);
                    m.set(3,d);
                    m.set(4,e);
                    m.set(5,f);
                    SkMatrix v = _skCanvas->getTotalMatrix();
                    v.postConcat(m);
                    _skCanvas->setMatrix(v);
                }

                virtual void setTransform(Float a,Float b,Float c,Float d,Float e,Float f) {
                    SkMatrix m ;
                    m.set(0,a);
                    m.set(1,b);
                    m.set(2,c);
                    m.set(3,d);
                    m.set(4,e);
                    m.set(5,f);
                    _skCanvas->setMatrix(m);
                }


                virtual void fillText(kk::CString text,Float x,Float y,Float maxWidth) {

                    if(text == nullptr) {
                        return ;
                    }



                }

                virtual void strokeText(kk::CString text,Float x,Float y,Float maxWidth) {

                    if(text == nullptr) {
                        return ;
                    }



                }

                virtual Float measureText(kk::CString text) {

                    if(text == nullptr) {
                        return 0.0f;
                    }


                    return 0.0f;
                }

                virtual void drawImage(Image * image,Float sx,Float sy,Float swidth,Float sheight,Float x,Float y,Float width,Float height) {

                    if(image == nullptr) {
                        return;
                    }



                }

                virtual void setFillStyle(Style * v) {

                }

                virtual Style * fillStyle() {
                    return _fillStyle;
                }

                virtual void setFillColor(Color v) {
                    _fillColor = v;
                }

                virtual Color fillColor() {
                    return _fillColor;
                }

                virtual void setStrokeStyle(Style * v) {
                    _strokeStyle = v;
                }

                virtual Style * strokeStyle() {
                    return _strokeStyle;
                }

                virtual void setStrokeColor(Color v) {
                    _strokeColor = v;
                    _strokeStyle = nullptr;
                }

                virtual Color strokeColor() {
                    return _strokeColor;
                }

                virtual void setShadowColor(Color v) {

                }

                virtual Color shadowColor() {
                    return _shadowColor;
                }

                virtual void setShadowBlur(Float v) {

                }

                virtual Float shadowBlur() {
                    return _shadowBlur;
                }

                virtual void setShadowOffsetX(Float v) {

                }

                virtual Float shadowOffsetX() {
                    return _shadowOffsetX;
                }

                virtual void setShadowOffsetY(Float v) {

                }

                virtual Float shadowOffsetY() {
                    return _shadowOffsetY;
                }

                virtual void setLineCap(LineCapType v) {
                    _lineCap = v;
                }

                virtual LineCapType lineCap() {
                    return _lineCap;
                }

                virtual void setLineJoin(LineJoinType v) {
                    _lineJoin = v;
                }

                virtual LineJoinType lineJoin() {
                    return _lineJoin;
                }

                virtual void setLineWidth(Float v) {
                    _lineWidth = v;
                    _skPaint.setStrokeWidth(v);
                }

                virtual Float lineWidth() {
                    return _lineWidth;
                }

                virtual void setMiterLimit(Float v) {
                    _miterLimit = v;
                    _skPaint.setStrokeMiter(v);
                }

                virtual Float miterLimit() {
                    return _miterLimit;
                }

                virtual void setFont(Font v) {
                    _font = v;
                }

                virtual Font font() {
                    return _font;
                }

                virtual void setTextAlign(TextAlign v) {
                    _textAlign = v;
                }

                virtual TextAlign textAlign() {
                    return _textAlign;
                }

                virtual void setTextBaseline(TextBaseline v) {
                    _baseline = v;
                }

                virtual TextBaseline textBaseline() {
                    return _baseline;
                }

                virtual void setGlobalAlpha(Float v) {
                    _globalAlpha = v;
                }

                virtual Float globalAlpha() {
                    return _globalAlpha;
                }

                virtual void setGlobalCompositeOperation(GlobalCompositeOperation v) {
                    _globalCompositeOperation = v;
                }

                virtual GlobalCompositeOperation globalCompositeOperation() {
                    return _globalCompositeOperation;
                }

                virtual kk::Uint width() {
                    return _width;
                }

                virtual kk::Uint height() {
                    return _height;
                }

                virtual void copyPixels(void * dest) {
                    _skCanvas->readPixels(_info,dest,_info.minRowBytes(),0,0);
                }

            protected:
                void * _data;
                SkImageInfo _info;
                std::unique_ptr<SkCanvas> _skCanvas;
                SkPath _skPath;
                SkPaint _skPaint;
                Strong<Style> _fillStyle;
                Strong<Style> _strokeStyle;
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
                LineCapType _lineCap;
                LineJoinType _lineJoin;
                Float _miterLimit;
                Float _globalAlpha;
                GlobalCompositeOperation _globalCompositeOperation;
            };
            
            kk::Strong<Context> Context::create(kk::Uint width,kk::Uint height) {
                return new SKCGContext(width,height);
            }


        }

    }

}
