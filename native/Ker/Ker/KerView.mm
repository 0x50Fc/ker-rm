//
//  KerView.m
//  Ker
//
//  Created by hailong11 on 2018/12/28.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerView.h"
#import <WebKit/WebKit.h>
#import "UIView+KerViewProtocol.h"
#import "KerApp.h"

namespace kk {
    namespace ui {
        extern CGImageRef GetCGImage(Image * image);
        extern NSAttributedString * GetAttributedText(AttributedText *text,kk::ui::Context * context);
        extern CGImageRef createCGImageWithCGContext(kk::ui::CG::Context * context);
    }
}

@interface KerView () {
    NSMutableDictionary * _views;
}

@end

@implementation KerView

-(instancetype) initWithApp:(KerApp *) app {
    if((self = [super init])) {
        _app = app;
    }
    return self;
}

-(void) dealloc {
    
    NSEnumerator * keyEnum = [_views keyEnumerator];
    
    id key;
    
    while((key = [keyEnum nextObject])) {
        UIView * view = [_views objectForKey:key];
        [view removeFromSuperview];
    }
    
}

-(void) createView:(kk::ui::ViewCreateCommand *) command {
    
    NSString * name = [NSString stringWithCString:command->name.c_str() encoding:NSUTF8StringEncoding];
    
    Class isa = NSClassFromString(name);
    
    if(isa == nil) {
        isa = [UIView class];
    }
    
    UIView * view = [isa KerViewCreateWithConfiguration:(kk::ui::ViewConfiguration *) command->configuration app:_app];
    
    [view KerViewObtain:command->viewId app:_app];
    
    if(_views == nil) {
        _views = [[NSMutableDictionary alloc] initWithCapacity:4];
    }
    
    [_views setObject:view forKey:@(command->viewId)];
    
}

-(void) createNativeView:(kk::ui::ViewNativeCreateCommand *) command {
    
    UIView * view = (__bridge UIView *) command->native;
    
    [view KerViewObtain:command->viewId app:_app];
    
    if(_views == nil) {
        _views = [[NSMutableDictionary alloc] initWithCapacity:4];
    }
    
    [_views setObject:view forKey:@(command->viewId)];
    
}

