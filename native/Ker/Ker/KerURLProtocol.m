//
//  KerURLProtocol.m
//  Ker
//
//  Created by zhanghailong on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerUI.h"
#import "KerURLProtocol.h"
#import <MobileCoreServices/MobileCoreServices.h>

@protocol KerUIWKBrowsingContextController  <NSObject>

-(void) registerSchemeForCustomProtocol:(NSString *) scheme;

@end


@interface KerURLProtocol()<WKURLSchemeHandler>

@end

static NSString * kKerURLProtocolKey = @"kKerURLProtocolKey";

@implementation KerURLProtocol

+(void) installWKWebViewConfiguration:(WKWebViewConfiguration *) confirguration {
    
    if (@available(iOS 11.0, *)) {
        KerURLProtocol * v = [[KerURLProtocol alloc] init];
        [confirguration setURLSchemeHandler:v forURLScheme:@"ker-app"];
        [confirguration setURLSchemeHandler:v forURLScheme:@"ker-tmp"];
        [confirguration setURLSchemeHandler:v forURLScheme:@"ker-data"];
        [confirguration setURLSchemeHandler:v forURLScheme:@"ker-native"];
    } else {

    }
}

+(void) openlibs {
    
    if (@available(iOS 11.0, *)) {
    } else {
        [NSURLProtocol registerClass:[KerURLProtocol class]];
        {
            Class cls = NSClassFromString(@"WKBrowsingContextController");
            SEL sel = @selector(registerSchemeForCustomProtocol:);
            if([cls respondsToSelector:sel]) {
                [(id<KerUIWKBrowsingContextController>)cls registerSchemeForCustomProtocol:@"ker-tmp"];
                [(id<KerUIWKBrowsingContextController>)cls registerSchemeForCustomProtocol:@"ker-data"];
                [(id<KerUIWKBrowsingContextController>)cls registerSchemeForCustomProtocol:@"ker-app"];
                [(id<KerUIWKBrowsingContextController>)cls registerSchemeForCustomProtocol:@"ker-native"];
            }
        }
    }
    
}

+ (BOOL)canInitWithRequest:(NSURLRequest *)request {
    if([NSURLProtocol propertyForKey:kKerURLProtocolKey inRequest:request]) {
        return NO;
    }
    NSString * scheme = [request.URL scheme];
    return [scheme isEqualToString:@"ker-tmp"] || [scheme isEqualToString:@"ker-data"] || [scheme isEqualToString:@"ker-app"] || [scheme isEqualToString:@"ker-native"];
}

+ (NSURLRequest *)canonicalRequestForRequest:(NSURLRequest *)request {
    return request;
}

-(void) dealloc {
//    NSLog(@"[Ker] [KerURLProtocol] [dealloc]");
}

-(void) getContent:(NSURL *) URL response:(NSURLResponse **) resp data:(NSData **) data error:(NSError **) error {
    
//    NSLog(@"[Ker] [KerURLProtocol] %@",[URL absoluteString]);
    
    NSString * filePath = nil;
    
    NSString * scheme = [URL scheme];
    
    if([scheme hasPrefix:@"ker-"]) {
        filePath = [KerUI resolvePath:URL.absoluteString];
    } else {
        * error = [NSError errorWithDomain:@"KerURLProtocol" code:0 userInfo:@{NSLocalizedDescriptionKey:@"Not Found File Path"}];
        return;
    }
    
    * data = [NSData dataWithContentsOfFile:filePath];
    
    if(* data == nil) {
        * error = [NSError errorWithDomain:@"KerURLProtocol" code:0 userInfo:@{NSLocalizedDescriptionKey:@"Not Found File"}];
        return;
    }
    
    NSString * mimeType = [KerUI mimeType:filePath data:* data defaultType:@"application/octet-stream"];
    
    * resp = [[NSURLResponse alloc] initWithURL:URL MIMEType:mimeType expectedContentLength:(* data).length textEncodingName:nil];
    
}

- (void)startLoading {
    
    NSMutableURLRequest * req = [self.request mutableCopy];
    
    [NSURLProtocol setProperty:@(YES) forKey:kKerURLProtocolKey inRequest:req];
    
    NSError * err = nil;
    NSData * data = nil;
    NSURLResponse * resp = nil;
    
    [self getContent:req.URL response:&resp data:&data error:&err];
    
    if(err != nil) {
        [self.client URLProtocol:self didFailWithError:err];
        return;
    }
    
    [self.client URLProtocol:self didReceiveResponse:resp cacheStoragePolicy:NSURLCacheStorageAllowed];
    [self.client URLProtocol:self didLoadData:data];
    [self.client URLProtocolDidFinishLoading:self];
    
}

- (void)stopLoading {
    
}

- (void)webView:(WKWebView *)webView startURLSchemeTask:(id <WKURLSchemeTask>)urlSchemeTask  API_AVAILABLE(ios(11.0)){
    
    NSError * err = nil;
    NSData * data = nil;
    NSURLResponse * resp = nil;
    
    [self getContent:[urlSchemeTask request].URL response:&resp data:&data error:&err];
    
    if(err != nil) {
        [urlSchemeTask didFailWithError:err];
        return;
    }
    
    [urlSchemeTask didReceiveResponse:resp];
    [urlSchemeTask didReceiveData:data];
    [urlSchemeTask didFinish];
    
}

- (void)webView:(WKWebView *)webView stopURLSchemeTask:(id <WKURLSchemeTask>)urlSchemeTask  API_AVAILABLE(ios(11.0)){
    
}


@end
