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

@interface WKWebViewKKViewProtocol : NSObject<WKScriptMessageHandler,WKNavigationDelegate>

@end

@implementation WKWebViewKKViewProtocol

- (void) userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
    
    kk::ui::View * view = (__bridge kk::ui::View *) objc_getAssociatedObject(message.webView, "__WKWebViewKerViewProtocol");
    
    if(view != nullptr) {
        
        kk::Strong<kk::Event> e = new kk::Event();
        
        e->setData(new kk::NativeValue((__bridge kk::Native *) message.body));
        
        view->emit("data", e);
        
    }
    
}

- (void)webView:(WKWebView *)webView decidePolicyForNavigationAction:(WKNavigationAction *)navigationAction decisionHandler:(void (^)(WKNavigationActionPolicy))decisionHandler {
    
    kk::ui::View * view = (__bridge kk::ui::View *) objc_getAssociatedObject(webView, "__WKWebViewKerViewProtocol");
    
    if(view != nullptr) {
        
        kk::ui::WebViewConfiguration * config = dynamic_cast<kk::ui::WebViewConfiguration *>( view->configuration() );
        
        if(config != nullptr ){
            
            WKNavigationActionPolicy policy = WKNavigationActionPolicyCancel;
            
            NSString * name = @"action";
            
            NSString * u = navigationAction.request.URL.absoluteString;
            
            auto m = config->userActions();
            auto i = m.begin();
            while(i != m.end()) {
                
                kk::ui::WebViewUserAction & v = * i;
                
                NSRegularExpression * pattern = [NSRegularExpression regularExpressionWithPattern:[NSString stringWithCString:v.pattern.c_str() encoding:NSUTF8StringEncoding] options:NSRegularExpressionAnchorsMatchLines error:nil];
                
                NSTextCheckingResult * r = [pattern firstMatchInString:u options:NSMatchingReportProgress range:NSMakeRange(0, [u length])];
                
                if(r != nil) {
                    name = [NSString stringWithCString:v.name.c_str() encoding:NSUTF8StringEncoding];
                    if(v.policy == kk::ui::WebViewActionPolicyAllow) {
                        policy = WKNavigationActionPolicyAllow;
                    }
                    break;
                }
                
                
                i ++;
            }
            
            if(i != m.end()) {
                
                kk::Strong<kk::Event> e = new kk::Event();
                
                e->setData(new kk::TObject<kk::String,kk::String>({{"url",[u UTF8String]}}));
                
                view->emit([name UTF8String], e);
                
                decisionHandler(policy);
                
                return;
            }
            
        }
        
    }
    
    
    decisionHandler(WKNavigationActionPolicyAllow);
    
}


- (void)webView:(WKWebView *)webView didCommitNavigation:(null_unspecified WKNavigation *)navigation {
    
    kk::ui::View * view = (__bridge kk::ui::View *) objc_getAssociatedObject(webView, "__WKWebViewKerViewProtocol");
    
    if(view != nullptr) {
        kk::Strong<kk::Event> e = new kk::Event();
        view->emit("loading", e);
    }
    
}

- (void)webView:(WKWebView *)webView didFinishNavigation:(null_unspecified WKNavigation *)navigation {
    kk::ui::View * view = (__bridge kk::ui::View *) objc_getAssociatedObject(webView, "__WKWebViewKerViewProtocol");
    if(view != nullptr) {
        kk::Strong<kk::Event> e = new kk::Event();
        view->emit("load", e);
    }
}

- (void)webView:(WKWebView *)webView didFailNavigation:(null_unspecified WKNavigation *)navigation withError:(NSError *)error {
    kk::ui::View * view = (__bridge kk::ui::View *) objc_getAssociatedObject(webView, "__WKWebViewKerViewProtocol");
    if(view != nullptr) {
        kk::Strong<kk::Event> e = new kk::Event();
        e->setData(new kk::TObject<kk::String,kk::String>({{"errmsg",[[error localizedDescription] UTF8String]}}));
        view->emit("load", e);
    }
}

- (void) observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey,id> *)change context:(void *)context {
    
    if([object isKindOfClass:[WKWebView class]]) {
        if([keyPath isEqualToString:@"estimatedProgress"]) {
            
            kk::ui::View * view = (__bridge kk::ui::View *) objc_getAssociatedObject(object, "__WKWebViewKerViewProtocol");
            
            if(view != nullptr) {
                kk::Strong<kk::Event> e = new kk::Event();
                e->setData(new kk::TObject<kk::String,kk::Double>({{"errmsg",[(WKWebView *) object estimatedProgress]}}));
                view->emit("progress", e);
            }
            
        }
    }
    
}


@end

@implementation WKWebView (KerViewProtocol)

