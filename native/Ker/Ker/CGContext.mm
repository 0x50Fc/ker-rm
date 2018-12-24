//
//  UI.m
//  KK
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>

#include <ui/CGContext.h>
#import "UI.h"


namespace kk {

    namespace ui {
    
        namespace CG {
            
            class OSImageData : public ImageData, public CGImageGetter {
            public:
                OSImageData(Uint width,Uint height):ImageData(width,height){}
                virtual ~OSImageData() {
                    if(_image) {
                        CGImageRelease(_image);
                    }
                }
                virtual CGImageRef CGImage() {
                    if(_image == nullptr) {
                        CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
                        CGContextRef ctx = CGBitmapContextCreate(_data, _width, _height, 8, _width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
                        _image = CGBitmapContextCreateImage(ctx);
                        CGColorSpaceRelease(rgbSpace);
                        CGContextRelease(ctx);
                    }
                    return _image;
                }
            protected:
                CGImageRef _image;
            };
            
            class OSLinearGradient: public LinearGradient {
            public:
                OSLinearGradient(CGPoint startPoint, CGPoint endPoint):startPoint(startPoint),endPoint(endPoint) {
                    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
                    _v = CGGradientCreateWithColorComponents(space, (CGFloat *) _colors.data(), (CGFloat *) _locations.data(), _colors.size());
                    CGColorSpaceRelease(space);
                }
                virtual ~OSLinearGradient() {
                    CGGradientRelease(_v);
                }
                CGGradientRef CGGradient() {
                    return _v;
                }
                CGPoint startPoint;
                CGPoint endPoint;
            protected:
                CGGradientRef _v;
            };
            
            class OSRadialGradient: public RadialGradient {
            public:
                OSRadialGradient(CGPoint startCenter,CGFloat startRadius,CGPoint endCenter,CGFloat endRadius)
                    :startCenter(startCenter),startRadius(startRadius),endCenter(endCenter),endRadius(endRadius){
                    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
                    _v = CGGradientCreateWithColorComponents(space, (CGFloat *) _colors.data(), (CGFloat *) _locations.data(), _colors.size());
                    CGColorSpaceRelease(space);
                }
                CGGradientRef CGGradient() {
                    return _v;
                }
                CGPoint startCenter;
                CGFloat startRadius;
                CGPoint endCenter;
                CGFloat endRadius;
            protected:
                CGGradientRef _v;
                
            };
            
            class OSContext : public Context {
            public:
                
