//
//  KerWXObject+Http.m
//  KerWX
//
//  Created by hailong11 on 2018/11/16.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+Http.h"

@implementation KerWXRequestRes


@end

@interface KerWXRequestTask : NSObject<KerWXRequestTask>{
    KerJSObject * _callback;
}

@property(nonatomic,weak) NSURLSessionDataTask * sessionTask;

@end

@implementation KerWXRequestTask

-(void) dealloc {
    NSLog(@"[KerWXRequestTask] [dealloc]");
}

-(void) abort {
    [_sessionTask cancel];
}

-(void) onHeadersReceived:(KerJSObject *) callback {
    _callback = callback;
}

-(void) offHeadersReceived:(KerJSObject *) callback {
    if(callback== nil || _callback == callback) {
        _callback = nil;
    }
}

-(void) onResponse:(NSHTTPURLResponse *) response {
    if(_callback) {
        [_callback callWithArguments:@[[response allHeaderFields]]];
    }
}


@end

static NSURLSession * gKerWXObjectNSURLSession = nil;

@interface KerWXObjectHttp : NSObject

@end

@implementation KerWXObject (Http)

+(NSURLSession *) defaultSession {
    if(gKerWXObjectNSURLSession == nil) {
        gKerWXObjectNSURLSession = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]];
    }
    return gKerWXObjectNSURLSession;
}

+(void) setDefaultSession:(NSURLSession *) session {
    gKerWXObjectNSURLSession = session;
}


-(id<KerWXRequestTask>) request:(KerJSObject *) jsObject {
    
    id<KerWXRequestObject> object = [jsObject implementProtocol:@protocol(KerWXRequestObject)];
    
    KerWXRequestTask * task = [[KerWXRequestTask alloc] init];
    
    NSMutableURLRequest * r = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:object.url]];
    
    [task setSessionTask:[[KerWXObject defaultSession] dataTaskWithRequest:r completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        
        [task onResponse:(NSHTTPURLResponse *) response];
        
        if(error != nil) {
            
            [object fail:[error localizedDescription]];
            
        } else {
            
            
            NSString * contentType = [[[(NSHTTPURLResponse *) response allHeaderFields] valueForKey:@"Content-Type"] lowercaseString];
            NSStringEncoding encoding = NSUTF8StringEncoding;
            
            if([contentType containsString:@"charset=gbk"] || [contentType containsString:@"charset=gb2312"]) {
                encoding = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGBK_95);
            }
            
            KerWXRequestRes * res = [[KerWXRequestRes alloc] init];
            
            res.statusCode = (int) [(NSHTTPURLResponse *) response statusCode];
            res.header = [(NSHTTPURLResponse *) response allHeaderFields];
            
            if([[object dataType] isEqualToString:@"json"]) {
                if(encoding == NSUTF8StringEncoding) {
                    res.data = [[KerJSONDataObject alloc] initWithData:data];
                } else {
                    NSString * s = [[NSString alloc] initWithData:data encoding:encoding];
                    res.data = [s dataUsingEncoding:NSUTF8StringEncoding];
                }
            } else if([[object responseType] isEqualToString:@"arraybuffer"]){
                res.data = data;
            } else {
                res.data = [[NSString alloc] initWithData:data encoding:encoding];
            }
            
            [object success:res];
            
        }
    }]];
    
    return task;
}

@end
