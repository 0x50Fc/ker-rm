//
//  WKWebView+KerViewProtocol.m
//  KK
//
//  Created by zhanghailong on 2018/11/8.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "WKWebView+KerViewProtocol.h"
#import "KerURLProtocol.h"
#import "KerObject.h"
#import "KerApp.h"
#include <ui/ui.h>
#include <ui/view.h>
#include <objc/runtime.h>
#import "KerApp.h"

@interface WKWebViewKKViewProtocol : NSObject<WKScriptMessageHandler,WKNavigationDelegate>

@property(nonatomic,assign) kk::Uint64 viewId;
@property(nonatomic,weak) KerApp * app;
@property(nonatomic,assign) kk::ui::WebViewConfiguration * configuration;
@end

@implementation WKWebViewKKViewProtocol

-(void) dealloc {
    if(_configuration != nullptr) {
        _configuration->release();
    }
}

-(void) setConfiguration:(kk::ui::WebViewConfiguration *)configuration {
    if(_configuration != configuration) {
        if(configuration != nullptr) {
            configuration->retain();
        }
        if(_configuration != nullptr) {
            _configuration->release();
        }
        _configuration = configuration;
    }
}

- (void) userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
    
    if(_viewId != 0) {
        [_app emit:@"data" viewId:_viewId data:message.body];
    }
    
}

- (void)webView:(WKWebView *)webView decidePolicyForNavigationAction:(WKNavigationAction *)navigationAction decisionHandler:(void (^)(WKNavigationActionPolicy))decisionHandler {
    
    if(_viewId != 0 && _configuration) {
        
        WKNavigationActionPolicy policy = WKNavigationActionPolicyCancel;
        
        NSString * u = navigationAction.request.URL.absoluteString;
        
        auto m = _configuration->userActions();
        auto i = m.begin();
        
        while(i != m.end()) {
            
            kk::ui::WebViewUserAction & v = * i;
            
            NSRegularExpression * pattern = [NSRegularExpression regularExpressionWithPattern:[NSString stringWithCString:v.pattern.c_str() encoding:NSUTF8StringEncoding] options:NSRegularExpressionAnchorsMatchLines error:nil];
            
            NSTextCheckingResult * r = [pattern firstMatchInString:u options:NSMatchingReportProgress range:NSMakeRange(0, [u length])];
            
            if(r != nil) {
                if(v.policy == kk::ui::WebViewActionPolicyAllow) {
                    policy = WKNavigationActionPolicyAllow;
                }
                break;
            }
            
            
            i ++;
        }
        
        if(i != m.end()) {
            
            [_app emit:@"action" viewId:_viewId data:@{@"url":u}];
            
            decisionHandler(policy);
            
            return;
        }
        
    }
    
    
    decisionHandler(WKNavigationActionPolicyAllow);
    
}


- (void)webView:(WKWebView *)webView didCommitNavigation:(null_unspecified WKNavigation *)navigation {
    
    if(_viewId != 0) {
        [_app emit:@"loading" viewId:_viewId data:nil];
    }
    
}

- (void)webView:(WKWebView *)webView didFinishNavigation:(null_unspecified WKNavigation *)navigation {
    if(_viewId != 0) {
        [_app emit:@"load" viewId:_viewId data:nil];
    }
}

- (void)webView:(WKWebView *)webView didFailNavigation:(null_unspecified WKNavigation *)navigation withError:(NSError *)error {
    if(_viewId != 0) {
        [_app emit:@"error" viewId:_viewId data:@{@"errmsg":[error localizedDescription]}];
    }
}

- (void) observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey,id> *)change context:(void *)context {
    
    if([object isKindOfClass:[WKWebView class]]) {
        if([keyPath isEqualToString:@"estimatedProgress"]) {
            
            if(_viewId != 0) {
                [_app emit:@"progress" viewId:_viewId data:@{@"value":@([(WKWebView *) object estimatedProgress])}];
            }
            
        }
    }
    
}


@end

@implementation WKWebView (KerViewProtocol)

