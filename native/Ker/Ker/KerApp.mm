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
#include "unzip.h"
#import "KerView.h"
#import "KerPage.h"
#import "KerWeak.h"

namespace kk {
    extern ::dispatch_queue_t DispatchQueueGCD(DispatchQueue * queue);
}

@interface KerApp() {
    kk::ui::App * _app;
    KerView * _view;
}

-(void) execViewCommand:(kk::ui::ViewCommand *) command;
-(void) execPageCommand:(kk::ui::PageCommand *) command;

@end


@implementation KerApp

+(void) openlib {
    
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
}

static NSString * gKerAppUserAgent = nil;

+(void) setUserAgent:(NSString *)v {
    gKerAppUserAgent = v;
}

+(NSString *) userAgent {
    if(gKerAppUserAgent == nil) {
        @autoreleasepool {
            UIWebView * view = [[UIWebView alloc] init];
            gKerAppUserAgent = [view stringByEvaluatingJavaScriptFromString:@"navigator.userAgent"];
        }
    }
    return gKerAppUserAgent;
}

-(void) execViewCommand:(kk::ui::ViewCommand *) command {
    [_view execCommand:command];
}

-(void) execPageCommand:(kk::ui::PageCommand *) command {
    
    kk::Strong<kk::ui::Page> page = _app->getPage(command->pageId);
    
    if(page != nullptr) {
        
        KerWeak * v = (__bridge KerWeak *) page->native();
        
        if(v != nil) {
            KerPage * p = (KerPage *) v.object;
            
            if(p != nil) {
            
                {
                    kk::ui::PageSetOptionsCommand * cmd = dynamic_cast<kk::ui::PageSetOptionsCommand *>(command);
                    
                    if(cmd != nullptr) {
                        id options = KerObjectFromObject((kk::Object *) cmd->data);
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [p setOptions:options];
                        });
                        return;
                    }
                }
                
                {
                    kk::ui::PageCloseCommand * cmd = dynamic_cast<kk::ui::PageCloseCommand *>(command);
                    
                    if(cmd != nullptr) {
                        BOOL animated = cmd->animated;
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [p close:animated];
                        });
                        return;
                    }
                }
                
            }
        }
        
    }
    
}

-(instancetype) initWithBasePath:(NSString *) basePath appkey:(NSString *) appkey {
    if((self = [super init])) {
        _view = [[KerView alloc] initWithApp:self];
        _appkey = appkey;
        _basePath = basePath;
        _dataPath = [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents/ker/"] stringByAppendingString:appkey];
        [[NSFileManager defaultManager] createDirectoryAtPath:_dataPath withIntermediateDirectories:YES attributes:nil error:nil];
        
        _app = new kk::ui::App([basePath UTF8String],"iOS",[[KerApp userAgent] UTF8String],[appkey UTF8String]);
        _app->retain();
        
        CFTypeRef native = (__bridge CFTypeRef) self;
        
        _app->setOnCommand([native](kk::ui::App * app,kk::ui::Command * command)->void{
            
            {
                kk::ui::AppOpenCommand * cmd = dynamic_cast<kk::ui::AppOpenCommand *>(command);
                if(cmd) {
                    
                    @autoreleasepool {
                        
                        KerApp * a = (__bridge KerApp *) native;
                        
                        NSString * u = [NSString stringWithCString:cmd->uri.c_str() encoding:NSUTF8StringEncoding];
                        BOOL animated = cmd->animated;
                        
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [a open:u animated:animated];
                        });
                        
                    }
                    
                    return;
                }
            }
            
            
            {
                kk::ui::AppBackCommand * cmd = dynamic_cast<kk::ui::AppBackCommand *>(command);
                
                if(cmd) {
                    
                    @autoreleasepool {
                        
                        __weak KerApp * a = (__bridge KerApp *) native;
                        
                        NSUInteger delta = cmd->delta;
                        BOOL animated = cmd->animated;
                        
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [a back:delta animated:animated];
                        });
                        
                    }
                    
                    return;
                }
            }
            
            {
                kk::ui::ViewCommand * cmd = dynamic_cast<kk::ui::ViewCommand *>(command);
                
                if(cmd) {
                    
                    @autoreleasepool {
                        
                        __weak KerApp * a = (__bridge KerApp *) native;
                        
                        [a execViewCommand:cmd];
                        
                    }
                    
                    return;
                }
            }
            
            {
                kk::ui::PageCommand * cmd = dynamic_cast<kk::ui::PageCommand *>(command);
                
                if(cmd) {
                    
                    @autoreleasepool {
                        
                        __weak KerApp * a = (__bridge KerApp *) native;
                        
                        [a execPageCommand:cmd];
                        
                    }
                    
                    return;
                }
            }
            
            
        });
        
    }
    return self;
}

