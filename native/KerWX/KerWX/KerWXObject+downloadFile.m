//
//  KerWXObject+downloadFile.m
//  KerWX
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+downloadFile.h"
#import "KerWXObject+Http.h"

@implementation KerWXDownloadFileRes


@end

@interface KerWXDownloadTask : NSObject<KerWXDownloadTask>{
    KerJSObject * _callback;
    KerJSObject * _process;
}

@property(nonatomic,strong) KerJSObject * process;
@property(nonatomic,strong) NSURLSessionDownloadTask * sessionTask;

-(void) onResponse:(NSHTTPURLResponse *) response;

@end

@interface KerWXDownloadTaskSessionDelegate : NSObject<NSURLSessionDownloadDelegate>

@property(nonatomic,strong) NSString * tempFilePath;
@property(nonatomic,strong) NSString * filePath;
@property(nonatomic,weak) KerWXDownloadTask * task;
@property(nonatomic,strong) id<KerWXDownloadFileObject> object;

@end

@implementation KerWXDownloadTaskSessionDelegate

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task
didCompleteWithError:(nullable NSError *)error {
    
    id<KerWXDownloadFileObject> object = _object;
    
    if(error != nil ){
    
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [object fail:[error localizedDescription]];
            
        });
    }

    [session finishTasksAndInvalidate];
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [object complete];
    });
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask
didWriteData:(int64_t)bytesWritten
totalBytesWritten:(int64_t)totalBytesWritten
totalBytesExpectedToWrite:(int64_t)totalBytesExpectedToWrite {
    KerJSObject * process = _task.process;
    if(process && totalBytesExpectedToWrite >0 ) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [process callWithArguments:@[@(totalBytesWritten * 100 / totalBytesExpectedToWrite),@(totalBytesWritten),@(totalBytesExpectedToWrite)]];
        });
    }
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask
didFinishDownloadingToURL:(NSURL *)location {
    
    KerWXDownloadFileRes * res = [[KerWXDownloadFileRes alloc] init];
    
    if(_filePath != nil) {
        NSFileManager * fm = [NSFileManager defaultManager];
        [fm removeItemAtPath:_filePath error:nil];
        [fm moveItemAtURL:location toURL:[NSURL fileURLWithPath:_filePath] error:nil];
    } else {
        NSString * path = [NSTemporaryDirectory() stringByAppendingFormat:@"Ker_%@",downloadTask.response.suggestedFilename];
        NSFileManager * fm = [NSFileManager defaultManager];
        [fm removeItemAtPath:path error:nil];
        [fm moveItemAtURL:location toURL:[NSURL fileURLWithPath:path] error:nil];
        res.tempFilePath = [KerApp relativeURI:path];
    }
    
    res.statusCode = (int) [(NSHTTPURLResponse *) downloadTask.response statusCode];
    
    __weak KerWXDownloadTask * task = _task;
    id<KerWXDownloadFileObject> object = _object;
    
    dispatch_async(dispatch_get_main_queue(), ^{
        
        [task onResponse:(NSHTTPURLResponse *) downloadTask.response];
        
        [object success:res];
        
    });
    
}

@end

@implementation KerWXDownloadTask

-(void) dealloc {
    NSLog(@"[KerWXDownloadTask] [dealloc]");
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


@implementation KerWXObject (downloadFile)


-(id<KerWXDownloadTask>) downloadFile:(KerJSObject *) jsObject {
    
    id<KerWXDownloadFileObject> object = [jsObject implementProtocol:@protocol(KerWXDownloadFileObject)];
    
    KerWXDownloadTask * task = [[KerWXDownloadTask alloc] init];
    
    NSString * URI = object.url;
    
    if(URI == nil) {
        [object fail:@"Not Found URL"];
        [object complete];
        return nil;
    }
    
    NSString * filePath = object.filePath;
    
    if([filePath length]) {
        filePath = [self.dataPath stringByAppendingPathComponent:filePath];
    } else {
        filePath = nil;
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
    
    [r setHTTPMethod:@"GET"];
    
    id header = [object header];
    
    if([header isKindOfClass:[NSDictionary class]]) {
        NSEnumerator * keyEnum = [header keyEnumerator];
        NSString * key;
        while((key = [keyEnum nextObject])) {
            [r setValue:[header ker_getString:key] forHTTPHeaderField:key];
        }
    }
    
    NSLog(@"[Ker] [HTTP] %@",[r.URL absoluteString]);
    
    KerWXDownloadTaskSessionDelegate * delegate = [[KerWXDownloadTaskSessionDelegate alloc] init];
    delegate.task = task;
    delegate.filePath = filePath;
    delegate.object  = object;
    
    NSURLSession * session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:delegate delegateQueue:[NSOperationQueue currentQueue]];
    
    [task setSessionTask:[session downloadTaskWithRequest:r]];
    
    [task.sessionTask resume];
    
    return task;
    
}

@end