-(void) deleteView:(kk::Uint64) viewId {
    
    if(_views == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [_views objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    [view KerViewRecycle:viewId app:_app];
    
    [_views removeObjectForKey:key];
    
}

-(void) setAttribute:(kk::Uint64) viewId name:(NSString *) name value:(NSString *) value {
    
    if(_views == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [_views objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    [view KerView:viewId setAttribute:name value:value app:_app];
    
}

-(void) setFrame:(kk::Uint64) viewId frame:(CGRect) frame {
    
    if(_views == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [_views objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    [view setFrame:frame];
    
}

-(void) setContentSize:(kk::Uint64) viewId size:(CGSize) size {
    
    if(_views == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [_views objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    if([view isKindOfClass:[UIScrollView class]]) {
        [(UIScrollView *) view setContentSize:size];
    }
    
}

-(void) setContentOffset:(kk::Uint64) viewId offset:(CGPoint) offset animated:(BOOL) animated {
    
    if(_views == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [_views objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    if([view isKindOfClass:[UIScrollView class]]) {
        [(UIScrollView *) view setContentOffset:offset animated:animated];
    }
    
    
}

-(void) setContent:(kk::Uint64) viewId content:(NSString *) content contentType:(NSString *) contentType basePath:(NSString * )basePath {
    
    if(_views == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [_views objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    [view KerView:viewId setContent:content contentType:contentType basePath:basePath app:_app];
}

-(void) setImage:(kk::Uint64) viewId image:(UIImage *) image {
    
    if(_views == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [_views objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    if([view isKindOfClass:[UIImageView class]]) {
        [(UIImageView *) view setImage:image];
    } else {
        view.layer.contents = (id) [image CGImage];
    }
}

-(void) evaluateJavaScript:(kk::Uint64) viewId code:(NSString *) code {
    
    if(_views == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [_views objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    if([view isKindOfClass:[UIWebView class]]) {
        [(UIWebView *) view stringByEvaluatingJavaScriptFromString:code];
    } else if([view isKindOfClass:[WKWebView class]]) {
        [(WKWebView *) view evaluateJavaScript:code completionHandler:nil];
    }
    
}

-(void) setAttributedString:(kk::Uint64) viewId string:(NSAttributedString *) string {
    
    if(_views == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [_views objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    if([view isKindOfClass:[UILabel class]]) {
        [(UILabel *) view setAttributedText:string];
    } else if([view isKindOfClass:[UITextView class]]) {
        [(UITextView *) view setAttributedText:string];
    }
}

-(void) addSubview:(kk::Uint64) viewId subviewId:(kk::Uint64) subviewId position:(kk::ui::SubviewPosition) position {
    
    if(_views == nil) {
        return ;
    }
    
    UIView * view = [_views objectForKey:@(viewId)];
    
    if(view == nil) {
        return;
    }
    
    UIView * subview = [_views objectForKey:@(subviewId)];
    
    if(subview == nil) {
        return;
    }
    
    if(position == kk::ui::SubviewPositionBack) {
        [view insertSubview:subview atIndex:0];
    } else {
        [view addSubview:subview];
    }
    
}

-(void) removeView:(kk::Uint64) viewId {
    
    if(_views == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [_views objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    [view removeFromSuperview];
}

-(void) execCommand:(kk::ui::ViewCommand *) command {
    
    {
        kk::ui::ViewCreateCommand * v = dynamic_cast<kk::ui::ViewCreateCommand *>(command);
        
        if(v != nullptr) {
            
            v->retain();
            
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view createView:v];
                v->release();
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewNativeCreateCommand * v = dynamic_cast<kk::ui::ViewNativeCreateCommand *>(command);
        
        if(v != nullptr) {
            
            v->retain();
            
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view createNativeView:v];
                v->release();
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewDeleteCommand * v = dynamic_cast<kk::ui::ViewDeleteCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view deleteView:viewId];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewSetCommand * v = dynamic_cast<kk::ui::ViewSetCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            NSString * name = [NSString stringWithCString:v->name.c_str() encoding:NSUTF8StringEncoding];
            NSString * value = [NSString stringWithCString:v->value.c_str() encoding:NSUTF8StringEncoding];
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view setAttribute:viewId name:name value:value];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewSetFrameCommand * v = dynamic_cast<kk::ui::ViewSetFrameCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            CGRect frame = CGRectMake(v->frame.origin.x, v->frame.origin.y, v->frame.size.width, v->frame.size.height);
            
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view setFrame:viewId frame:frame];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewAddsubviewCommand * v = dynamic_cast<kk::ui::ViewAddsubviewCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            kk::Uint64 subviewId = v->subviewId;
            kk::ui::SubviewPosition position = v->position;
            
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view addSubview:viewId subviewId:subviewId position:position];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewRemoveCommand * v = dynamic_cast<kk::ui::ViewRemoveCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view removeView:viewId];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewSetContentSizeCommand * v = dynamic_cast<kk::ui::ViewSetContentSizeCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            CGSize size = CGSizeMake(v->size.width, v->size.height);
            
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view setContentSize:viewId size:size];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewSetContentOffsetCommand * v = dynamic_cast<kk::ui::ViewSetContentOffsetCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            CGPoint offset = CGPointMake(v->offset.x, v->offset.y);
            BOOL animated = v->animated;
            
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view setContentOffset:viewId offset:offset animated:animated];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewSetContentCommand * v = dynamic_cast<kk::ui::ViewSetContentCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            NSString * content = [NSString stringWithCString:v->content.c_str() encoding:NSUTF8StringEncoding];
            NSString * contentType = [NSString stringWithCString:v->contentType.c_str() encoding:NSUTF8StringEncoding];
            NSString * basePath = [NSString stringWithCString:v->basePath.c_str() encoding:NSUTF8StringEncoding];
            
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view setContent:viewId content:content contentType:contentType basePath:basePath];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewSetImageCommand * v = dynamic_cast<kk::ui::ViewSetImageCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            UIImage * image = nil;
            
            CGImageRef i = v->image == nullptr ? nil : kk::ui::GetCGImage(v->image);
            
            if(i != nil) {
                image = [UIImage imageWithCGImage:i];
            }
            
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view setImage:viewId image:image];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewEvaluateJavaScriptCommand * v = dynamic_cast<kk::ui::ViewEvaluateJavaScriptCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            NSString * code = [NSString stringWithCString:v->code.c_str() encoding:NSUTF8StringEncoding];
            
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view evaluateJavaScript:viewId code:code];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewSetAttributedTextCommand * v = dynamic_cast<kk::ui::ViewSetAttributedTextCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            NSAttributedString * string = nil;
            
            if(v->text != nullptr) {
                string = kk::ui::GetAttributedText(v->text,(kk::ui::Context *) [_app CPointer]);
            }
            
            __weak KerView * view = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view setAttributedString:viewId string:string];
            });
            
            return;
        }
    }
    
    
}

-(void) execCanvasCommand:(kk::ui::CanvasCommand *) command {
    
    {
        kk::ui::CanvasDisplayCGContextCommand * cmd = dynamic_cast<kk::ui::CanvasDisplayCGContextCommand *>(command);
        
        if(cmd) {
            
            CGImageRef image = kk::ui::createCGImageWithCGContext(cmd->context);
            
            if(image != nil) {
                kk::Uint64 viewId = cmd->viewId;
                UIImage * i = [UIImage imageWithCGImage:image];
                CGImageRelease(image);
                __weak KerView * view = self;
                dispatch_async(dispatch_get_main_queue(), ^{
                    [view setImage:viewId image:i];
                });
            }
            
            return ;
        }
    }
}


@end
