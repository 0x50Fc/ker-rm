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

@interface KerApp() {
    kk::ui::App * _app;
}

@end


@implementation KerApp

+(void) openlib {
    kk::ui::App::Openlib();
}

-(instancetype) initWithBasePath:(NSString *) basePath {
    if((self = [super init])) {
        _basePath = basePath;
        _app = new kk::ui::App([basePath UTF8String],"iOS");
        _app->retain();
        
        CFTypeRef app = (__bridge CFTypeRef) self;
        
        _app->on("open", new kk::TFunction<void,kk::Event *>([app](kk::Event * event)->void{
            
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
        
        _app->on("back", new kk::TFunction<void,kk::Event *>([app](kk::Event * event)->void{
            
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

-(void) open:(NSString *) uri animated:(BOOL) animated {
    
    if([uri containsString:@"://"]) {
        
        
    } else {
        
        NSMutableDictionary<NSString *,NSString *> *query = [[NSMutableDictionary<NSString *,NSString *> alloc] initWithCapacity:4];
        
        NSRange i = [uri rangeOfString:@"?"];
        
        NSString * path = uri;
        
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
            path = [uri substringToIndex:i.location];
        }
        
        [self openPageViewController:path animated:animated query:query];
        
    }
    
}

-(void) openPageViewController:(NSString *) path animated:(BOOL) animated query:(NSDictionary<NSString *,NSString *> *) query {
    
    KerPageViewController * viewController = [[KerPageViewController alloc] initWithNibName:nil bundle:nil];
    
    viewController.app = self;
    viewController.path = path;
    viewController.query = query;
    
    [self openViewController:viewController animated:animated];
    
}

-(void) openPageWindow:(NSString *) path animated:(BOOL) animated query:(NSDictionary<NSString *,NSString *> *) query {
    
}

-(void) openApp:(NSString *) url animated:(BOOL) animated query:(NSDictionary<NSString *,NSString *> *) query {
    
}

-(void) openViewController:(UIViewController *) viewController animated:(BOOL) animated {
    
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

@end
