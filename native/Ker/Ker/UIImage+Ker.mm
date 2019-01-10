//
//  UIImage+Ker.m
//  Ker
//
//  Created by zhanghailong on 2018/12/10.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "UIImage+Ker.h"
#import <CommonCrypto/CommonCrypto.h>
#import "KerUI.h"

#include <core/dispatch.h>

namespace kk {
    extern ::dispatch_queue_t DispatchQueueGCD(DispatchQueue * queue);
}

@interface UIImageKerCacheItem : NSObject

@property(nonatomic,weak) UIImage * image;

@end

@implementation UIImageKerCacheItem

@end

@interface UIImageKerCache : NSObject {
    NSMutableDictionary * _items;
    NSMutableDictionary * _loadings;
}

-(UIImage *) imageWithPath:(NSString *) path;

-(void) setImage:(UIImage *) image path:(NSString *) path;

-(BOOL) hasLoading:(NSString *) path;

-(BOOL) addLoading:(NSString *) path callback:(UIImageKerCallback) callback;

-(void) removeLoading:(NSString *) path callback:(UIImageKerCallback) callback;

-(NSSet *) clearLoading:(NSString * ) path ;

@end

@implementation UIImageKerCache

-(UIImage *) imageWithPath:(NSString *) path {
    if(path == nil) {
        return nil;
    }
    @synchronized (self) {
        UIImageKerCacheItem * item = [_items valueForKey:path];
        if(item != nil) {
            UIImage * image = item.image;
            if(image == nil) {
                [_items removeObjectForKey:path];
            }
            return image;
        }
    }
    return nil;
}

-(void) setImage:(UIImage *) image path:(NSString *) path {
    if(path == nil || image == nil) {
        return;
    }
    @synchronized (self) {
        if(_items == nil) {
            _items = [[NSMutableDictionary alloc] initWithCapacity:4];
        }
        UIImageKerCacheItem * item = [_items valueForKey:path];
        if(item == nil) {
            item = [[UIImageKerCacheItem alloc] init];
            [_items setValue:item forKey:path];
        }
        item.image = image;
    }
}

-(BOOL) hasLoading:(NSString *) path {
    @synchronized (self) {
        return [_loadings valueForKey:path] != nil;
    }
}

-(BOOL) addLoading:(NSString *) path callback:(UIImageKerCallback) callback {
    
    BOOL r = YES;
    
    @synchronized (self) {
        
        if(_loadings == nil) {
            _loadings = [[NSMutableDictionary alloc] initWithCapacity:4];
        }
        
        NSMutableSet * callbacks = [_loadings valueForKey:path];
        
        if(callbacks == nil) {
            callbacks = [[NSMutableSet alloc] initWithCapacity:4];
            [_loadings setValue:callbacks forKey:path];
            r = NO;
        }
        
        [callbacks addObject:callback];
        
    }
    
    return r;
}

-(void) removeLoading:(NSString *) path callback:(UIImageKerCallback) callback {
    
    @synchronized (self) {
        
        NSMutableSet * callbacks = [_loadings valueForKey:path];
        
        if(callbacks != nil) {
            [callbacks removeObject:callback];
            if([callbacks count] == 0) {
                [_loadings removeObjectForKey:path];
            }
        }
        
    }
    
}

-(NSSet *) clearLoading:(NSString * ) path {
    
    @synchronized (self) {
        
        NSMutableSet * callbacks = [_loadings valueForKey:path];
        
        if(callbacks != nil) {
            [_loadings removeObjectForKey:path];
        }
        
        return callbacks;
        
    }
    
    return nil;
}

@end

@implementation UIImage (Ker)

+(NSString *) ker_keyWithURI:(NSString *) URI {
    CC_MD5_CTX m;
    CC_MD5_Init(&m);
    CC_MD5_Update(&m, [URI UTF8String], (CC_LONG) [URI length]);
    unsigned char md[16];
    CC_MD5_Final(md, &m);
    return [NSString stringWithFormat:@"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
            ,md[0],md[1],md[2],md[3],md[4],md[5],md[6],md[7]
            ,md[8],md[9],md[10],md[11],md[12],md[13],md[14],md[15]];
}

+(NSString *) ker_pathWithURI:(NSString *) URI {
    return [KerUI resolvePath:URI];
    
}

+(UIImageKerCache *) ker_imageCache {
    static UIImageKerCache * v = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        v = [[UIImageKerCache alloc] init];
    });
    return v;
}

