//
//  UI.m
//  Ker
//
//  Created by zhanghailong on 2019/1/3.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include <ui/ui.h>
#include <ui/app.h>
#include <ui/page.h>
#include <ui/package.h>
#include <ui/ViewElement.h>
#include <ui/TextElement.h>
#include <ui/PageElement.h>
#include <ui/ImageElement.h>
#include <core/uri.h>

#import "KerUI.h"
#import "KerPage.h"
#import "KerPageViewController.h"
#import "KerPageWindowController.h"
#import "KerObject.h"
#import "UIView+KerViewProtocol.h"
#import <MobileCoreServices/MobileCoreServices.h>
#import "KerURLProtocol.h"
#import "KerView.h"
#import "KerAudio.h"

static NSMutableDictionary * KerUIPages = nil;
static NSMutableDictionary * KerUIViews = nil;

@interface KerUI() {
    
}

+(void) setPage:(KerPage *) page pageId:(kk::Uint64) pageId;

+(void) execViewCommand:(kk::ui::ViewCommand *) command app:(KerId) appid;

+(void) execCanvasCommand:(kk::ui::CanvasCommand *) command app:(KerId) appid;

+(void) execPageCommand:(kk::ui::PageCommand *) command app:(KerId) appid;

@end

namespace kk {

    extern ::dispatch_queue_t DispatchQueueGCD(DispatchQueue * queue);
    
    namespace ui {
     
        extern CGImageRef GetCGImage(Image * image);
        extern NSAttributedString * GetAttributedText(AttributedText *text,kk::ui::Context * context);
        extern CGImageRef createCGImageWithCGContext(kk::ui::CG::Context * context);
        
        App::App(kk::Uint64 appid,kk::CString basePath,kk::CString appkey)
            :App(appid,basePath,"iOS",[[KerUI userAgent] UTF8String],appkey)
        {
        }
        
        void UI::commitTransaction() {
            
        }
        
        void UI::execCommand(App * app,Command * command) {
            startTransaction();
            
            {
                kk::ui::AppBackCommand * cmd = dynamic_cast<kk::ui::AppBackCommand *>(command);
                
                if(cmd) {
                    
                    @autoreleasepool {
                        
                        NSUInteger delta = cmd->delta;
                        BOOL animated = cmd->animated;
                        
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [KerUI popPageViewController:delta animated:animated];
                        });
                        
                    }
                    
                    return;
                }
            }
            
            
            {
                kk::ui::PageCommand * cmd = dynamic_cast<kk::ui::PageCommand *>(command);
                
                if(cmd) {
                    
                    @autoreleasepool {
                        
                        [KerUI execPageCommand:cmd app:app->appid()];
                        
                    }
                    
                    return;
                }
            }
            
            {
                kk::ui::ViewCommand * cmd = dynamic_cast<kk::ui::ViewCommand *>(command);
                
                if(cmd) {
                    
                    @autoreleasepool {
                        
                        [KerUI execViewCommand:cmd app:app->appid()];
                        
                    }
                    
                    return;
                }
            }
        
            
            {
                kk::ui::CanvasCommand * cmd = dynamic_cast<kk::ui::CanvasCommand *>(command);
                
                if(cmd) {
                    
                    @autoreleasepool {
                        [KerUI execCanvasCommand:cmd app:app->appid()];
                    }
                    
                    return;
                }
            }
            
        }
        
        void Page::opening() {
            
            @autoreleasepool {
                
                kk::Uint64 pageId = _pageId;
                
                KerPage * page = [[KerPage alloc] initWithPage:this];
                
                dispatch_async(dispatch_get_main_queue(), ^{
                    [KerUI setPage:page pageId:pageId];
                });
            }
            
        }
        
        void Page::open(kk::Boolean animated) {
            
            @autoreleasepool {
                
                kk::Uint64 pageId = _pageId;
            
                dispatch_async(dispatch_get_main_queue(), ^{
                    KerPage * page = [KerUI getPage:pageId];
                    if(page) {
                        [KerUI openPageViewController:pageId animated:animated];
                    }
                });
            }
            
        }
        
    }
}


@implementation KerUI

+(void) setPage:(KerPage *) page pageId:(kk::Uint64) pageId {
    if(KerUIPages == nil) {
        KerUIPages = [[NSMutableDictionary alloc] init];
    }
    KerUIPages[@(pageId)] = page;
}

