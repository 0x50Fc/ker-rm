//
//  CGContext.cc
//  kk
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <ui/CGContext.h>

namespace kk {
    
    namespace ui {
    
        
        namespace CG {

            PatternType PatternTypeFromString(kk::CString string) {
                if(CStringEqual(string, "repeat-x")){
                    return PatternTypeRepeatX;
                }
                if(CStringEqual(string, "repeat-y")){
                    return PatternTypeRepeatY;
                }
                if(CStringEqual(string, "no-repeat")){
                    return PatternTypeNoRepeat;
                }
                return PatternTypeRepeat;
            }
            
            kk::CString StringFromPatternType(PatternType v) {
                switch (v) {
                    case PatternTypeRepeatX:
                        return "repeat-x";
                    case PatternTypeRepeatY:
                        return "repeat-y";
                    case PatternTypeNoRepeat:
                        return "no-repeat";
                    default:
                        return "repeat";
                }
            }
            
            
            LineCapType LineCapTypeFromString(kk::CString string) {
                if(CStringEqual(string, "round")) {
                    return LineCapTypeRound;
                }
                if(CStringEqual(string, "square")) {
                    return LineCapTypeSquare;
                }
                return LineCapTypeButt;
            }
            
            kk::CString StringFromLineCapType(LineCapType v) {
                switch (v) {
                    case LineCapTypeRound:
                        return "round";
                    case LineCapTypeSquare:
                        return "square";
                    default:
                        return "butt";
                }
            }
            
            LineJoinType LineJoinTypeFromString(kk::CString string) {
                
                if(CStringEqual(string, "round")) {
                    return LineJoinTypeRound;
                }
                
                if(CStringEqual(string, "bevel")) {
                    return LineJoinTypeBevel;
                }
                
                return LineJoinTypeMiter;
            }
            
            kk::CString StringFromLineJoinType(LineJoinType v) {
                switch (v) {
                    case LineJoinTypeRound:
                        return "round";
                    case LineJoinTypeBevel:
                        return "bevel";
                    default:
                        return "miter";
                }
            }
            
            
            GlobalCompositeOperation GlobalCompositeOperationFromString(kk::CString string) {
                
                if(CStringEqual(string, "source-atop")) {
                    return GlobalCompositeOperationSourceAtop;
                }
                
                if(CStringEqual(string, "source-in")) {
                    return GlobalCompositeOperationSourceIn;
                }
                
                if(CStringEqual(string, "source-out")) {
                    return GlobalCompositeOperationSourceOut;
                }
                
                if(CStringEqual(string, "destination-over")) {
                    return GlobalCompositeOperationDestinationOver;
                }
                
                if(CStringEqual(string, "destination-atop")) {
                    return GlobalCompositeOperationDestinationAtop;
                }
                
                if(CStringEqual(string, "destination-in")) {
                    return GlobalCompositeOperationDestinationIn;
                }
                
                if(CStringEqual(string, "destination-out")) {
                    return GlobalCompositeOperationDestinationOut;
                }
                
                if(CStringEqual(string, "destination-lighter")) {
                    return GlobalCompositeOperationDestinationLighter;
                }
                
                if(CStringEqual(string, "destination-copy")) {
                    return GlobalCompositeOperationDestinationCopy;
                }
                
                if(CStringEqual(string, "destination-xor")) {
                    return GlobalCompositeOperationDestinationXOR;
                }
                
                return GlobalCompositeOperationSourceOver;
            }
            
