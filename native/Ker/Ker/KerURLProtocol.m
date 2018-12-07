//
//  KerURLProtocol.m
//  Ker
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerApp.h"
#import "KerURLProtocol.h"
#import <MobileCoreServices/MobileCoreServices.h>

static NSString * kKerURLProtocolKey = @"kKerURLProtocolKey";

@implementation KerURLProtocol

+ (BOOL)canInitWithRequest:(NSURLRequest *)request {
    if([NSURLProtocol propertyForKey:kKerURLProtocolKey inRequest:request]) {
        return NO;
    }
    NSString * scheme = [request.URL scheme];
    return [scheme isEqualToString:@"ker-tmp"] || [scheme isEqualToString:@"ker-data"];
}

+ (NSURLRequest *)canonicalRequestForRequest:(NSURLRequest *)request {
    return request;
}

- (void)startLoading {
    
    NSMutableURLRequest * req = [self.request mutableCopy];
    
    [NSURLProtocol setProperty:@(YES) forKey:kKerURLProtocolKey inRequest:req];
    
    NSString * filePath = nil;
    
    NSString * scheme = [req.URL scheme];
    
    if([scheme isEqualToString:@"ker-tmp"]) {
        filePath = [KerApp pathWithURI:req.URL.absoluteString];
    } else if([scheme isEqualToString:@"ker-data"]) {
        filePath = [KerApp pathWithURI:req.URL.absoluteString];
    } else {
        [self.client URLProtocol:self didFailWithError:[NSError errorWithDomain:@"KerURLProtocol" code:0 userInfo:@{NSLocalizedDescriptionKey:@"Not Found File Path"}]];
        return;
    }
    
    NSData * data = [NSData dataWithContentsOfFile:filePath];
    
    if(data == nil) {
        [self.client URLProtocol:self didFailWithError:[NSError errorWithDomain:@"KerURLProtocol" code:0 userInfo:@{NSLocalizedDescriptionKey:@"Not Found File"}]];
        return;
    }
    
    NSString * mimeType = [KerApp mimeType:filePath data:data defaultType:@"application/octet-stream"];
    
    NSURLResponse * resp = [[NSURLResponse alloc] initWithURL:req.URL MIMEType:mimeType expectedContentLength:data.length textEncodingName:nil];
    
    [self.client URLProtocol:self didReceiveResponse:resp cacheStoragePolicy:NSURLCacheStorageAllowed];
    [self.client URLProtocol:self didLoadData:data];
    [self.client URLProtocolDidFinishLoading:self];
    
}

- (void)stopLoading {
    
}


@end