+(KerPage *) getPage:(KerId) pageId {
    return KerUIPages[@(pageId)];
}

+(void) removePage:(KerId) pageId {
    id key =@(pageId);
    KerPage * page = KerUIPages[key];
    if(page != nil) {
        page.delegate = nil;
        [page recycle];
        [KerUIPages removeObjectForKey:key];
    }
}

+(void) openPageViewController:(KerId) pageId animated:(BOOL) animated {
    KerPage * page = [self getPage:pageId];
    if(page == nil) {
        return;
    }
    if([page.type isEqualToString:@"window"]) {
        KerPageWindowController * viewController = [[KerPageWindowController alloc] initWithPageId:pageId];
        [viewController showAnimated:animated];
    } else {
        KerPageViewController * viewController = [[KerPageViewController alloc] initWithPageId:pageId];
        UIViewController * topViewController = [self rootViewController];
        if([topViewController isKindOfClass:[UINavigationController class]]) {
            [(UINavigationController *) topViewController pushViewController:viewController animated:animated];
        } else {
            [topViewController presentViewController:viewController animated:animated completion:nil];
        }
    }
}

+(void) popPageViewController:(NSUInteger)delta animated:(BOOL) animated {
    
    UIViewController * topViewController = [self rootViewController];
    
    if([topViewController isKindOfClass:[UINavigationController class]]) {
        NSArray * viewControllers = [(UINavigationController *) topViewController viewControllers];
        if(delta + 1 < [viewControllers count]) {
            [(UINavigationController *) topViewController popToViewController:[viewControllers objectAtIndex:[viewControllers count] - delta - 1] animated:animated];
        } else {
            [(UINavigationController *) topViewController popToRootViewControllerAnimated:animated];
        }
        
    } else {
        
        while([topViewController presentingViewController]) {
            topViewController = [topViewController presentingViewController];
        }
        
        while([topViewController presentedViewController] && delta > 0) {
            
            UIViewController * v = [topViewController presentedViewController];
            
            [topViewController dismissViewControllerAnimated:delta == 1 && animated completion:nil];
            
            topViewController = v;
            delta --;
        }
    }
}

+(UIViewController * ) rootViewController {
    return [[UIApplication sharedApplication] keyWindow].rootViewController;
}

static NSMutableDictionary * gKerUIViewClass = nil;

+(void) setViewClass:(Class) viewClass name:(NSString *) name {
    if(gKerUIViewClass == nil) {
        gKerUIViewClass = [[NSMutableDictionary alloc] initWithCapacity:4];
    }
    gKerUIViewClass[name] = viewClass;
    kk::ui::ViewElement::library([name UTF8String]);
}

+(void) setTextViewClass:(Class) viewClass name:(NSString *) name {
    if(gKerUIViewClass == nil) {
        gKerUIViewClass = [[NSMutableDictionary alloc] initWithCapacity:4];
    }
    gKerUIViewClass[name] = viewClass;
    kk::ui::TextElement::library([name UTF8String]);
}

+(void) setPageViewClass:(Class) viewClass name:(NSString *) name {
    if(gKerUIViewClass == nil) {
        gKerUIViewClass = [[NSMutableDictionary alloc] initWithCapacity:4];
    }
    gKerUIViewClass[name] = viewClass;
    kk::ui::PageElement::library([name UTF8String]);
}

+(void) setImageViewClass:(Class) viewClass name:(NSString *) name {
    if(gKerUIViewClass == nil) {
        gKerUIViewClass = [[NSMutableDictionary alloc] initWithCapacity:4];
    }
    gKerUIViewClass[name] = viewClass;
    kk::ui::ImageElement::library([name UTF8String]);
}

+(void) createView:(kk::ui::ViewCreateCommand *) command app:(KerId) appid {
    
    NSString * name = [NSString stringWithCString:command->name.c_str() encoding:NSUTF8StringEncoding];
    
    Class isa = [gKerUIViewClass valueForKey:name];
    
    if(isa == nil) {
        isa = NSClassFromString(name);
    }
    
    if(isa == nil) {
        isa = [UIView class];
    }
    
    UIView * view = [isa KerViewCreateWithConfiguration:(kk::ui::ViewConfiguration *) command->configuration app:appid];
    
    [view KerViewObtain:command->viewId app:appid];
    
    if(KerUIViews == nil) {
        KerUIViews = [[NSMutableDictionary alloc] initWithCapacity:4];
    }
    
    [KerUIViews setObject:view forKey:@(command->viewId)];
    
}

