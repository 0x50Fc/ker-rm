//
//  KerApp.m
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KerApp.h"
#include <ui/ui.h>
#include <ui/app.h>
#include <ui/view.h>
#include <ui/page.h>
#import "KerPageViewController.h"
#import "KerPageWindowController.h"
#import "KerObject.h"
#import <CommonCrypto/CommonCrypto.h>
#import "KerURLProtocol.h"
#import <MobileCoreServices/MobileCoreServices.h>

@protocol KerAppWKBrowsingContextController  <NSObject>

-(void) registerSchemeForCustomProtocol:(NSString *) scheme;

@end

namespace kk {
    extern ::dispatch_queue_t DispatchQueueGCD(DispatchQueue * queue);
}

@interface KerApp() {
    kk::ui::App * _app;
}

@end


@implementation KerApp

+(void) openlib {
    
    [NSURLProtocol registerClass:[KerURLProtocol class]];
    
    {
        Class cls = NSClassFromString(@"WKBrowsingContextController");
        SEL sel = @selector(registerSchemeForCustomProtocol:);
        if([cls respondsToSelector:sel]) {
            [(id<KerAppWKBrowsingContextController>)cls registerSchemeForCustomProtocol:@"ker-tmp"];
            [(id<KerAppWKBrowsingContextController>)cls registerSchemeForCustomProtocol:@"ker-data"];
        }
    }
    
    kk::ui::App::Openlib();
}

static NSString * gKerAppUserAgent = nil;

+(void) setUserAgent:(NSString *)v {
    gKerAppUserAgent = v;
}

+(NSString *) userAgent {
    if(gKerAppUserAgent == nil) {
        UIWebView * view = [[UIWebView alloc] init];
        gKerAppUserAgent = [view stringByEvaluatingJavaScriptFromString:@"navigator.userAgent"];
    }
    return gKerAppUserAgent;
}

