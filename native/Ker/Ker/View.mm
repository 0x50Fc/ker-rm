//
//  Canvas.m
//  KK
//
//  Created by zhanghailong on 2018/10/29.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>
#import "KerViewProtocol.h"
#import "UIFont+Ker.h"
#import "UIColor+Ker.h"

#include <ui/ui.h>
#include <ui/view.h>
#include <ui/CGContext.h>
#include <core/dispatch.h>




namespace kk {
    
    namespace ui {
        
        extern kk::Strong<kk::ui::CG::Context> createCGContext(kk::Uint width,kk::Uint height);
        extern void displayCGContext(kk::ui::CG::Context * context,CFTypeRef view);
        extern kk::Strong<Image> createImageWithCGContext(kk::ui::CG::Context * context);
        
        extern NSAttributedString * GetAttributedText(AttributedText *text,kk::ui::Context * context);
        extern CGImageRef GetCGImage(Image * image);
        
        NSAttributedString * GetAttributedText(AttributedText *text,kk::ui::Context * context) {
            
            if(text == nullptr) {
                return nil;
            }
            
            NSMutableAttributedString * string = [[NSMutableAttributedString alloc] init];
            
            auto & sp = text->spans();
            auto i = sp.begin();
            
            while (i != sp.end()) {
                
                AttributedTextSpan & span = * i;
                
                if(span.type == AttributedTextSpanTypeText) {
                    
                    if(!span.text.empty()) {
                        
                        NSMutableDictionary * attrs = [NSMutableDictionary dictionaryWithCapacity:4];
                        
                        attrs[NSFontAttributeName] = [UIFont fontWithKerUIFont:&span.font];
                        attrs[NSForegroundColorAttributeName] = [UIColor colorWithKerUIColor:&span.color];
                        attrs[NSKernAttributeName] = @(span.letterSpacing);
                        
                        NSMutableParagraphStyle * style = [[NSMutableParagraphStyle alloc] init];
                        
                        switch(span.textAlign) {
                            case TextAlignLeft:
                            case TextAlignStart:
                                style.alignment = NSTextAlignmentLeft;
                                break;
                            case TextAlignEnd:
                            case TextAlignRight:
                                style.alignment = NSTextAlignmentRight;
                                break;
                            case TextAlignCenter:
                                style.alignment = NSTextAlignmentCenter;
                                break;
                        }
                        
                        style.lineSpacing = span.lineSpacing;
                        style.paragraphSpacing = span.paragraphSpacing;
                        
                        attrs[NSParagraphStyleAttributeName] = style;
                        
                        NSString * v = [NSString stringWithCString:span.text.c_str() encoding:NSUTF8StringEncoding];
                        
                        if(v != nil) {
                            [string appendAttributedString:[[NSAttributedString alloc] initWithString:v attributes:attrs]];
                        }
                        
                    }
                    
                } else if(span.type == AttributedTextSpanTypeImage) {
                    if(span.image != nullptr) {
                        CGImageRef image = GetCGImage(span.image);
                        if(image != nil) {
                            NSTextAttachment * attach = [[NSTextAttachment alloc] init];
                            attach.image = [UIImage imageWithCGImage:image];
                            [string appendAttributedString:[NSAttributedString attributedStringWithAttachment:attach]];
                        }
                    }
                }
                
                i ++;
            }
            
            return string;
        }
        
        Size getAttributedTextContentSize(Context * context,AttributedText * text,kk::Float maxWidth) {
            NSAttributedString * string = GetAttributedText(text, context);
            CGRect bounds = [string boundingRectWithSize:CGSizeMake(maxWidth == 0 ? MAXFLOAT : maxWidth, MAXFLOAT) options:NSStringDrawingUsesLineFragmentOrigin context:nil];
            Size v = { (Float) ceil(bounds.size.width), (Float) ceil(bounds.size.height)};
            return v;
        }
        
        
    }
    
}