+(void) createNativeView:(UIView *) view viewId:(kk::Uint64) viewId app:(KerId) appid {
    
    [view KerViewObtain:viewId app:appid];
    
    if(KerUIViews == nil) {
        KerUIViews = [[NSMutableDictionary alloc] initWithCapacity:4];
    }
    
    [KerUIViews setObject:view forKey:@(viewId)];
    
}

+(void) deleteView:(kk::Uint64) viewId app:(KerId) appid {
    
    if(KerUIViews == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [KerUIViews objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    [view KerViewRecycle:viewId app:appid];
    
    [KerUIViews removeObjectForKey:key];
    
}

+(void) setAttribute:(kk::Uint64) viewId name:(NSString *) name value:(NSString *) value app:(KerId) appid {
    
    if(KerUIViews == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [KerUIViews objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    [view KerView:viewId setAttribute:name value:value app:appid];
    
}

+(void) setFrame:(kk::Uint64) viewId frame:(CGRect) frame app:(KerId) appid {
    
    if(KerUIViews == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [KerUIViews objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    [view setFrame:frame];
    
}

+(void) setContentSize:(kk::Uint64) viewId size:(CGSize) size app:(KerId) appid {
    
    if(KerUIViews == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [KerUIViews objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    if([view isKindOfClass:[UIScrollView class]]) {
        [(UIScrollView *) view setContentSize:size];
    }
    
}

+(void) setContentOffset:(kk::Uint64) viewId offset:(CGPoint) offset animated:(BOOL) animated app:(KerId) appid {
    
    if(KerUIViews == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [KerUIViews objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    if([view isKindOfClass:[UIScrollView class]]) {
        [(UIScrollView *) view setContentOffset:offset animated:animated];
    }
    
    
}

+(void) setContent:(kk::Uint64) viewId content:(NSString *) content contentType:(NSString *) contentType basePath:(NSString * )basePath app:(KerId) appid {
    
    if(KerUIViews == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [KerUIViews objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    [view KerView:viewId setContent:content contentType:contentType basePath:basePath app:appid];
}

+(void) setImage:(kk::Uint64) viewId image:(UIImage *) image app:(KerId) appid {
    
    if(KerUIViews == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [KerUIViews objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    if([view isKindOfClass:[UIImageView class]]) {
        [(UIImageView *) view setImage:image];
    } else {
        view.layer.contents = (id) [image CGImage];
    }
}

+(void) evaluateJavaScript:(kk::Uint64) viewId code:(NSString *) code app:(KerId) appid {
    
    if(KerUIViews == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [KerUIViews objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    if([view isKindOfClass:[UIWebView class]]) {
        [(UIWebView *) view stringByEvaluatingJavaScriptFromString:code];
    } else if([view isKindOfClass:[WKWebView class]]) {
        [(WKWebView *) view evaluateJavaScript:code completionHandler:nil];
    }
    
}

+(void) setAttributedString:(kk::Uint64) viewId string:(NSAttributedString *) string app:(KerId) appid {
    
    if(KerUIViews == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [KerUIViews objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    if([view isKindOfClass:[UILabel class]]) {
        [(UILabel *) view setAttributedText:string];
    } else if([view isKindOfClass:[UITextView class]]) {
        [(UITextView *) view setAttributedText:string];
    }
}

+(void) addSubview:(kk::Uint64) viewId subviewId:(kk::Uint64) subviewId position:(kk::ui::SubviewPosition) position app:(KerId) appid {
    
    if(KerUIViews == nil) {
        return ;
    }
    
    UIView * view = [KerUIViews objectForKey:@(viewId)];
    
    if(view == nil) {
        return;
    }
    
    if([view isKindOfClass:[UIScrollView class]]) {
        NSLog(@"");
    }
    
    view = [view KerViewContentView];
    
    if(view == nil) {
        return;
    }
    
    UIView * subview = [KerUIViews objectForKey:@(subviewId)];
    
    if(subview == nil) {
        return;
    }
    
    
    if(position == kk::ui::SubviewPositionBack) {
        [view insertSubview:subview atIndex:0];
    } else {
        [view addSubview:subview];
    }
    
}

+(void) removeView:(kk::Uint64) viewId app:(KerId) appid {
    
    if(KerUIViews == nil) {
        return ;
    }
    
    id key = @(viewId);
    
    UIView * view = [KerUIViews objectForKey:key];
    
    if(view == nil) {
        return;
    }
    
    [view removeFromSuperview];
}

+(void) execViewCommand:(kk::ui::ViewCommand *) command app:(KerId) app {
    
    {
        kk::ui::ViewCreateCommand * v = dynamic_cast<kk::ui::ViewCreateCommand *>(command);
        
        if(v != nullptr) {
            
            v->retain();

            dispatch_async(dispatch_get_main_queue(), ^{
                [KerUI createView:v app:app];
                v->release();
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewNativeCreateCommand * v = dynamic_cast<kk::ui::ViewNativeCreateCommand *>(command);
        
        if(v != nullptr) {
            
            @autoreleasepool {
                
                kk::Uint64 viewId = v->viewId;
                UIView * view = (__bridge UIView *) v->native;
                
                dispatch_async(dispatch_get_main_queue(), ^{
                    [KerUI createNativeView:view viewId:viewId app:app];
                });
                
            }
            
            return;
        }
    }
    
    {
        kk::ui::ViewDeleteCommand * v = dynamic_cast<kk::ui::ViewDeleteCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [KerUI deleteView:viewId app:app];
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

            dispatch_async(dispatch_get_main_queue(), ^{
                [KerUI setAttribute:viewId name:name value:value app:app];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewSetFrameCommand * v = dynamic_cast<kk::ui::ViewSetFrameCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            CGRect frame = CGRectMake(v->frame.origin.x, v->frame.origin.y, v->frame.size.width, v->frame.size.height);

            dispatch_async(dispatch_get_main_queue(), ^{
                [KerUI setFrame:viewId frame:frame app:app];
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
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [KerUI addSubview:viewId subviewId:subviewId position:position app:app];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewRemoveCommand * v = dynamic_cast<kk::ui::ViewRemoveCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;

            dispatch_async(dispatch_get_main_queue(), ^{
                [KerUI removeView:viewId app:app];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewSetContentSizeCommand * v = dynamic_cast<kk::ui::ViewSetContentSizeCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            CGSize size = CGSizeMake(v->size.width, v->size.height);
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [KerUI setContentSize:viewId size:size app:app];
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
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [KerUI setContentOffset:viewId offset:offset animated:animated app:app];
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
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [KerUI setContent:viewId content:content contentType:contentType basePath:basePath app:app];
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

            dispatch_async(dispatch_get_main_queue(), ^{
                [KerUI setImage:viewId image:image app:app];
            });
            
            return;
        }
    }
    
    {
        kk::ui::ViewEvaluateJavaScriptCommand * v = dynamic_cast<kk::ui::ViewEvaluateJavaScriptCommand *>(command);
        
        if(v != nullptr) {
            
            kk::Uint64 viewId = v->viewId;
            
            NSString * code = [NSString stringWithCString:v->code.c_str() encoding:NSUTF8StringEncoding];

            dispatch_async(dispatch_get_main_queue(), ^{
                [KerUI evaluateJavaScript:viewId code:code app:app];
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
                string = kk::ui::GetAttributedText(v->text,(kk::ui::Context *) app);
            }

            dispatch_async(dispatch_get_main_queue(), ^{
                [KerUI setAttributedString:viewId string:string app:app];
            });
            
            return;
        }
    }
    
}

+(void) execCanvasCommand:(kk::ui::CanvasCommand *) command app:(KerId) app {
 
    {
        kk::ui::CanvasDisplayCGContextCommand * cmd = dynamic_cast<kk::ui::CanvasDisplayCGContextCommand *>(command);
        
        if(cmd) {
            
            CGImageRef image = kk::ui::createCGImageWithCGContext(cmd->context);
            
            if(image != nil) {
                kk::Uint64 viewId = cmd->viewId;
                UIImage * i = [UIImage imageWithCGImage:image];
                CGImageRelease(image);
                dispatch_async(dispatch_get_main_queue(), ^{
                    [KerUI setImage:viewId image:i app:app];
                });
            }
            
            return ;
        }
    }
    
}

+(void) execPageCommand:(kk::ui::PageCommand *) command app:(KerId) appid {
    
    {
        kk::ui::PageSetOptionsCommand * v = dynamic_cast<kk::ui::PageSetOptionsCommand *>(command);
        if(v != nullptr) {
            @autoreleasepool {
                id options = KerObjectFromObject((kk::Object *) v->data);
                kk::Uint64 pageId = v->pageId;
                dispatch_async(dispatch_get_main_queue(), ^{
                    KerPage * page = [KerUI getPage:pageId];
                    if(page) {
                        [page setOptions:options];
                    }
                });
            }
            return;
        }
    }
    
    {
        kk::ui::PageCloseCommand * v = dynamic_cast<kk::ui::PageCloseCommand *>(command);
        if(v != nullptr) {
            @autoreleasepool {
                BOOL animated = v->animated;
                kk::Uint64 pageId = v->pageId;
                dispatch_async(dispatch_get_main_queue(), ^{
                    KerPage * page = [KerUI getPage:pageId];
                    if(page) {
                        [page close:animated];
                    }
                });
            }
            return;
        }
    }
    
}

+(void) app:(KerId) appid setContentOffset:(CGPoint) offset viewId:(KerId) viewId {
    kk::Strong<kk::ui::ViewSetContentOffsetCommand> cmd = new kk::ui::ViewSetContentOffsetCommand();
    cmd->viewId = viewId;
    cmd->offset.x = offset.x;
    cmd->offset.y = offset.y;
    kk::ui::UI::main()->dispatchCommand(appid, cmd);
}

+(void) app:(KerId) appid emit:(NSString *) name viewId:(KerId) viewId data:(id) data {
    
    kk::Strong<kk::Event> e = new kk::Event();
    
    if(data != nil) {
        e->setData(new kk::NativeValue((__bridge kk::Native *) data));
    }
    
    kk::ui::UI::main()->emit(appid,viewId, [name UTF8String], e);
    
}


+(dispatch_queue_t) IOQueue {
    return kk::DispatchQueueGCD(kk::IODispatchQueue());
}

+(dispatch_queue_t) UIQueue {
    return kk::DispatchQueueGCD(kk::ui::UI::main()->queue());
}

+(NSString *) mimeType:(NSString *) filePath data:(NSData *) data defaultType:(NSString *) defaultType {
    
    NSString * mimeType = nil;
    
    if(mimeType == nil) {
        
        CFStringRef uti = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, (__bridge CFStringRef) filePath.pathExtension, nil);
        
        CFStringRef v = UTTypeCopyPreferredTagWithClass(uti,kUTTagClassMIMEType);
        
        mimeType = (__bridge NSString *) v ;
        
        CFRelease(uti);
        CFRelease(v);
        
    }
    
    if(mimeType == nil && data) {
        
        uint8_t c;
        
        [data getBytes:&c length:1];
        
        switch (c) {
            case 0xFF:
                mimeType = @"image/jpeg";
                break;
            case 0x89:
                mimeType = @"image/png";
                break;
            case 0x47:
                mimeType = @"image/gif";
                break;
            case 0x49:
            case 0x4D:
                mimeType = @"image/tiff";
                break;
        }
    }
    
    if(mimeType == nil) {
        mimeType = defaultType;
    }
    
    return mimeType;
    
}

+(NSString *) resolveURI:(NSString *) path {
    if(path == nil) {
        return nil;
    }
    kk::String v = kk::ResolveURI([path UTF8String]);
    return [NSString stringWithCString:v.c_str() encoding:NSUTF8StringEncoding];
}

+(NSString *) resolvePath:(NSString *) URI {
    if(URI == nil) {
        return nil;
    }
    kk::String v = kk::ResolvePath([URI UTF8String]);
    return [NSString stringWithCString:v.c_str() encoding:NSUTF8StringEncoding];
}

+(NSString *) resolvePath:(NSString *) URI basePath:(NSString *) basePath {
    if([URI containsString:@"://"]) {
        return [self resolvePath:URI];
    } else if(basePath != nil) {
        return [self resolveURI:[basePath stringByAppendingPathComponent:URI]];
    }
    return [self resolvePath:URI];
}

+(NSString *) resolvePath:(NSString *) URI app:(KerId) appid {
    return [self resolvePath:URI basePath:[self basePathWithApp:appid]];
}

+(NSString *) encodeURL:(NSString *) v {
    if(v == nil) {
        return nil;
    }
    kk::String s = kk::URI::encodeURL([v UTF8String]);
    return [NSString stringWithCString:s.c_str() encoding:NSUTF8StringEncoding];
}

+(NSString *) decodeURL:(NSString *) v {
    if(v == nil) {
        return nil;
    }
    kk::String s = kk::URI::decodeURL([v UTF8String]);
    return [NSString stringWithCString:s.c_str() encoding:NSUTF8StringEncoding];
}

+(NSString *) basePathWithApp:(KerId) appid {
    kk::String s;
    kk::ui::UI::main()->queue()->sync([&s,appid]()->void{
        kk::Strong<kk::ui::App> app = kk::ui::UI::main()->getApp(appid);
        if(app != nullptr) {
            s = app->basePath();
        }
    });
    return [NSString stringWithCString:s.c_str() encoding:NSUTF8StringEncoding];
}
                 
 +(void) open:(NSString *) URI query:(NSDictionary<NSString *,NSString *> *) query callback:(KerUIOpenCallback) callback {

     kk::Strong<kk::NativeObject> cb = new kk::NativeObject((__bridge kk::Native *) callback);
     kk::Strong<kk::NativeValue> q = new kk::NativeValue((__bridge kk::Native *) query);
     kk::ui::UI::main()->open([URI UTF8String], q,[cb](kk::Uint64 appid,kk::CString errmsg)->void{
         
         @autoreleasepool {
             
             NSString * e = errmsg == nullptr ? nil : [NSString stringWithCString:errmsg encoding:NSUTF8StringEncoding];
             KerUIOpenCallback callback = (__bridge KerUIOpenCallback) cb.operator->()->native();
             
             dispatch_async(dispatch_get_main_queue(), ^{
                 if(callback) {
                     callback(appid,e);
                 }
             });
             
         }
         
     });
 }

static NSString * gKerAppUserAgent = nil;

+(void) openlib {
    
    if(gKerAppUserAgent == nil) {
        @autoreleasepool {
            UIWebView * view = [[UIWebView alloc] init];
            gKerAppUserAgent = [[view stringByEvaluatingJavaScriptFromString:@"navigator.userAgent"] stringByAppendingFormat:@" Ker/%s",KER_VERSION_STR];
        }
    }
    
    [self setViewClass:[KerView class] name:@"view"];
    [self setPageViewClass:[KerView class] name:@"page"];
    [self setImageViewClass:[UIImageView class] name:@"image"];
    [self setViewClass:[UIScrollView class] name:@"scroll"];
    [self setViewClass:[WKWebView class] name:@"webview"];
    [self setTextViewClass:[UILabel class] name:@"text"];
    [self setTextViewClass:[UITextView class] name:@"textarea"];
    [self setTextViewClass:[UITextField class] name:@"input"];
    
    [KerURLProtocol openlibs];
    
    kk::ui::App::Openlib();
    kk::ui::addAppOpenlib([](duk_context * ctx, kk::ui::App * app)->void{
        
        @autoreleasepool {
            
            UIScreen * mainScreen = [UIScreen mainScreen];
            
            CGFloat width = MIN(mainScreen.bounds.size.width,mainScreen.bounds.size.height);
            CGFloat height = MAX(mainScreen.bounds.size.width,mainScreen.bounds.size.height);
            
            duk_push_object(ctx);
            
            duk_push_number(ctx, (duk_double_t) width);
            duk_put_prop_string(ctx, -2, "width");
            
            duk_push_number(ctx, (duk_double_t) height);
            duk_put_prop_string(ctx, -2, "height");
            
            duk_push_number(ctx, 1);
            duk_put_prop_string(ctx, -2, "density");
            
            duk_push_number(ctx, mainScreen.scale);
            duk_put_prop_string(ctx, -2, "scale");
            
            duk_put_global_string(ctx, "screen");
            
        }
        
    });
    
    [KerAudio openlib];
    
}


+(void) setUserAgent:(NSString *)v {
    gKerAppUserAgent = v;
}

+(NSString *) userAgent {
    return gKerAppUserAgent;
}

@end