            kk::CString StringFromGlobalCompositeOperation(GlobalCompositeOperation v) {
                
                switch (v) {
                    case GlobalCompositeOperationSourceAtop:
                        return "source-atop";
                    case GlobalCompositeOperationSourceIn:
                        return "source-in";
                    case GlobalCompositeOperationSourceOut:
                        return "source-out";
                    case GlobalCompositeOperationDestinationOver:
                        return "destination-over";
                    case GlobalCompositeOperationDestinationAtop:
                        return "destination-atop";
                    case GlobalCompositeOperationDestinationIn:
                        return "destination-in";
                    case GlobalCompositeOperationDestinationOut:
                        return "destination-out";
                    case GlobalCompositeOperationDestinationLighter:
                        return "destination-lighter";
                    case GlobalCompositeOperationDestinationCopy:
                        return "destination-copy";
                    case GlobalCompositeOperationDestinationXOR:
                        return "destination-xor";
                    default:
                        break;
                }
                return "source-over";
            }
            
            Pattern::Pattern(Image * image,PatternType type):_image(image),_type(type) {
                
            }
            
            Image * Pattern::image() {
                return _image;
            }
            
            PatternType Pattern::type() {
                return _type;
            }

            ImageData::ImageData(Uint width,Uint height):_data(nullptr),_width(width),_height(height) {
                size_t n =_width * height * 4;
                _data = (Ubyte *) malloc(n);
                memset(_data, 0, n);
            }
            
            ImageData::~ImageData() {
                free(_data);
            }
            
            void ImageData::copyPixels(void * data) {
                memcpy(data, _data, _width * _height * 4);
            }
            
            Boolean ImageData::isCopyPixels() {
                return true;
            }
            
            Uint ImageData::width() {
                return _width;
            }
            
            Uint ImageData::height() {
                return _height;
            }
            
            Ubyte * ImageData::data() {
                return _data;
            }
            
            Uint ImageData::size() {
                return _width * _height * 4;
            }
            
            ImageState ImageData::state() {
                return ImageStateLoaded;
            }
            
            kk::CString ImageData::src() {
                return nullptr;
            }
            
            void Gradient::addColorStop(Float loc,Color color) {
                _locations.push_back(loc);
                _colors.push_back(color);
            }
            
            void Context::setFillStyleAny(Any v) {
                if(v.type == TypeObject) {
                    setFillStyle((Style *) v);
                } else {
                    setFillStyle((Style *) nullptr);
                    setFillColor(Color((CString)v));
                }
            }
            
            Any Context::fillStyleAny() {
                Style * v = fillStyle();
                if(v == nullptr) {
                    return fillColor();
                }
                return v;
            }
            
            void Context::setStrokeStyleAny(Any v) {
                if(v.type == TypeObject) {
                    setStrokeStyle((Style *) v);
                } else {
                    setStrokeStyle((Style *) nullptr);
                    setStrokeColor(Color((CString)v));
                }
            }
            
            Any Context::strokeStyleAny() {
                Style * v = strokeStyle();
                if(v == nullptr) {
                    return strokeColor();
                }
                return v;
            }
            
            kk::Strong<Pattern> Context::createPattern(Image * image,kk::CString type) {
                return createPattern(image, PatternTypeFromString(type));
            }
            
            void Context::setLineCapString(kk::CString v) {
                setLineCap(LineCapTypeFromString(v));
            }
            
            kk::CString Context::lineCapString() {
                return StringFromLineCapType(lineCap());
            }
            
            void Context::setLineJoinString(kk::CString v) {
                setLineJoin(LineJoinTypeFromString(v));
            }
            
            kk::CString Context::lineJoinString() {
                return StringFromLineJoinType(lineJoin());
            }
            
            void Context::setTextAlignString(kk::CString v) {
                setTextAlign(TextAlignFromString(v));
            }
            
            kk::CString Context::textAlignString() {
                return StringFromTextAlign(textAlign());
            }
            
            void Context::setTextBaselineString(kk::CString v) {
                setTextBaseline(TextBaselineFromString(v));
            }
            
            kk::CString Context::textBaselineString() {
                return StringFromTextBaseline(textBaseline());
            }
            
            void Context::setGlobalCompositeOperationString(kk::CString v) {
                setGlobalCompositeOperation(GlobalCompositeOperationFromString(v));
            }
            
