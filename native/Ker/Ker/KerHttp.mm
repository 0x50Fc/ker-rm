//
//  KerHttp.m
//  Ker
//
//  Created by zhanghailong on 2019/1/2.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <core/http.h>


typedef void (^KerHTTPRequestTaskProgress)(int64_t bytes,int64_t total);

@interface KerHTTPRequestTask : NSObject

@property(nonatomic,strong,readonly) NSURLSessionTask * task;
@property(nonatomic,strong) KerHTTPRequestTaskProgress progress;

-(instancetype) initWithSessionTask:(NSURLSessionTask *) task;

-(void) cancel ;

@end

@implementation KerHTTPRequestTask

-(instancetype) initWithSessionTask:(NSURLSessionTask *) task {
    if((self = [super init])) {
        _task = task;
        [_task addObserver:self forKeyPath:@"countOfBytesReceived" options:NSKeyValueObservingOptionNew context:nil];
    }
    return self;
}

-(void) dealloc {
    [_task removeObserver:self forKeyPath:@"countOfBytesReceived"];
    [_task cancel];
}

-(void) cancel {
    _progress = nil;
    [_task cancel];
}

-(void) observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey,id> *)change context:(void *)context {
    
    if(object == _task) {
        if(_progress) {
            _progress(_task.countOfBytesReceived,_task.countOfBytesExpectedToReceive);
        }
    }
    
}

@end

namespace kk {
    
    void HTTPRequest::send(void * data,size_t size) {
        
        this->retain();
        
        @autoreleasepool {
            
            NSURL * u ;
            
            @try {
                u = [NSURL URLWithString:[NSString stringWithCString:_url.c_str() encoding:NSUTF8StringEncoding]];
            }
            @catch(NSException * ex) {
                this->onError([[ex description] UTF8String]);
                return;
            }
            
            NSMutableURLRequest * req = [NSMutableURLRequest requestWithURL:u cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval:0];
            
            [req setHTTPMethod:[NSString stringWithCString:_method.c_str() encoding:NSUTF8StringEncoding]];
            
            if(data && size > 0) {
                [req setHTTPBody:[NSData dataWithBytes:data length:size]];
            }
            
            auto i = _reqHeaders.begin();
            auto e = _reqHeaders.end();
            
            while(i != e) {
                [req setValue:[NSString stringWithCString:i->second.c_str() encoding:NSUTF8StringEncoding] forHTTPHeaderField:[NSString stringWithCString:i->first.c_str() encoding:NSUTF8StringEncoding]];
                i ++;
            }
            
            NSURLSessionTask * task = [[NSURLSession sharedSession] dataTaskWithRequest:req completionHandler:^(NSData * data, NSURLResponse * response, NSError * error) {
                
                if(error != nil) {
                    this->onError([[error localizedDescription] UTF8String]);
                } else {
                    auto & m = this->respHeaders();
                    NSDictionary * h = [(NSHTTPURLResponse *) response allHeaderFields];
                    NSEnumerator * keyEnum = [h keyEnumerator];
                    NSString * key;
                    while((key = [keyEnum nextObject])) {
                        m[[key UTF8String]] = [[h valueForKey:key] UTF8String];
                    }
                    this->onResponse((kk::Int) [(NSHTTPURLResponse *) response statusCode]);
                    this->onData((void *) [data bytes], (size_t) [data length]);
                    this->onDone();
                }
                
                this->queue()->async([this]()->void{
                    this->release();
                });
            }];
            
            KerHTTPRequestTask * reqTask = [[KerHTTPRequestTask alloc] initWithSessionTask:task];
            
            reqTask.progress = ^(int64_t bytes, int64_t total) {
                this->onProgress(bytes, total);
            };
            
            [task resume];
            
            _native = new kk::NativeObject((__bridge kk::Native *) reqTask);
            
        }
    }
    
    void HTTPRequest::cancel() {
        
        if(_native != nullptr) {
            
            @autoreleasepool {
                KerHTTPRequestTask * task = (__bridge KerHTTPRequestTask *) _native->native();
                [task cancel];
            }
            
            _native = nullptr;
        }
        
    }
    
}

