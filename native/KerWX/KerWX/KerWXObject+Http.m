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

@property(nonatomic,strong) NSURLSessionDataTask * sessionTask;

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
    if(callback== nil || [_callback isEqual: callback] ) {
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

-(id<KerWXRequestTask>) request:(KerJSObject *) jsObject {
    
    id<KerWXRequestObject> object = [jsObject implementProtocol:@protocol(KerWXRequestObject)];
    
    KerWXRequestTask * task = [[KerWXRequestTask alloc] init];
    
    NSString * URI = object.url;
    
    if(URI == nil) {
        [object fail:@"Not Found URL"];
        [object complete];
        return nil;
    }
    
    NSString * method = [object method];
    
    if(method == nil) {
        method = @"GET";
    }
    
    id data = [object data];
    
    if([method isEqualToString:@"GET"]) {
        
        NSMutableArray * vs = [NSMutableArray arrayWithCapacity:4];
        
        if([data isKindOfClass:[NSDictionary class]]) {
            NSEnumerator * keyEnum = [data keyEnumerator];
            NSString * key;
            while((key = [keyEnum nextObject])) {
                [vs addObject:[NSString stringWithFormat:@"%@=%@",key,[KerUI encodeURL:[data ker_getString:key]]]];
            }
        }
        
        if([vs count] > 0) {
            if([URI hasSuffix:@"?"]) {
                URI = [URI stringByAppendingString:[vs componentsJoinedByString:@"&"]];
            } else if([URI containsString:@"?"]) {
                URI = [[URI stringByAppendingString:@"&"] stringByAppendingString:[vs componentsJoinedByString:@"&"]];
            } else {
                URI = [[URI stringByAppendingString:@"?"] stringByAppendingString:[vs componentsJoinedByString:@"&"]];
            }
        }
    }
    
    NSURL * u = nil;
    
    @try {
        u = [NSURL URLWithString:URI];
    }
    @catch(NSException * ex) {
        [object fail:[ex description]];
        [object complete];
        return nil;
    }
    
    if(u == nil) {
        [object fail:@"Not Found URL"];
        [object complete];
        return nil;
    }
    
    NSMutableURLRequest * r = [NSMutableURLRequest requestWithURL:u];
    
    id header = [object header];
    
    if([header isKindOfClass:[NSDictionary class]]) {
        NSEnumerator * keyEnum = [header keyEnumerator];
        NSString * key;
        while((key = [keyEnum nextObject])) {
            [r setValue:[header ker_getString:key] forHTTPHeaderField:key];
        }
    }
    
    [r setHTTPMethod:method];
    
    if(![method isEqualToString:@"GET"]) {
        
        NSString * contentType = [r valueForHTTPHeaderField:@"Content-Type"];
        
        if([data isKindOfClass:[NSDictionary class]]) {
            
            if([contentType isEqualToString:@"json"]) {
                [r setHTTPBody:[NSJSONSerialization dataWithJSONObject:data options:NSJSONWritingPrettyPrinted error:nil]];
            } else {
                NSMutableString * string = [NSMutableString stringWithCapacity:64];
                NSEnumerator * keyEnum = [data keyEnumerator];
                NSString * key;
                while((key = [keyEnum nextObject])) {
                    if([string length]) {
                        [string appendString:@"&"];
                    }
                    [string appendFormat:@"%@=%@",key,[KerUI encodeURL:[data ker_getString:key]]];
                }
                [r setHTTPBody:[string dataUsingEncoding:NSUTF8StringEncoding]];
            }
            
        } else if([data isKindOfClass:[NSString class]]) {
            [r setHTTPBody:[data dataUsingEncoding:NSUTF8StringEncoding]];
        } else if([data isKindOfClass:[NSData class]]) {
            [r setHTTPBody:data];
        }
    }
    
    NSLog(@"[Ker] [HTTP] %@",[r.URL absoluteString]);
    
    __weak KerWXRequestTask * reqTask = task;
    
    dispatch_queue_t queue = jsObject.queue;
    
    [task setSessionTask:[[NSURLSession sharedSession] dataTaskWithRequest:r completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        
        dispatch_async(queue, ^{
            
            [reqTask onResponse:(NSHTTPURLResponse *) response];
            
            if(error != nil) {
                
                [object fail:[error localizedDescription]];
                [object complete];
                
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
                [object complete];
            }
            
        });
        
    }]];
    
    [task.sessionTask resume];
    
    return task;
}

@end