+(instancetype) KerViewCreateWithConfiguration:(KerViewConfigurationCPointer *) config app:(KerApp *) app {
    
    kk::ui::WebViewConfiguration * v = dynamic_cast<kk::ui::WebViewConfiguration *>((kk::ui::ViewConfiguration *) config);
    
    WKWebViewConfiguration * configuration = [[WKWebViewConfiguration alloc] init];
    
    WKUserContentController * userContentController = [[WKUserContentController alloc] init];
    
    WKWebViewKKViewProtocol * object = [[WKWebViewKKViewProtocol alloc] init];
    
    object.configuration = v;
    object.app = app;
    
    [userContentController addScriptMessageHandler:object name:@"ker"];
    
    if(v != nullptr) {
        
        auto & m = v->userScripts();
        
        auto i = m.begin();
        
        while(i != m.end()) {
            
            kk::ui::WebViewUserScript & vv = * i;
            
            WKUserScript * userScript = [[WKUserScript alloc] initWithSource:[NSString stringWithCString:vv.code.c_str() encoding:NSUTF8StringEncoding] injectionTime:(WKUserScriptInjectionTime) vv.injectionTime forMainFrameOnly:YES];
            
            [userContentController addUserScript:userScript];
            
            i ++;
        }
        
    }
    
    configuration.userContentController = userContentController;
    
    [configuration.preferences setJavaScriptCanOpenWindowsAutomatically:YES];
    [configuration.preferences setJavaScriptEnabled:YES];
    [configuration.preferences setMinimumFontSize:0];
    
    @try {
        [configuration.preferences setValue:@TRUE forKey:@"allowFileAccessFromFileURLs"];
    }
    @catch (NSException *exception) {}
    
    @try {
        [configuration setValue:@TRUE forKey:@"allowUniversalAccessFromFileURLs"];
    }
    @catch (NSException *exception) {}

    [KerURLProtocol installWKWebViewConfiguration:configuration];

    WKWebView * view = [[self alloc] initWithFrame:CGRectZero configuration:configuration];
    
    objc_setAssociatedObject(view, "__WKWebViewKerViewProtocol", object, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    [view setNavigationDelegate:object];
    [view setOpaque:NO];
    
    return view;
}

-(void) KerViewObtain:(KerViewId) viewId app:(KerApp *)app {
    [super KerViewObtain:viewId app:app];
    WKWebViewKKViewProtocol * object = (WKWebViewKKViewProtocol *) objc_getAssociatedObject(self, "__WKWebViewKerViewProtocol");
    object.viewId = viewId;
    [self.scrollView KerViewObtain:viewId app:app];
}

-(void) KerViewRecycle:(KerViewId) viewId app:(KerApp *) app {
    [super KerViewRecycle:viewId app:app];
    objc_setAssociatedObject(self, "__WKWebViewKerViewProtocol", nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [self.scrollView KerViewRecycle:viewId app:app];
}

-(UIView *) KerViewContentView {
    return self.scrollView;
}

-(void) KerView:(KerViewId) viewId setAttribute:(NSString *) key value:(NSString *) value app:(KerApp *)app {
    
    [super KerView:viewId setAttribute:key value:value app:app];
    
    if(key == nil) {
        return ;
    }
    
    if([key isEqualToString:@"src"]) {
        
        if(value != nil) {
            
            if([value rangeOfString:@"://"].location == NSNotFound) {
                
                NSString * basePath = app.basePath;
                
                NSURL * baseURL = [NSURL URLWithString:[KerApp relativeURI:basePath]];
                
                [self loadFileURL:[NSURL URLWithString:value relativeToURL:baseURL] allowingReadAccessToURL:baseURL];
                
            } else {
                @try {
                    NSURL * u = [NSURL URLWithString:value];
                    [self loadRequest:[NSURLRequest requestWithURL:u]];
                }
                @catch(NSException * ex) {
                    NSLog(@"[Ker] %@",ex);
                }
            }
        }
        
    } else if([key isEqualToString:@"#text"]) {
        
        if(value != nil) {
            
            NSString * basePath = app.basePath;
            
            NSURL * baseURL = [NSURL URLWithString:[KerApp relativeURI:basePath]];
            
            [self loadHTMLString:value baseURL:baseURL];
            
        }
    }
    
}

-(void) KerView:(KerViewId) viewId setContent:(NSString *) content contentType:(NSString *) contentType basePath:(NSString *) basePath app:(KerApp *)app {
    
    if(content != nil) {
        
        NSString * path = [app basePath];
        
        if([basePath length] != 0) {
            path = [path stringByAppendingPathComponent:basePath];
        }
        
        if(![path hasSuffix:@"/"]) {
            path = [path stringByAppendingString:@"/"];
        }
        
        NSURL * baseURL = [NSURL URLWithString:[KerApp relativeURI:path]];
        
        if([contentType length] == 0) {
            [self loadData:[content dataUsingEncoding:NSUTF8StringEncoding] MIMEType:contentType characterEncodingName:@"utf-8" baseURL:baseURL];
        } else {
            [self loadHTMLString:content baseURL:baseURL];
        }
        
    }
}


@end