                OSContext(kk::Uint width,kk::Uint height)
                :_width(width),_height(height),
                 _lineWidth(0),_textAlign(TextAlignLeft),
                _shadowBlur(0),_shadowOffsetX(0),_shadowOffsetY(0),
                _baseline(TextBaselineAlphabetic),_lineCap(LineCapTypeButt),_lineJoin(LineJoinTypeMiter),_miterLimit(0),
                _globalAlpha(1),_globalCompositeOperation(GlobalCompositeOperationSourceOver){
                    @autoreleasepool {
                        CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
                        _ctx = CGBitmapContextCreate(NULL, width, height, 8, width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
                        CGContextTranslateCTM(_ctx, 0, height);
                        CGContextScaleCTM(_ctx, 1, -1);
                        CGColorSpaceRelease(rgbSpace);
                    }
                }
                
                virtual ~OSContext() {
                    @autoreleasepool {
                        CGContextRelease(_ctx);
                    }
                }
                
                virtual kk::Strong<LinearGradient> createLinearGradient(Float x0,Float y0, Float x1, Float y1) {
                    return new OSLinearGradient(CGPointMake(x0, y0),CGPointMake(x1, y1));
                }
                
                virtual kk::Strong<RadialGradient> createRadialGradient(Float x0,Float y0,Float r0, Float x1, Float y1, Float r1) {
                    return new OSRadialGradient(CGPointMake(x0, y0),r0,CGPointMake(x1, y1),r1);
                }
                
                virtual kk::Strong<Pattern> createPattern(Image * image,PatternType type) {
                    return nullptr;
                }
                
                virtual kk::Strong<ImageData> createImageData(Uint width,Uint height) {
                    
                    if(width == 0 || height == 0) {
                        return nullptr;
                    }
                    
                    return new OSImageData(width,height);
                }
                
                virtual kk::Strong<ImageData> getImageData(Uint x,Uint y,Uint width,Uint height) {
                   
                    if(width == 0 || height == 0) {
                        return nullptr;
                    }
                    
                    kk::Strong<ImageData> image = createImageData(width, height);
  
                    if(image != nullptr) {
                        
                        Ubyte * p = image->data();
                        Ubyte * s = (Ubyte *) CGBitmapContextGetData(_ctx);
                        
                        for(Uint r = 0; r < height; r ++) {
                            
                            for(Uint c = 0; c < width; c ++) {
                                
                                if(r + y < _height && c + x < _width) {
                                    for(Uint n = 0; n < 4; n ++) {
                                        *p = s[((r + y) * _width + (c + x)) * 4 + n];
                                        p ++;
                                    }
                                } else {
                                    p += 4;
                                }
                            }
                        }
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
                    
                    Ubyte * p = image->data();
                    Ubyte * s = (Ubyte *) CGBitmapContextGetData(_ctx);
                    
                    for(Uint r = 0; r < height; r ++) {
                        
                        for(Uint c = 0; c < width; c ++) {
                            
                            Uint i_p = ((r + y) * mWidth + (c + x)) * 4;
                            Uint i_s = ((r + dirtyY) * _width + (c + dirtyX)) * 4;
                            
                            for(Uint n = 0; n < 4; n ++) {
                                
                                s[i_s + n] = p[i_p + n];
                                
                            }
                            
                        }
                    }
                    
                }
                
                
                virtual void save() {
                    CGContextSaveGState(_ctx);
                }
                
                virtual void restore() {
                    CGContextRestoreGState(_ctx);
                }
                
                virtual void rect(Float x, Float y,Float width,Float height) {
                    CGContextAddRect(_ctx, CGRectMake(x, y, width, height));
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
                    CGContextClearRect(_ctx, CGRectMake(x, y, width, height));
                }
                
                virtual void clear() {
                    void * data = CGBitmapContextGetData(_ctx);
                    memset(data, 0, _width * _height * 4);
                }
                
                virtual void fill() {
                    
                    {
                        OSLinearGradient * v = _fillStyle;
                        
                        if(v) {
                            
                            CGGradient * gradient = v->CGGradient();
                            
                            CGContextClip(_ctx);
                            
                            CGContextDrawLinearGradient(_ctx, gradient, v->startPoint, v->endPoint, kCGGradientDrawsBeforeStartLocation);
                            
                            
                            CGGradientRelease(gradient);
                            
                            return;
                        }
                    }
                    
                    {
                        OSRadialGradient * v = _fillStyle;
                        
                        if(v) {
                            
                            CGGradient * gradient = v->CGGradient();
                            
                            CGContextClip(_ctx);
                            
                            CGContextDrawRadialGradient(_ctx, gradient, v->startCenter, v->startRadius, v->endCenter, v->endRadius, kCGGradientDrawsBeforeStartLocation);
                            
                            CGGradientRelease(gradient);
                            
                            return;
                        }
                    }
                    
                    CGContextDrawPath(_ctx, kCGPathFill);
                }
                
                virtual void stroke() {
                    
                    {
                        OSLinearGradient * v = _strokeStyle;
                        
                        if(v) {
                            
                            CGGradient * gradient = v->CGGradient();
                            
                            CGPathRef path = CGContextCopyPath(_ctx);
                            
                            CGRect r = CGPathGetBoundingBox(path);
                            
                            size_t width = (size_t) ceil(r.size.width);
                            size_t height = (size_t) ceil(r.size.height);
                            
                            CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
                            
                            CGContextRef c = CGBitmapContextCreate(NULL, width, height, 8, width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
                            CGContextTranslateCTM(c, 0, height);
                            CGContextScaleCTM(c, 1, -1);
                            CGColorSpaceRelease(rgbSpace);
                            
                            CGContextTranslateCTM(c, - r.origin.x, - r.origin.y);
                            
                            CGContextAddPath(c, path);
                            
                            CGContextSetLineWidth(c, _lineWidth);
                            CGContextSetRGBStrokeColor(c, 0, 0, 0, 1);
                            CGContextDrawPath(c, kCGPathStroke);
                            
                            CGImageRef image = CGBitmapContextCreateImage(c);
                            
                            CGContextRelease(c);
                            CGPathRelease(path);
                            
                            
                            CGContextClipToMask(_ctx, r, image);
                            
                            CGContextDrawLinearGradient(_ctx, gradient, v->startPoint, v->endPoint, kCGGradientDrawsBeforeStartLocation);
                            
                            CGGradientRelease(gradient);
                            
                            CGImageRelease(image);
                            
                            return;
                        }
                    }
                    
                    {
                        OSRadialGradient * v = _strokeStyle;
                        
                        if(v) {
                            
                            CGGradientRef gradient = (CGGradientRef) v;
                            
                            CGPathRef path = CGContextCopyPath(_ctx);
                            
                            CGRect r = CGPathGetBoundingBox(path);
                            
                            size_t width = (size_t) ceil(r.size.width);
                            size_t height = (size_t) ceil(r.size.height);
                            
                            CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
                            
                            CGContextRef c = CGBitmapContextCreate(NULL, width, height, 8, width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
                            CGContextTranslateCTM(c, 0, height);
                            CGContextScaleCTM(c, 1, -1);
                            CGColorSpaceRelease(rgbSpace);
                            
                            CGContextTranslateCTM(c, - r.origin.x, - r.origin.y);
                            
                            CGContextAddPath(c, path);
                            
                            CGContextSetLineWidth(c, _lineWidth);
                            CGContextSetRGBStrokeColor(c, 0, 0, 0, 1);
                            CGContextDrawPath(c, kCGPathStroke);
                            
                            CGImageRef image = CGBitmapContextCreateImage(c);
                            
                            CGContextRelease(c);
                            CGPathRelease(path);
                            
                            CGContextClipToMask(_ctx, r, image);
                            
                            CGContextDrawRadialGradient(_ctx, gradient, v->startCenter, v->startRadius, v->endCenter, v->endRadius, kCGGradientDrawsBeforeStartLocation);
                            
                            CGGradientRelease(gradient);
                            
                            CGImageRelease(image);
                            
                            return;
                        }
                    }
                    
                    CGContextDrawPath(_ctx, kCGPathStroke);
                }
                
                virtual void beginPath() {
                    CGContextBeginPath(_ctx);
                }
                
                virtual void closePath() {
                    CGContextClosePath(_ctx);
                }
                
                virtual void moveTo(Float x,Float y) {
                    CGContextMoveToPoint(_ctx, x, y);
                }
                
                virtual void lineTo(Float x,Float y) {
                    CGContextAddLineToPoint(_ctx,x, y);
                }
                
                virtual void clip() {
                    CGContextClip(_ctx);
                }
                
                virtual void quadraticCurveTo(Float cpx,Float cpy,Float x,Float y) {
                    CGContextAddQuadCurveToPoint(_ctx, cpx, cpy, x, y);
                }
                
                virtual void bezierCurveTo(Float cp1x,Float cp1y,Float cp2x,Float cp2y,Float x,Float y) {
                    CGContextAddCurveToPoint(_ctx, cp1x, cp1y, cp2x, cp2y, x, y);
                }
                
                virtual void arc(Float x,Float y,Float r, Float sAngle,Float eAngle,Boolean counterclockwise) {
                    CGContextAddArc(_ctx, x, y, r, sAngle, eAngle, counterclockwise ? 1 : 0);
                }
                
                virtual void arcTo(Float x1,Float y1,Float x2,Float y2,Float r) {
                    CGContextAddArcToPoint(_ctx, x1, y1, x2, y2, r);
                }
                
                virtual Boolean isPointInPath(Float x,Float y) {
                    return CGContextPathContainsPoint(_ctx, CGPointMake(x, y), kCGPathFillStroke);
                }
                
                virtual void scale(Float sx,Float sy) {
                    CGContextScaleCTM(_ctx, sx, sy);
                }
                
                virtual void rotate(Float angle) {
                    CGContextRotateCTM(_ctx, angle);
                }
                
                virtual void translate(Float dx,Float dy) {
                    CGContextTranslateCTM(_ctx, dx, dy);
                }
                
                virtual void transform(Float a,Float b,Float c,Float d,Float e,Float f) {
                    CGContextConcatCTM(_ctx, CGAffineTransformMake(a, b, c, d, e, f));
                }
                
                virtual void setTransform(Float a,Float b,Float c,Float d,Float e,Float f) {
                    CGAffineTransform v = CGContextGetCTM(_ctx);
                    v = CGAffineTransformInvert(v);
                    v = CGAffineTransformConcat(v, CGAffineTransformMake(a, b, c, d, e, f));
                    CGContextConcatCTM(_ctx, v);
                }
                
                Float GetAttributeStringBaseline(NSDictionary * attributes,Context * ctx) {
                    
                    UIFont * font = [attributes valueForKey:NSFontAttributeName];
                    
                    if(font) {
                        switch (ctx->textBaseline()) {
                            case TextBaselineAlphabetic:
                                return -font.ascender;
                                break;
                            case TextBaselineBottom:
                                return -font.lineHeight;
                                break;
                            case TextBaselineMiddle:
                                return -font.lineHeight * 0.5;
                                break;
                            case TextBaselineHanging:
                                return font.descender;
                                break;
                            case TextBaselineIdeographic:
                                return - font.ascender - font.leading;
                            default:
                                break;
                        }
                    }
                    
                    return 0;
                };
                
                NSMutableDictionary *CreateAttributeStringAttributes(Context * ctx) {
                    
                    NSMutableDictionary * attributes = [NSMutableDictionary dictionaryWithCapacity: 4];
                    
                    {
                        UIFont * v = nil;
                        Font font = ctx->font();
                        
                        if(font.family != "") {
                            
                            NSString * name = nil;
                            NSArray * fontNames = [UIFont fontNamesForFamilyName:[NSString stringWithCString:font.family.c_str() encoding:NSUTF8StringEncoding]];
                            
                            for(NSString * fontName in fontNames) {
                                
                                if(font.style == FontStyleItalic) {
                                    if([fontName hasSuffix:@"Italic"]) {
                                        name = fontName;
                                        break;
                                    }
                                } else if(font.weight == FontWeightBold) {
                                    if([fontName hasSuffix:@"Bold"]) {
                                        name = fontName;
                                        break;
                                    }
                                } else {
                                    name = fontName;
                                    break;
                                }
                            }
                            
                            if(name == nil && [fontNames count] > 0) {
                                name = [fontNames objectAtIndex:0];
                            }
                            
                            if(name) {
                                v = [UIFont fontWithName:name size:font.size];
                            }
                        }
                        
                        if(v == nil) {
                            if(font.style == FontStyleItalic) {
                                v = [UIFont italicSystemFontOfSize:font.size];
                            } else if(font.weight == FontWeightBold) {
                                v = [UIFont boldSystemFontOfSize:font.size];
                            } else {
                                v = [UIFont systemFontOfSize:font.size];
                            }
                        }
                        
                        attributes[NSFontAttributeName] = v;
                        
                    }
                    
                    
                    return attributes;
                }
                
                
                virtual void fillText(kk::CString text,Float x,Float y,Float maxWidth) {
                    
                    if(text == nullptr) {
                        return ;
                    }
                    
                    {
                        OSLinearGradient * v = _fillStyle;
                        
                        if(v) {
                            
                            @autoreleasepool {
                                
                                CGGradient * gradient = v->CGGradient();
                                
                                NSMutableDictionary * attributes = CreateAttributeStringAttributes(this);
                                
                                NSAttributedString * string = [[NSAttributedString alloc] initWithString:[NSString stringWithCString:text encoding:NSUTF8StringEncoding] attributes:attributes];
                                
                                CGRect r = [string boundingRectWithSize:CGSizeMake(maxWidth > 0 ?maxWidth: MAXFLOAT, MAXFLOAT)
                                                                options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                                
                                size_t width = (size_t) ceil(r.size.width);
                                size_t height = (size_t) ceil(r.size.height);
                                
                                CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
                                CGContextRef c = CGBitmapContextCreate(NULL, width, height, 8, width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
                                CGColorSpaceRelease(rgbSpace);
                                
                                UIGraphicsPushContext(c);
                                
                                [string drawWithRect:r options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                                
                                UIGraphicsPopContext();
                                
                                CGImageRef image = CGBitmapContextCreateImage(c);
                                
                                CGContextRelease(c);
                                
                                r.origin.x = x;
                                r.origin.y = y;
                                
                                switch (_textAlign) {
                                    case TextAlignEnd:
                                    case TextAlignRight:
                                        r.origin.x = x - r.size.width;
                                        break;
                                    case TextAlignCenter:
                                        r.origin.x = x - r.size.width * 0.5f;
                                        break;
                                    default:
                                        break;
                                }
                                
                                r.origin.y += GetAttributeStringBaseline(attributes,this);
                                
                                CGContextClipToMask(_ctx, r, image);
                                
                                CGContextDrawLinearGradient(_ctx, gradient, v->startPoint, v->endPoint, kCGGradientDrawsBeforeStartLocation);
                                
                                CGGradientRelease(gradient);
                                
                                CGImageRelease(image);
                                
                            }
                            
                            return;
                        }
                    }
                    
                    {
                        OSRadialGradient * v = _strokeStyle;
                        
                        if(v) {
                            
                            @autoreleasepool {
                                
                                CGGradient * gradient = v->CGGradient();
                                
                                NSMutableDictionary * attributes = CreateAttributeStringAttributes(this);
                                
                                NSAttributedString * string = [[NSAttributedString alloc] initWithString:[NSString stringWithCString:text encoding:NSUTF8StringEncoding] attributes:attributes];
                                
                                CGRect r = [string boundingRectWithSize:CGSizeMake(maxWidth > 0 ?maxWidth: MAXFLOAT, MAXFLOAT)
                                                                options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                                
                                size_t width = (size_t) ceil(r.size.width);
                                size_t height = (size_t) ceil(r.size.height);
                                
                                CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
                                CGContextRef c = CGBitmapContextCreate(NULL, width, height, 8, width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
                                CGColorSpaceRelease(rgbSpace);
                                
                                UIGraphicsPushContext(c);
                                
                                [string drawWithRect:r options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                                
                                UIGraphicsPopContext();
                                
                                CGImageRef image = CGBitmapContextCreateImage(c);
                                
                                CGContextRelease(c);
                                
                                r.origin.x = x;
                                r.origin.y = y;
                                
                                switch (_textAlign) {
                                    case TextAlignEnd:
                                    case TextAlignRight:
                                        r.origin.x = x - r.size.width;
                                        break;
                                    case TextAlignCenter:
                                        r.origin.x = x - r.size.width * 0.5f;
                                        break;
                                    default:
                                        break;
                                }
                                
                                r.origin.y += GetAttributeStringBaseline(attributes,this);
                                
                                CGContextClipToMask(_ctx, r, image);
                                
                                CGContextDrawRadialGradient(_ctx, gradient, v->startCenter, v->startRadius, v->endCenter, v->endRadius, kCGGradientDrawsBeforeStartLocation);
                                
                                CGGradientRelease(gradient);
                                
                                CGImageRelease(image);
                                
                            }
                            
                            return;
                        }
                    }
                    
                    @autoreleasepool {
                        
                        NSMutableDictionary * attributes = CreateAttributeStringAttributes(this);
                        
                        attributes[NSForegroundColorAttributeName] = [UIColor colorWithRed:_fillColor.r green:_fillColor.g blue:_fillColor.b alpha:_fillColor.a];
                        
                        NSAttributedString * string = [[NSAttributedString alloc] initWithString:[NSString stringWithCString:text encoding:NSUTF8StringEncoding] attributes:attributes];
                        
                        CGRect r = [string boundingRectWithSize:CGSizeMake(MAXFLOAT, MAXFLOAT) options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                        
                        r.origin.x = x;
                        r.origin.y = y;
                        
                        switch (_textAlign) {
                            case TextAlignEnd:
                            case TextAlignRight:
                                r.origin.x = x - r.size.width;
                                break;
                            case TextAlignCenter:
                                r.origin.x = x - r.size.width * 0.5f;
                                break;
                            default:
                                break;
                        }
                        
                        r.origin.y += GetAttributeStringBaseline(attributes,this);
                        
                        UIGraphicsPushContext(_ctx);
                        
                        [string drawWithRect:r options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                        
                        UIGraphicsPopContext();
                        
                    }
                    
                }
                
                virtual void strokeText(kk::CString text,Float x,Float y,Float maxWidth) {
                    
                    if(text == nullptr) {
                        return ;
                    }
                    
                    {
                        OSLinearGradient * v = _strokeStyle;
                        
                        if(v) {
                            
                            @autoreleasepool {
                                
                                CGGradientRef gradient = v->CGGradient();
                                
                                NSMutableDictionary * attributes = CreateAttributeStringAttributes(this);
                                
                                attributes[NSStrokeColorAttributeName] = [UIColor blackColor];
                                attributes[NSStrokeWidthAttributeName] = @(_lineWidth);
                                
                                NSAttributedString * string = [[NSAttributedString alloc] initWithString:[NSString stringWithCString:text encoding:NSUTF8StringEncoding] attributes:attributes];
                                
                                CGRect r = [string boundingRectWithSize:CGSizeMake(maxWidth > 0 ?maxWidth: MAXFLOAT, MAXFLOAT)
                                                                options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                                
                                size_t width = (size_t) ceil(r.size.width);
                                size_t height = (size_t) ceil(r.size.height);
                                
                                CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
                                CGContextRef c = CGBitmapContextCreate(NULL, width, height, 8, width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
                                CGColorSpaceRelease(rgbSpace);
                                
                                UIGraphicsPushContext(c);
                                
                                [string drawWithRect:r options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                                
                                UIGraphicsPopContext();
                                
                                CGImageRef image = CGBitmapContextCreateImage(c);
                                
                                CGContextRelease(c);
                                
                                r.origin.x = x;
                                r.origin.y = y;
                                
                                switch (_textAlign) {
                                    case TextAlignEnd:
                                    case TextAlignRight:
                                        r.origin.x = x - r.size.width;
                                        break;
                                    case TextAlignCenter:
                                        r.origin.x = x - r.size.width * 0.5f;
                                        break;
                                    default:
                                        break;
                                }
                                
                                r.origin.y += GetAttributeStringBaseline(attributes,this);
                                
                                CGContextClipToMask(_ctx, r, image);
                                
                                CGContextDrawLinearGradient(_ctx, gradient, v->startPoint, v->endPoint, kCGGradientDrawsBeforeStartLocation);
                                
                                CGGradientRelease(gradient);
                                
                                CGImageRelease(image);
                                
                            }
                            
                            return;
                        }
                    }
                    
                    {
                        OSRadialGradient * v = _strokeStyle;
                        
                        if(v) {
                            
                            @autoreleasepool {
                                
                                CGGradientRef gradient = v->CGGradient();
                                
                                NSMutableDictionary * attributes = CreateAttributeStringAttributes(this);
                                
                                attributes[NSStrokeColorAttributeName] = [UIColor blackColor];
                                attributes[NSStrokeWidthAttributeName] = @(_lineWidth);
                                
                                NSAttributedString * string = [[NSAttributedString alloc] initWithString:[NSString stringWithCString:text encoding:NSUTF8StringEncoding] attributes:attributes];
                                
                                CGRect r = [string boundingRectWithSize:CGSizeMake(maxWidth > 0 ?maxWidth: MAXFLOAT, MAXFLOAT)
                                                                options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                                
                                size_t width = (size_t) ceil(r.size.width);
                                size_t height = (size_t) ceil(r.size.height);
                                
                                CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
                                CGContextRef c = CGBitmapContextCreate(NULL, width, height, 8, width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
                                CGColorSpaceRelease(rgbSpace);
                                
                                UIGraphicsPushContext(c);
                                
                                [string drawWithRect:r options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                                
                                UIGraphicsPopContext();
                                
                                CGImageRef image = CGBitmapContextCreateImage(c);
                                
                                CGContextRelease(c);
                                
                                r.origin.x = x;
                                r.origin.y = y;
                                
                                switch (_textAlign) {
                                    case TextAlignEnd:
                                    case TextAlignRight:
                                        r.origin.x = x - r.size.width;
                                        break;
                                    case TextAlignCenter:
                                        r.origin.x = x - r.size.width * 0.5f;
                                        break;
                                    default:
                                        break;
                                }
                                
                                r.origin.y += GetAttributeStringBaseline(attributes,this);
                                
                                CGContextClipToMask(_ctx, r, image);
                                
                                CGContextDrawRadialGradient(_ctx, gradient, v->startCenter, v->startRadius, v->endCenter, v->endRadius, kCGGradientDrawsBeforeStartLocation);
                                
                                CGGradientRelease(gradient);
                                
                                CGImageRelease(image);
                                
                            }
                            
                            return;
                        }
                    }
                    
                    @autoreleasepool {
                        
                        NSMutableDictionary * attributes = CreateAttributeStringAttributes(this);
                        
                        attributes[NSStrokeColorAttributeName] = [UIColor colorWithRed:_strokeColor.r green:_strokeColor.g blue:_strokeColor.b alpha:_strokeColor.a];
                        
                        attributes[NSStrokeWidthAttributeName] = @(_lineWidth);
                        
                        NSAttributedString * string = [[NSAttributedString alloc] initWithString:[NSString stringWithCString:text encoding:NSUTF8StringEncoding] attributes:attributes];
                        
                        CGRect r = [string boundingRectWithSize:CGSizeMake(maxWidth > 0 ?maxWidth: MAXFLOAT, MAXFLOAT)
                                                        options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                        
                        r.origin.x = x;
                        r.origin.y = y;
                        
                        switch (_textAlign) {
                            case TextAlignEnd:
                            case TextAlignRight:
                                r.origin.x = x - r.size.width;
                                break;
                            case TextAlignCenter:
                                r.origin.x = x - r.size.width * 0.5f;
                                break;
                            default:
                                break;
                        }
                        
                        r.origin.y += GetAttributeStringBaseline(attributes,this);
                        
                        UIGraphicsPushContext(_ctx);
                        
                        [string drawWithRect:r options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                        
                        UIGraphicsPopContext();
                        
                    }
                    
                }
                
                virtual Float measureText(kk::CString text) {
                    
                    if(text == nullptr) {
                        return 0.0f;
                    }
                    
                    CGRect r = CGRectZero;
                    
                    @autoreleasepool {
                        
                        NSMutableDictionary * attributes = CreateAttributeStringAttributes(this);
                        
                        NSAttributedString * string = [[NSAttributedString alloc] initWithString:[NSString stringWithCString:text encoding:NSUTF8StringEncoding] attributes:attributes];
                        
                        r = [string boundingRectWithSize:CGSizeMake(MAXFLOAT, MAXFLOAT) options:NSStringDrawingUsesLineFragmentOrigin context:nil];
                        
                    }
                    
                    return r.size.width;
                }
                
                virtual void drawImage(Image * image,Float sx,Float sy,Float swidth,Float sheight,Float x,Float y,Float width,Float height) {
                    
                    if(image == nullptr) {
                        return;
                    }
                    
                    CGImageRef CGImage = GetCGImage(image);
                    
                    if(CGImage == nil) {
                        return;
                    }
                    
                    CGContextSaveGState(_ctx);
                    
                    if(swidth >0 && sheight > 0) {
                        
                        if(width <= 0) {
                            width = swidth;
                        }
                        
                        if(height <= 0) {
                            height = sheight;
                        }
                        
                        CGContextClipToRect(_ctx, CGRectMake(x, y, width, height));
                        
                        CGContextDrawImage(_ctx, CGRectMake(-sx, -sy, CGImageGetWidth(CGImage), CGImageGetHeight(CGImage)), CGImage);
                        
                    } else {
                        
                        if(width <= 0) {
                            width = CGImageGetWidth(CGImage);
                        }
                        
                        if(height <= 0) {
                            height = CGImageGetHeight(CGImage);
                        }
                        
                        CGContextDrawImage(_ctx, CGRectMake(x, y, width, height), CGImage);
                    }
                    
                    CGContextRestoreGState(_ctx);
                    
                }
                
                static void OSContext_CGPatternDrawPatternCallback(void * info,
                                                                   CGContextRef ctx) {
                    Pattern * p = (Pattern *) info;
                    
                    Image * image = p->image();
                    CGImageRef CGImage = GetCGImage( image );
                    
                    if(image && CGImage) {
                        CGContextDrawImage(ctx, CGRectMake(0, 0, image->width(), image->height()), CGImage);
                    }
                }
                
                static void OSContext_CGPatternReleaseInfoCallback(void * info) {
                    Pattern * p = (Pattern *) info;
                    p->release();
                }
                
                virtual void setFillStyle(Style * v) {
                    
                    Pattern * p = dynamic_cast<Pattern *>(v);
                    
                    if(p) {
                        
                        Image * image = p->image();
                        
                        CGPatternCallbacks cb = {0,OSContext_CGPatternDrawPatternCallback,OSContext_CGPatternReleaseInfoCallback};
                        
                        CGPatternRef vv = CGPatternCreate(p, CGRectMake(0, 0, image->width(), image->height()), CGAffineTransformIdentity, image->width(), image->height(), kCGPatternTilingNoDistortion, false, &cb);
                        
                        p->retain();
                        
                        CGContextSetFillPattern(_ctx, vv, nil);
                        
                        CGPatternRelease(vv);
                    }
                    
                }
                
                virtual Style * fillStyle() {
                    return _fillStyle;
                }
                
                virtual void setFillColor(Color v) {
                    CGContextSetRGBFillColor(_ctx, v.r, v.g, v.b, v.a);
                }
                
                virtual Color fillColor() {
                    return _fillColor;
                }
                
                virtual void setStrokeStyle(Style * v) {
                    
                    Pattern * p = dynamic_cast<Pattern *>(v);
                    
                    if(p) {
                        Image * image = p->image();
                        
                        CGPatternCallbacks cb = {0,OSContext_CGPatternDrawPatternCallback,OSContext_CGPatternReleaseInfoCallback};
                        
                        CGPatternRef vv = CGPatternCreate(p, CGRectMake(0, 0, image->width(), image->height()), CGAffineTransformIdentity, image->width(), image->height(), kCGPatternTilingNoDistortion, false, &cb);
                        
                        p->retain();
                        
                        CGContextSetStrokePattern(_ctx, vv, nil);
                        
                        CGPatternRelease(vv);
                    }
                }
                
                virtual Style * strokeStyle() {
                    return _strokeStyle;
                }
                
                virtual void setStrokeColor(Color v) {
                    CGContextSetRGBStrokeColor(_ctx, v.r, v.g, v.b, v.a);
                }
                
                virtual Color strokeColor() {
                    return _strokeColor;
                }
                
                virtual void setShadowColor(Color v) {
                    @autoreleasepool {
                        UIColor * vv = [UIColor colorWithRed:_shadowColor.r green:_shadowColor.g blue:_shadowColor.b alpha:_shadowColor.a];
                        CGContextSetShadowWithColor(_ctx, CGSizeMake(_shadowOffsetX, _shadowOffsetY), _shadowBlur, vv.CGColor);
                    }
                    
                }
                
                virtual Color shadowColor() {
                    return _shadowColor;
                }
                
                virtual void setShadowBlur(Float v) {
                    @autoreleasepool {
                        UIColor * vv = [UIColor colorWithRed:_shadowColor.r green:_shadowColor.g blue:_shadowColor.b alpha:_shadowColor.a];
                        CGContextSetShadowWithColor(_ctx, CGSizeMake(_shadowOffsetX, _shadowOffsetY), _shadowBlur, vv.CGColor);
                    }
                }
                
                virtual Float shadowBlur() {
                    return _shadowBlur;
                }
                
                virtual void setShadowOffsetX(Float v) {
                    @autoreleasepool {
                        UIColor * vv = [UIColor colorWithRed:_shadowColor.r green:_shadowColor.g blue:_shadowColor.b alpha:_shadowColor.a];
                        CGContextSetShadowWithColor(_ctx, CGSizeMake(_shadowOffsetX, _shadowOffsetY), _shadowBlur, vv.CGColor);
                    }
                }
                
                virtual Float shadowOffsetX() {
                    return _shadowOffsetX;
                }
                
                virtual void setShadowOffsetY(Float v) {
                    @autoreleasepool {
                        UIColor * vv = [UIColor colorWithRed:_shadowColor.r green:_shadowColor.g blue:_shadowColor.b alpha:_shadowColor.a];
                        CGContextSetShadowWithColor(_ctx, CGSizeMake(_shadowOffsetX, _shadowOffsetY), _shadowBlur, vv.CGColor);
                    }
                }
                
                virtual Float shadowOffsetY() {
                    return _shadowOffsetY;
                }
                
                virtual void setLineCap(LineCapType v) {
                    CGContextSetLineCap(_ctx, (CGLineCap) v);
                    _lineCap = v;
                }
                
                virtual LineCapType lineCap() {
                    return _lineCap;
                }
                
                virtual void setLineJoin(LineJoinType v) {
                    CGContextSetLineJoin(_ctx, (CGLineJoin)v);
                    _lineJoin = v;
                }
                
                virtual LineJoinType lineJoin() {
                    return _lineJoin;
                }
                
                virtual void setLineWidth(Float v) {
                    CGContextSetLineWidth(_ctx, v);
                    _lineWidth = v;
                }
                
                virtual Float lineWidth() {
                    return _lineWidth;
                }
                
                virtual void setMiterLimit(Float v) {
                    CGContextSetMiterLimit(_ctx, v);
                    _miterLimit = v;
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
                    CGContextSetAlpha(_ctx, v);
                    
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
                
                virtual CGImageRef createCGImage() {
                    return CGBitmapContextCreateImage(_ctx);
                }
     
            protected:
                CGContextRef _ctx;
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
            
        }
        
        kk::Strong<kk::ui::CG::Context> createCGContext(kk::Uint width,kk::Uint height) {
            return new kk::ui::CG::OSContext(width,height);
        }
        
        void displayCGContext(kk::ui::CG::Context * context,CFTypeRef view) {
            kk::ui::CG::OSContext * v = dynamic_cast<kk::ui::CG::OSContext *>(context);
            if(v) {
                @autoreleasepool {
                    UIView * vv = (__bridge UIView *) view;
                    CGImageRef image = v->createCGImage();
                    
                    dispatch_async(dispatch_get_main_queue(), ^{
                        @autoreleasepool {
                            vv.layer.contents = (__bridge id) image;
                            CGImageRelease(image);
                        }
                    });
                    
                }
            }
        }
        
        extern Strong<Image> createCGImage(CGImageRef image);
        
        kk::Strong<Image> createImageWithCGContext(kk::ui::CG::Context * context) {
        
            kk::ui::CG::OSContext * v = dynamic_cast<kk::ui::CG::OSContext *>(context);
            if(v) {
                
                @autoreleasepool {
                    
                    CGImageRef image = v->createCGImage();
                    
                    Strong<Image> r = createCGImage(image);
                    
                    CGImageRelease(image);
                    
                    return r;
                    
                }
            }
            
            return nullptr;
        }
        
    }
    
    
    
}