-(void) dealloc {
    if(_app != nullptr) {
        kk::ui::App * app = (kk::ui::App *) _app;
        _app = nullptr;
        app->setOnCommand(nullptr);
        app->queue()->async([app]()->void{
            app->off();
            app->release();
            kk::Zombies * z = kk::Zombies::current();
            if(z != nullptr) {
                z->dump();
            }
        });
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

    kk::Strong<kk::TObject<kk::String, kk::Any>> v = new kk::TObject<kk::String, kk::Any>({{"query",(__bridge kk::Native *) query}});
    
    CFTypeRef a = CFBridgingRetain(self);
    
    app->queue()->async([v,app,a]()->void{
        app->exec("main.js", v.operator->());
        CFRelease(a);
    });

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
    
    if([scheme isEqualToString:@"ker-app"]) {
        return [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents/ker-app"] stringByAppendingPathComponent:uri];
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
    
    if([scheme isEqualToString:@"ker-app"]) {
        return [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents/ker-app"] stringByAppendingPathComponent:uri];
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
        kk::ui::App * app = (kk::ui::App *) _app;
        _app = nullptr;
        app->setOnCommand(nullptr);
        app->queue()->async([app]()->void{
            app->off();
            app->release();
            kk::Zombies * z = kk::Zombies::current();
            if(z != nullptr) {
                z->dump();
            }
        });
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
    {
        NSString * basePath = [NSHomeDirectory() stringByAppendingString:@"/Documents/ker-app/"];
        if([filePath hasPrefix:basePath]) {
            return [NSString stringWithFormat:@"ker-app:///%@",[filePath substringFromIndex:[basePath length]]];
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

+(NSMutableDictionary *) getPackageCache {
    static NSMutableDictionary * v = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        v = [[NSMutableDictionary alloc] initWithCapacity:4];
    });
    return v;
}

+(BOOL) addPackageCallback:(KerAppGetPackageCallback) callback URI:(NSString *) URI {
    
    BOOL r = YES;
    
    NSMutableDictionary * v = [self getPackageCache];
    
    @synchronized (v) {
        
        NSMutableSet * items = [v valueForKey:URI];
        
        if(items == nil) {
            items = [[NSMutableSet alloc] initWithCapacity:4];
            [v setValue:items forKey:URI];
            r = NO;
        }
        
        [items addObject:callback];
        
    }
    
    return r;
}

+(NSSet *) clearPackageCallbackWithURI:(NSString *) URI {
    
    NSMutableDictionary * v = [self getPackageCache];
    
    @synchronized (v) {
        
        NSMutableSet * items = [v valueForKey:URI];
        
        if(items != nil) {
            [v removeObjectForKey:URI];
            return items;
        }
    
    }
    
    return nil;
}

+(void) getPackage:(NSString *) URI basePath:(NSString **) basePath appkey:(NSString **) appkey {
    
    if(URI == nil) {
        * basePath = nil;
        * appkey = nil;
        return;
    }
    
    if([URI hasPrefix:@"ker-tmp://"]) {
        * basePath = [NSTemporaryDirectory() stringByAppendingPathComponent:[URI substringFromIndex:10]];
        * appkey = [self keyWithURI: * basePath];
    } else if([URI hasPrefix:@"ker-data://"]) {
        * basePath = [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents/ker"] stringByAppendingPathComponent:[URI substringFromIndex:11]];
        * appkey = [self keyWithURI: * basePath];
    } else if([URI hasPrefix:@"http://"] || [URI hasPrefix:@"https://"]) {
        * appkey = [self keyWithURI:URI];
        * basePath = [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents/ker-app"] stringByAppendingPathComponent:* appkey];
    } else if([URI containsString:@"://"]) {
        * basePath = nil;
        * appkey = nil;
        return;
    } else {
        * basePath = URI;
        * appkey = [self keyWithURI: * basePath];
    }
    
}

+(void) getPackage:(NSString *) URI callback:(KerAppGetPackageCallback) callback queue:(dispatch_queue_t) queue {
    
    NSString * basePath = nil;
    NSString * appkey = nil;
    
    [self getPackage:URI basePath:& basePath appkey:& appkey];
    
    if(appkey == nil) {
        
        dispatch_async(queue, ^{
            callback(basePath,appkey,@"错误的URI");
        });
        
    } else {
        
        NSFileManager * fm = [NSFileManager defaultManager];
        
        if([fm fileExistsAtPath:basePath]) {
            
            dispatch_async(queue, ^{
                callback(basePath,appkey,nil);
            });
            
        } else if([URI hasPrefix:@"http://"] || [URI hasPrefix:@"https://"]) {
            
            if(![self addPackageCallback:callback URI:URI]) {
                
                NSString * errmsg = nil;
                NSURL * u = nil;
                
                @try {
                    u = [NSURL URLWithString:URI];
                }
                @catch(NSException * ex) {
                    @try {
                        u = [NSURL URLWithString:[URI stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLPathAllowedCharacterSet]]];
                    }
                    @catch(NSException * ex) {
                        errmsg = @"错误的URL";
                    }
                }
                
                if(u == nil) {
                    dispatch_async(queue, ^{
                        callback(basePath,appkey,errmsg);
                    });
                } else {
                    
                    NSMutableURLRequest * req = [NSMutableURLRequest requestWithURL:u];
                    
                    NSURLSessionDownloadTask * task = [[NSURLSession sharedSession] downloadTaskWithRequest:req completionHandler:^(NSURL * location, NSURLResponse * response, NSError * error) {
                        
                        if(error != nil) {
                            
                            dispatch_async(queue, ^{
                                callback(basePath,appkey,[error localizedDescription]);
                            });
                            
                        } else {
                            
                            NSString * tBasePath = [basePath stringByAppendingString:@"_unpkg"];
                            
                            unzFile zf = unzOpen([[location path] UTF8String]);
                            
                            if(zf) {
                                
                                [fm createDirectoryAtPath:tBasePath withIntermediateDirectories:YES attributes:nil error:nil];
                                
                                unz_file_info fi;
                                char szFileName[1024];
                                char data[204800];
                                ssize_t n;
                                NSString * errmsg = nil;
                                
                                int err = unzGoToFirstFile(zf);
                                
                                while(err == UNZ_OK) {
                                    
                                    unzGetCurrentFileInfo(zf, &fi, szFileName, sizeof(szFileName), nullptr, 0, nullptr, 0);
                                    
                                    if(kk::CStringHasSuffix(szFileName, "/")) {
                                        NSString * path = [tBasePath stringByAppendingPathComponent:[NSString stringWithCString:szFileName encoding:NSUTF8StringEncoding]];
                                        [fm createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:nil];
                                    } else {
                                        
                                        FILE * fd = fopen([[tBasePath stringByAppendingPathComponent:[NSString stringWithCString:szFileName encoding:NSUTF8StringEncoding]] UTF8String], "wb");
                                        
                                        if(fd) {
                                            
                                            if(unzOpenCurrentFile(zf) == UNZ_OK) {
                                                
                                                while((n = unzReadCurrentFile(zf, data, sizeof(data))) > 0) {
                                                    fwrite(data, 1, n, fd);
                                                }
                                                
                                                unzCloseCurrentFile(zf);
                                            } else {
                                                errmsg = @"解压文件出错";
                                            }
                                            
                                            fclose(fd);
                                        } else {
                                            errmsg = @"无法创建文件";
                                        }
                                        
                                    }
                                    
                                    err = unzGoToNextFile(zf);
                                }
                                
                                unzClose(zf);
                                
                                if(errmsg) {
                                    [fm removeItemAtPath:tBasePath error:nil];
                                    dispatch_async(queue, ^{
                                        callback(basePath,appkey,errmsg);
                                    });
                                } else {
                                    
                                    [fm moveItemAtPath:tBasePath toPath:basePath error:nil];
                                    
                                    dispatch_async(queue, ^{
                                        
                                        for(KerAppGetPackageCallback cb in [KerApp clearPackageCallbackWithURI:URI]) {
                                            cb(basePath,appkey,nil);
                                        }
                                        
                                    });
                                    
                                }
                            } else {
                                
                                dispatch_async(queue, ^{
                                    callback(basePath,appkey,@"错误的程序包");
                                });
                                
                            }
                            
                        }
                        
                    }];
                    
                    [task resume];
                }
                
            }
            
        } else {
            
            dispatch_async(queue, ^{
                callback(basePath,appkey,@"未找到应用");
            });
            
        }
    }
    
}

+(BOOL) isLoadingPackage:(NSString *) URI {
    
    NSMutableDictionary * v = [self getPackageCache];
    
    @synchronized (v) {
        return [v valueForKey:URI] != nil;
    }

    return NO;
}

+(BOOL) hasPackage:(NSString *) URI basePath:(NSString *) basePath appkey:(NSString *) appkey {
    
    if([self isLoadingPackage:URI]) {
        return NO;
    }
    
    
    if(basePath) {
        return [[NSFileManager defaultManager] fileExistsAtPath:basePath];
    }
    return NO;
}

+(void) run:(NSString *) URI query:(NSDictionary<NSString *,NSString *> *) query {
    
    [self getPackage:URI callback:^(NSString *basePath, NSString *appkey, NSString *errmsg) {
    
        if(errmsg != nil) {
            NSLog(@"[Ker] [ERROR] %@: %@",URI,errmsg);
        } else {
            KerApp * app = [[KerApp alloc] initWithBasePath:basePath appkey:appkey];
            [app run:query];
        }
    } queue:dispatch_get_main_queue()];
    
}

-(void) emit:(NSString *) name viewId:(unsigned long long) viewId data:(id) data {
    kk::String n = [name UTF8String];
    kk::Strong<kk::ui::App> app = _app;
    kk::Strong<kk::Event> e = new kk::Event();
    
    if(data != nil) {
        e->setData(new kk::NativeValue((__bridge kk::Native *) data));
    }
    
    app->queue()->async([e,n,viewId,app]()->void{
        kk::Strong<kk::ui::View> view = app->getView(viewId);
        if(view != nullptr) {
            view->emit(n.c_str(), e.operator->());
        }
    });
    
}

-(void) setContentOffset:(CGPoint) offset viewId:(unsigned long long) viewId {
    kk::Strong<kk::ui::ViewSetContentOffsetCommand> cmd = new kk::ui::ViewSetContentOffsetCommand();
    cmd->viewId = viewId;
    cmd->offset.x = offset.x;
    cmd->offset.y = offset.y;
    kk::Strong<kk::ui::App> app = _app;
    app->queue()->async([cmd,app]()->void{
        app->dispatchCommand(cmd.operator->());
    });
}

@end