+(UIImage *) ker_imageWithURI:(NSString *) URI {
    NSString * path = [self ker_pathWithURI:URI];
    UIImageKerCache * cache = [self ker_imageCache];
    UIImage * image = [cache imageWithPath:path];
    if(image == nil) {
        image = [UIImage imageWithContentsOfFile:path];
        if(image != nil) {
            [cache setImage:image path:path];
        }
    }
    return image;
}

+(UIImage *) ker_imageByCacheWithURI:(NSString *) URI {
    NSString * path = [self ker_pathWithURI:URI];
    UIImageKerCache * cache = [self ker_imageCache];
    UIImage * image = [cache imageWithPath:path];
    return image;
}

+(void) ker_imageWithURI:(NSString *) URI callback:(UIImageKerCallback) callback queue:(dispatch_queue_t) queue {
    NSString * path = [self ker_pathWithURI:URI];
    UIImageKerCache * cache = [self ker_imageCache];
    UIImage * image = [cache imageWithPath:path];
    
    if(queue == nil) {
        queue = dispatch_get_main_queue();
    }
    
    if(image == nil) {
        
        dispatch_async(kk::DispatchQueueGCD(kk::IODispatchQueue()), ^{
            
            NSFileManager * fm = [NSFileManager defaultManager];
            
            UIImage * image = nil;
            NSError * err = nil;
            
            if([fm fileExistsAtPath:path]) {
                image = [UIImage imageWithContentsOfFile:path];
                if(image != nil) {
                    [cache setImage:image path:path];
                }
            } else if([URI hasPrefix:@"http://"] || [URI hasPrefix:@"https://"]){
                
                if(callback) {
                    
                    if(! [cache addLoading:path callback:callback]) {
                        
                        NSURL * u = nil;
                        
                        @try {
                            u = [NSURL URLWithString:URI];
                        }
                        @catch(NSException * ex) {
                            @try {
                                u = [NSURL URLWithString:[URI stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLPathAllowedCharacterSet]]];
                            }
                            @catch(NSException * ex) {
                                err = [NSError errorWithDomain:@"UIImageKer" code:0 userInfo:@{NSLocalizedDescriptionKey:[ex description]}];
                            }
                            
                        }
                        
                        if(u != nil) {
                            
                            NSMutableURLRequest * req = [NSMutableURLRequest requestWithURL:u];
                            
                            NSURLSessionDownloadTask * task = [[NSURLSession sharedSession] downloadTaskWithRequest:req
                                                                                                  completionHandler:^(NSURL * location, NSURLResponse * response, NSError *  error) {
                                                                                                      
                                                                                                      UIImage * image = nil;
                                                                                                      
                                                                                                      if(error == nil) {
                                                
                                                                                                          NSFileManager * fm = [NSFileManager defaultManager];
                                                                                                          [fm removeItemAtPath:path error:nil];
                                                                                                          [fm moveItemAtURL:location toURL:[NSURL fileURLWithPath:path] error:nil];
                                                                                                          
                                                                                                          image = [UIImage imageWithContentsOfFile:path];
                                                                                                          
                                                                                                          if(image == nil) {
                                                                                                              error = [NSError errorWithDomain:@"UIImageKer" code:0 userInfo:@{NSLocalizedDescriptionKey:@"未找到图片"}];
                                                                                                          } else {
                                                                                                              [cache setImage:image path:path];
                                                                                                          }
                                                                                                          
                                                                                                      }
                                                                                                      
                                                                                                      dispatch_async(queue, ^{
                                                                                                          
                                                                                                          for(UIImageKerCallback cb in [cache clearLoading:path]) {
                                                                                                              cb(image,error);
                                                                                                          }
                                                                                                          
                                                                                                      });
                                                                                                      
                                                                                                  }];
                            
                            [task resume];
                            
                        } else {
                            
                            dispatch_async(queue, ^{
                                
                                for(UIImageKerCallback cb in [cache clearLoading:path]) {
                                    if(err == nil) {
                                        cb(nil,[NSError errorWithDomain:@"UIImageKer" code:0 userInfo:@{NSLocalizedDescriptionKey:@"未找到图片"}]);
                                    } else {
                                        cb(nil,err);
                                    }
                                }
                                
                            });
                            
                        }
                        
                    }
                    
                }
                
                return ;
            }
            
            if(callback) {
                
                dispatch_async(queue, ^{
                   
                    if(image == nil && err == nil) {
                        callback(nil,[NSError errorWithDomain:@"UIImageKer" code:0 userInfo:@{NSLocalizedDescriptionKey:@"未找到图片"}]);
                    } else if(image != nil) {
                        callback(image,nil);
                    } else {
                        callback(nil,err);
                    }
                });
                
            }
            
        });
        
    } else if(callback) {
        dispatch_async(queue, ^{
            callback(image,nil);
        });
    }
}

@end
