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
            
            auto sp = text->spans();
            auto i = sp.begin();
            
            while (i != sp.end()) {
                
                AttributedTextSpan & span = * i;
                
                if(span.type == AttributedTextSpanTypeText) {
                    
                    NSMutableDictionary * attrs = [NSMutableDictionary dictionaryWithCapacity:4];
                    
                    attrs[NSFontAttributeName] = [UIFont fontWithKerUIFont:&span.font];
                    attrs[NSForegroundColorAttributeName] = [UIColor colorWithKerUIColor:&span.color];
                   
                    
                    [string appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithCString:span.text.c_str() encoding:NSUTF8StringEncoding] attributes:attrs]];
                    
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
            Size v = { (Float) bounds.size.width, (Float) bounds.size.height};
            return v;
        }
        
        
    }
    
}