+(instancetype) KerViewCreateWithConfiguration:(void *) config {
    
    kk::ui::WebViewConfiguration * v = dynamic_cast<kk::ui::WebViewConfiguration *>((kk::ui::ViewConfiguration *) config);
    
    WKWebViewConfiguration * configuration = [[WKWebViewConfiguration alloc] init];
    
    WKUserContentController * userContentController = [[WKUserContentController alloc] init];
    
    WKWebViewKKViewProtocol * object = [[WKWebViewKKViewProtocol alloc] init];
    
    [userContentController addScriptMessageHandler:object name:@"kk"];
    
    if(v != nullptr) {
        
        auto m = v->userScripts();
        
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
    
    [view setNavigationDelegate:object];
    [view setOpaque:NO];
    
    return view;
}

-(void) KerViewObtain:(KerViewCPointer) view {
    [super KerViewObtain:view];
    objc_setAssociatedObject(self, "__WKWebViewKerViewProtocol", (__bridge id) view, OBJC_ASSOCIATION_ASSIGN);
    [self.scrollView KerViewObtain:view];
}

-(void) KerViewRecycle:(KerViewCPointer) view {
    [super KerViewRecycle:view];
    objc_setAssociatedObject(self, "__WKWebViewKerViewProtocol", nil, OBJC_ASSOCIATION_ASSIGN);
    [self.scrollView KerViewRecycle:view];
}

-(UIView *) KerViewContentView {
    return self.scrollView;
}

-(void) KerView:(KerViewCPointer) view setAttribute:(const char *) key value:(const char *) value {
    
    [super KerView:view setAttribute:key value:value];
    
    if(key == nullptr) {
        return ;
    }
    
    if(strcmp(key, "src") == 0) {
        
        if(value != nullptr) {
            
            NSString * v = [NSString stringWithCString:value encoding:NSUTF8StringEncoding];
            
            if([v rangeOfString:@"://"].location == NSNotFound) {
                
                kk::ui::View * view = (__bridge kk::ui::View *) objc_getAssociatedObject(self, "__WKWebViewKerViewProtocol");
                
                if(view) {
                    
                    kk::ui::Context * ctx = view->context();
                    
                    if(ctx != nullptr) {
                        kk::CString basePath = ctx->basePath();
                        NSURL * baseURL = [NSURL fileURLWithPath:[NSString stringWithCString:basePath encoding:NSUTF8StringEncoding]];
                        [self loadFileURL:[NSURL URLWithString:v relativeToURL:baseURL] allowingReadAccessToURL:baseURL];
                    }
                }
                
                
            } else {
                @try {
                    NSURL * u = [NSURL URLWithString:[NSString stringWithCString:value encoding:NSUTF8StringEncoding]];
                    [self loadRequest:[NSURLRequest requestWithURL:u]];
                }
                @catch(NSException * ex) {
                    NSLog(@"[KK] %@",ex);
                }
            }
        }
        
    } else if(strcmp(key, "#text") == 0) {
        
        if(value != nullptr) {
            
            NSURL * baseURL = nil;
            
            kk::ui::View * view = (__bridge kk::ui::View *) objc_getAssociatedObject(self, "__WKWebViewKerViewProtocol");
            
            if(view) {
                
                kk::ui::Context * ctx = view->context();
                
                if(ctx != nullptr) {
                    kk::CString basePath = ctx->basePath();
                    baseURL = [NSURL fileURLWithPath:[NSString stringWithCString:basePath encoding:NSUTF8StringEncoding]];
                }
            }
            
            [self loadHTMLString:[NSString stringWithCString:value encoding:NSUTF8StringEncoding] baseURL:baseURL];
            
        }
    }
    
}

-(void) KerView:(KerViewCPointer) view setContent:(const char *) content contentType:(const char *) contentType basePath:(const char *) basePath {
    
    if(content != nullptr) {
        
        NSURL * baseURL = nil;
        
        kk::ui::View * view = (__bridge kk::ui::View *) objc_getAssociatedObject(self, "__WKWebViewKerViewProtocol");
        
        if(view) {
            
            kk::ui::Context * ctx = view->context();
            
            if(ctx != nullptr) {
                
                kk::String path ;
                
                if(basePath == nullptr) {
                    path.append(ctx->basePath());
                } else {
                    path = ctx->absolutePath(basePath);
                }
                
                if(!kk::CStringHasSuffix(path.c_str(), "/")) {
                    path.append("/");
                }
  
                baseURL = [NSURL URLWithString:[KerApp relativeURI:[NSString stringWithCString:path.c_str() encoding:NSUTF8StringEncoding]]];
            }
        }
        
        if(contentType != nullptr) {
            [self loadData:[NSData dataWithBytes:content length:strlen(content)] MIMEType:[NSString stringWithCString:contentType encoding:NSUTF8StringEncoding] characterEncodingName:@"utf-8" baseURL:baseURL];
        } else {
            [self loadHTMLString:[NSString stringWithCString:content encoding:NSUTF8StringEncoding] baseURL:baseURL];
        }
        
    }
}


@end