            kk::CString Context::globalCompositeOperationString() {
                return StringFromGlobalCompositeOperation(globalCompositeOperation());
            }
            
            void Context::Openlib() {
                
                kk::Openlib<>::add([](duk_context * ctx)->void{
                    
                    kk::PushInterface<Context>(ctx, [](duk_context * ctx)->void{
                        
                        
                        kk::PutStrongMethod<Context,LinearGradient,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "createLinearGradient", &Context::createLinearGradient);
                        kk::PutStrongMethod<Context,RadialGradient,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "createRadialGradient", &Context::createRadialGradient);
                        kk::PutStrongMethod<Context,Pattern,Image *,kk::CString>(ctx, -1, "createPattern", &Context::createPattern);
                        kk::PutStrongMethod<Context,ImageData,kk::Uint,kk::Uint>(ctx, -1, "createImageData", &Context::createImageData);
                        kk::PutStrongMethod<Context,ImageData,kk::Uint,kk::Uint,kk::Uint,kk::Uint>(ctx, -1, "getImageData", &Context::getImageData);
                        kk::PutMethod<Context,void,ImageData *,kk::Uint,kk::Uint,kk::Uint,kk::Uint,kk::Uint,kk::Uint>(ctx, -1, "putImageData", &Context::putImageData);
                        kk::PutMethod<Context,void>(ctx, -1, "save", &Context::save);
                        kk::PutMethod<Context,void>(ctx, -1, "restore", &Context::restore);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "rect", &Context::rect);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "fillRect", &Context::fillRect);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "strokeRect", &Context::strokeRect);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "clearRect", &Context::clearRect);
                        kk::PutMethod<Context,void>(ctx, -1, "fill", &Context::fill);
                        kk::PutMethod<Context,void>(ctx, -1, "stroke", &Context::stroke);
                        kk::PutMethod<Context,void>(ctx, -1, "beginPath", &Context::beginPath);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float>(ctx, -1, "moveTo", &Context::moveTo);
                        kk::PutMethod<Context,void>(ctx, -1, "closePath", &Context::closePath);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float>(ctx, -1, "lineTo", &Context::lineTo);
                        kk::PutMethod<Context,void>(ctx, -1, "clip", &Context::clip);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "quadraticCurveTo", &Context::quadraticCurveTo);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "bezierCurveTo", &Context::bezierCurveTo);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::Boolean>(ctx, -1, "arc", &Context::arc);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "arcTo", &Context::arcTo);
                        kk::PutMethod<Context,kk::Boolean,kk::ui::Float,kk::ui::Float>(ctx, -1, "isPointInPath", &Context::isPointInPath);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float>(ctx, -1, "scale", &Context::scale);
                        kk::PutMethod<Context,void,kk::ui::Float>(ctx, -1, "rotate", &Context::rotate);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float>(ctx, -1, "translate", &Context::translate);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "transform", &Context::transform);
                        kk::PutMethod<Context,void,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "setTransform", &Context::setTransform);
                        kk::PutMethod<Context,void,kk::CString,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "fillText", &Context::fillText);
                        kk::PutMethod<Context,void,kk::CString,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "strokeText", &Context::strokeText);
                        kk::PutMethod<Context,kk::ui::Float,kk::CString>(ctx, -1, "measureText", &Context::measureText);
                        kk::PutMethod<Context,void,Image *,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float,kk::ui::Float>(ctx, -1, "drawImage", &Context::drawImage);
                        kk::PutProperty<Context,Any>(ctx, -1, "fillStyle", &Context::fillStyleAny, &Context::setFillStyleAny);
                        kk::PutMethod<Context,void,Any>(ctx, -1, "setFillStyle", &Context::setFillStyleAny);
                        kk::PutProperty<Context,Any>(ctx, -1, "strokeStyle", &Context::strokeStyleAny, &Context::setStrokeStyleAny);
                        kk::PutMethod<Context,void,Any>(ctx, -1, "setStrokeStyle", &Context::setStrokeStyleAny);
                        kk::PutProperty<Context,Color>(ctx, -1, "shadowColor", &Context::shadowColor, &Context::setShadowColor);
                        kk::PutProperty<Context,kk::ui::Float>(ctx, -1, "shadowBlur", &Context::shadowBlur, &Context::setShadowBlur);
                        kk::PutMethod<Context,void,kk::ui::Float>(ctx, -1, "setShadowBlur", &Context::setShadowBlur);
                        kk::PutProperty<Context,kk::ui::Float>(ctx, -1, "shadowOffsetX", &Context::shadowOffsetX, &Context::setShadowOffsetX);
                        kk::PutMethod<Context,void,kk::ui::Float>(ctx, -1, "setShadowOffsetX", &Context::setShadowOffsetX);
                        kk::PutProperty<Context,kk::ui::Float>(ctx, -1, "shadowOffsetX", &Context::shadowOffsetY, &Context::setShadowOffsetY);
                        kk::PutMethod<Context,void,kk::ui::Float>(ctx, -1, "setShadowOffsetY", &Context::setShadowOffsetY);
                        kk::PutProperty<Context,kk::CString>(ctx, -1, "lineCap", &Context::lineCapString, &Context::setLineCapString);
                        kk::PutMethod<Context,void,kk::CString>(ctx, -1, "setLineCap", &Context::setLineCapString);
                        kk::PutProperty<Context,kk::CString>(ctx, -1, "lineJoin", &Context::lineJoinString, &Context::setLineJoinString);
                         kk::PutMethod<Context,void,kk::CString>(ctx, -1, "setLineJoin", &Context::setLineJoinString);
                        kk::PutProperty<Context,kk::ui::Float>(ctx, -1, "lineWidth", &Context::lineWidth, &Context::setLineWidth);
                        kk::PutMethod<Context,void,kk::ui::Float>(ctx, -1, "setLineWidth", &Context::setLineWidth);
                        kk::PutProperty<Context,kk::ui::Float>(ctx, -1, "miterLimit", &Context::miterLimit, &Context::setMiterLimit);
                        kk::PutMethod<Context,void,kk::ui::Float>(ctx, -1, "setMiterLimit", &Context::setMiterLimit);
                        kk::PutProperty<Context,kk::ui::Font>(ctx, -1, "font", &Context::font, &Context::setFont);
                        kk::PutMethod<Context,void,kk::ui::Font>(ctx, -1, "setFont", &Context::setFont);
                        kk::PutProperty<Context,kk::CString>(ctx, -1, "textAlign", &Context::textAlignString, &Context::setTextAlignString);
                        kk::PutMethod<Context,void,kk::CString>(ctx, -1, "setTextAlign", &Context::setTextAlignString);
                        kk::PutProperty<Context,kk::CString>(ctx, -1, "textBaseline", &Context::textBaselineString, &Context::setTextBaselineString);
                        kk::PutMethod<Context,void,kk::CString>(ctx, -1, "setTextBaseline", &Context::setTextBaselineString);
                        kk::PutProperty<Context,kk::ui::Float>(ctx, -1, "globalAlpha", &Context::globalAlpha, &Context::setGlobalAlpha);
                        kk::PutMethod<Context,void,kk::ui::Float>(ctx, -1, "setGlobalAlpha", &Context::setGlobalAlpha);
                        kk::PutProperty<Context,kk::CString>(ctx, -1, "globalCompositeOperation", &Context::globalCompositeOperationString, &Context::setGlobalCompositeOperationString);
                        kk::PutMethod<Context,void,kk::CString>(ctx, -1, "setGlobalCompositeOperation", &Context::setGlobalCompositeOperationString);
                        
                    });
                    
                });
                
            }
            
        }
        
        
    }
    
}