-(instancetype) initWithBasePath:(NSString *) basePath appkey:(NSString *) appkey {
    if((self = [super init])) {
        _appkey = appkey;
        _basePath = basePath;
        _dataPath = [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents/ker/"] stringByAppendingString:appkey];
        [[NSFileManager defaultManager] createDirectoryAtPath:_dataPath withIntermediateDirectories:YES attributes:nil error:nil];
        
        _app = new kk::ui::App([basePath UTF8String],"iOS",[[KerApp userAgent] UTF8String],[appkey UTF8String]);
        _app->retain();
        
        CFTypeRef app = (__bridge CFTypeRef) self;
        
        _app->on("open", new kk::TFunction<void,kk::CString,kk::Event *>([app](kk::CString name,kk::Event * event)->void{
            
            @autoreleasepool {
                KerApp * a = (__bridge KerApp *) app;
                kk::TObject<kk::String, kk::Any> * data = (kk::TObject<kk::String, kk::Any> *) event->data();
                kk::CString uri = (*data)["uri"];
                kk::Boolean animated = true;
                
                if(data->find("animated") != data->end()) {
                    animated = (*data)["animated"];
                }
                
                [a open:[NSString stringWithCString:uri encoding:NSUTF8StringEncoding] animated:animated];
            }
            
        }));
        
        _app->on("back", new kk::TFunction<void,kk::CString,kk::Event *>([app](kk::CString name,kk::Event * event)->void{
            
            @autoreleasepool {
                KerApp * a = (__bridge KerApp *) app;
                kk::TObject<kk::String, kk::Any> * data = (kk::TObject<kk::String, kk::Any> *) event->data();
                kk::Uint delta = (*data)["delta"];
                kk::Boolean animated = true;
                
                if(data->find("animated") != data->end()) {
                    animated = (*data)["animated"];
                }
                
                [a back:(NSUInteger) delta animated:animated];
            }
            
        }));
        
    }
    return self;
}

-(void) dealloc {
    if(_app != nullptr) {
        _app->off();
        _app->release();
    }
}

-(KerAppCPointer) CPointer {
    return _app;
}

-(UIViewController * ) rootViewController {
    if(_rootViewController == nil) {
        _rootViewController = [[UIApplication sharedApplication] keyWindow].rootViewController;
    }
    return _rootViewController;
}

-(void) run:(NSDictionary<NSString *,NSString *> *) query {
    
    if(_app == nullptr) {
        return;
    }
    
    kk::ui::App * app = (kk::ui::App *) _app;
    kk::Strong<kk::TObject<kk::String, kk::Any>> librarys = new kk::TObject<kk::String, kk::Any>();
    NSEnumerator * keyEnum = [query keyEnumerator];
    NSString * key;
    while((key = [keyEnum nextObject])) {
        NSString * value = [query valueForKey:key];
        (*librarys)[[key UTF8String]] = (kk::Any) [value UTF8String];
    }
    
    app->exec("main.js", (kk::TObject<kk::String, kk::Any> *) librarys);
    
}

-(void) back:(NSUInteger)delta animated:(BOOL) animated {
    
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

+(NSDictionary *) queryWithURI:(NSString *) uri {
    
    NSMutableDictionary *query = [NSMutableDictionary dictionaryWithCapacity:4];
    
    NSRange i = [uri rangeOfString:@"?"];
    
    if(i.location != NSNotFound) {
        NSArray * vs = [[uri substringFromIndex:i.location + i.length] componentsSeparatedByString:@"&"];
        for(NSString * v in vs) {
            NSArray * kv = [v componentsSeparatedByString:@"="];
            if([kv count] > 0) {
                query[kv[0]] = [KerApp decodeURL:kv[1]];
            } else {
                query[kv[0]] = @"";
            }
        }
    }
    
    return query;
}

+(NSString *) pathWithURI:(NSString *) uri {
    
    if(uri == nil) {
        return nil;
    }
    
    NSRange i = [uri rangeOfString:@"://"];
    
    NSString * scheme = nil;
    
    if(i.location != NSNotFound) {
        scheme = [uri substringToIndex:i.location];
        uri = [uri substringFromIndex:i.location + i.length];
    }

    i = [uri rangeOfString:@"?"];
    
    if(i.location != NSNotFound) {
        uri = [uri substringToIndex:i.location];
    }
    
    if([scheme isEqualToString:@"ker-tmp"]) {
        return [NSTemporaryDirectory() stringByAppendingPathComponent:uri];
    }
    
    if([scheme isEqualToString:@"ker-data"]) {
        return [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents/ker"] stringByAppendingPathComponent:uri];
    }
    
    return uri;
}

+(NSString *) filePathWithURI:(NSString *) uri basePath:(NSString *) basePath {
    
    if(uri == nil) {
        return nil;
    }
    
    NSRange i = [uri rangeOfString:@"://"];
    
    NSString * scheme = nil;
    
    if(i.location != NSNotFound) {
        scheme = [uri substringToIndex:i.location];
        uri = [uri substringFromIndex:i.location + i.length];
    }
    
    i = [uri rangeOfString:@"?"];
    
    if(i.location != NSNotFound) {
        uri = [uri substringToIndex:i.location];
    }
    
    if([scheme isEqualToString:@"ker-tmp"]) {
        return [NSTemporaryDirectory() stringByAppendingPathComponent:uri];
    }
    
    if([scheme isEqualToString:@"ker-data"]) {
        return [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents/ker"] stringByAppendingPathComponent:uri];
    }
    
    if(basePath != nil) {
        return [basePath stringByAppendingPathComponent:uri];
    }
    
    return uri;
    
}

-(void) open:(NSString *) uri animated:(BOOL) animated {
    
    if([uri containsString:@"://"]) {
        
        if([uri hasPrefix:@"window://"]) {
            
            NSString * path = [KerApp pathWithURI:uri];
            NSDictionary * query = [KerApp queryWithURI:uri];
            
            [self openPageWindow:path animated:[[query ker_getValue:@"animated"] boolValue] query:query];
        }
        
    } else {
        
        NSString * path = [KerApp pathWithURI:uri];
        NSDictionary * query = [KerApp queryWithURI:uri];
        
        [self openPageViewController:path animated:animated query:query];
        
    }
    
}


-(void) openPageViewController:(NSString *) path animated:(BOOL) animated query:(NSDictionary<NSString *,NSString *> *) query {
    
    if([_delegate respondsToSelector:@selector(KerApp:openPageViewController:animated:query:)]
       && [_delegate KerApp:self openPageViewController:path animated:animated query:query]){
        return;
    }
    
    KerPageViewController * viewController = [[KerPageViewController alloc] initWithNibName:nil bundle:nil];
    
    viewController.app = self;
    viewController.path = path;
    viewController.query = query;
    
    [self openViewController:viewController animated:animated];
    
}

-(void) openPageWindow:(NSString *) path animated:(BOOL) animated query:(NSDictionary<NSString *,NSString *> *) query {
    
    if([_delegate respondsToSelector:@selector(KerApp:openPageWindow:animated:query:)]
       && [_delegate KerApp:self openPageWindow:path animated:animated query:query]) {
        return;
    }
    
    KerPageWindowController * viewController = [[KerPageWindowController alloc] init];
    
    viewController.app = self;
    viewController.path = path;
    viewController.query = query;
    
    [viewController showAnimated:animated];

}

-(void) openApp:(NSString *) url animated:(BOOL) animated query:(NSDictionary<NSString *,NSString *> *) query {
 
}

-(void) openViewController:(UIViewController *) viewController animated:(BOOL) animated {
    
    if([_delegate respondsToSelector:@selector(KerApp:openViewController:animated:)]
       && [_delegate KerApp:self openViewController:viewController animated:animated]) {
        return;
    }
    
    UIViewController * topViewController = [self rootViewController];
    
    if([topViewController isKindOfClass:[UINavigationController class]]) {
        [(UINavigationController *) topViewController pushViewController:viewController animated:animated];
    } else {
        [topViewController presentViewController:viewController animated:animated completion:nil];
    }
    
}

-(void) recycle {
    if(_app != nullptr) {
        _app->off();
        _app->release();
        _app = nullptr;
    }
}


+(NSString *) encodeURL:(NSString *) url {
    
    if(url == nil) {
        return nil;
    }
    
    return [url stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet characterSetWithCharactersInString:@":/?&=;+!@#$()',*"]];
}

+(NSString *) decodeURL:(NSString *) url {
    
    if(url == nil) {
        return nil;
    }
    return [url stringByRemovingPercentEncoding];
}

+(NSString *) keyWithURI:(NSString *) uri {
    
    CC_MD5_CTX ctx;
    
    CC_MD5_Init(&ctx);
    
    CC_MD5_Update(&ctx, [uri UTF8String], (CC_LONG) [uri length]);
    
    unsigned char md[16];
    
    CC_MD5_Final(md, &ctx);
    
    return [NSString stringWithFormat:@"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
         ,md[0],md[1],md[2],md[3],md[4],md[5],md[6],md[7]
         ,md[8],md[9],md[10],md[11],md[12],md[13],md[14],md[15]];
    
}

+(dispatch_queue_t) IOQueue {
    return kk::DispatchQueueGCD(kk::IODispatchQueue());
}

+(NSString *) relativeURI:(NSString *) filePath {
    {
        NSString * basePath = [NSHomeDirectory() stringByAppendingString:@"/Documents/ker/"];
        if([filePath hasPrefix:basePath]) {
            return [NSString stringWithFormat:@"ker-data:///%@",[filePath substringFromIndex:[basePath length]]];
        }
    }
    {
        NSString * basePath = NSTemporaryDirectory();
        if([filePath hasPrefix:basePath]) {
            return [NSString stringWithFormat:@"ker-tmp:///%@",[filePath substringFromIndex:[basePath length]]];
        }
    }
    return [NSString stringWithFormat:@"file://%@",filePath];
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

@end
