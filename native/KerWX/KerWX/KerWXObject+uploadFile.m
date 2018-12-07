//
//  KerWXObject+uploadFile.m
//  KerWX
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+uploadFile.h"
#import "KerWXObject+Http.h"
#import "KerWXHttpBody.h"

@implementation KerWXUploadFileRes


@end

@interface KerWXUploadTask : NSObject<KerWXUploadTask>{
    KerJSObject * _callback;
    KerJSObject * _process;
}

@property(nonatomic,strong) KerJSObject * process;
@property(nonatomic,strong) NSURLSessionUploadTask * sessionTask;

-(void) onResponse:(NSHTTPURLResponse *) response;

@end

@interface KerWXUploadTaskSessionDelegate : NSObject<NSURLSessionDataDelegate> {
    NSMutableData * _data;
}

@property(nonatomic,strong) NSString * tempFilePath;
@property(nonatomic,strong) NSString * filePath;
@property(nonatomic,weak) KerWXUploadTask * task;
@property(nonatomic,strong) id<KerWXUploadFileObject> object;

@end

@implementation KerWXUploadTaskSessionDelegate

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task
didCompleteWithError:(nullable NSError *)error {
    
    id<KerWXUploadFileObject> object = _object;
    
    if(error != nil ){
    
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [object fail:[error localizedDescription]];
            
        });
    } else {
        
        KerWXUploadFileRes * res = [[KerWXUploadFileRes alloc] init];
        
        res.data = _data == nil ? nil :  [[NSString alloc] initWithData:_data encoding:NSUTF8StringEncoding];

        dispatch_async(dispatch_get_main_queue(), ^{
            [object success:res];
        });
    }
    
    [session finishTasksAndInvalidate];
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [object complete];
    });

}

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task
   didSendBodyData:(int64_t)bytesSent
    totalBytesSent:(int64_t)totalBytesSent
totalBytesExpectedToSend:(int64_t)totalBytesExpectedToSend {
    KerJSObject * process = _task.process;
    if(process && totalBytesExpectedToSend >0 ) {
        [process callWithArguments:@[@(totalBytesSent * 100 / totalBytesExpectedToSend),@(totalBytesSent),@(totalBytesExpectedToSend)]];
    }
}

- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask
    didReceiveData:(NSData *)data {
    if(_data == nil) {
        _data = [[NSMutableData alloc] initWithCapacity:64];
    }
    [_data appendData:data];
}

@end

@implementation KerWXUploadTask

-(void) dealloc {
    NSLog(@"[KerWXUploadTask] [dealloc]");
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

-(void) onProgressUpdate:(KerJSObject *) callback {
    _process = callback;
}

-(void) offProgressUpdate:(KerJSObject *) callback {
    if(callback== nil || [_process isEqual: callback] ) {
        _process = nil;
    }
}

-(void) onResponse:(NSHTTPURLResponse *) response {
    if(_callback) {
        [_callback callWithArguments:@[[response allHeaderFields]]];
    }
}

@end


@implementation KerWXObject (uploadFile)


-(id<KerWXUploadTask>) uploadFile:(KerJSObject *) jsObject {
    
    id<KerWXUploadFileObject> object = [jsObject implementProtocol:@protocol(KerWXUploadFileObject)];
    
    KerWXUploadTask * task = [[KerWXUploadTask alloc] init];
    
    NSString * URI = object.url;
    
    if(URI == nil) {
        [object fail:@"Not Found URL"];
        [object complete];
        return nil;
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
    
    [r setHTTPMethod:@"POST"];
    
    id header = [object header];
    
    if([header isKindOfClass:[NSDictionary class]]) {
        NSEnumerator * keyEnum = [header keyEnumerator];
        NSString * key;
        while((key = [keyEnum nextObject])) {
            [r setValue:[header ker_getString:key] forHTTPHeaderField:key];
        }
    }
    
    KerWXHttpBody * body = [[KerWXHttpBody alloc] init];
    
    NSString * filePath = [KerApp filePathWithURI:object.filePath basePath:self.dataPath];
    NSString * name = object.name;
    
    if(filePath != nil && name != nil) {
        NSData * data = [NSData dataWithContentsOfFile:filePath];
        if(data == nil) {
            [object fail:@"Not Found File"];
            [object complete];
            return nil;
        }
        NSString * mimeType = [KerApp mimeType:filePath data:data defaultType:@"application/octet-stream"];
        [body add:name data:data type:mimeType name:filePath.lastPathComponent];
    }
    
    id formData = object.formData;
    
    if([formData isKindOfClass:[NSDictionary class]]) {
        NSEnumerator * keyEnum = [formData keyEnumerator];
        NSString * key;
        while((key = [keyEnum nextObject])) {
            NSString * v = [formData ker_getString:key];
            if(v) {
                [body add:key value:v];
            }
        }
    }
    
    [r setValue:[body contentType] forHTTPHeaderField:@"Content-Type"];
    
    NSLog(@"[Ker] [HTTP] %@",[r.URL absoluteString]);
    
    KerWXUploadTaskSessionDelegate * delegate = [[KerWXUploadTaskSessionDelegate alloc] init];
    delegate.task = task;
    delegate.filePath = filePath;
    delegate.object  = object;
    
    NSURLSession * session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:delegate delegateQueue:[NSOperationQueue currentQueue]];
    
    [task setSessionTask:[session uploadTaskWithRequest:r fromData:[body data]]];
    
    [task.sessionTask resume];
    
    return task;
    
}

@end


