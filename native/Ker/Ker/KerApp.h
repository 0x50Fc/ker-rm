//
//  KerApp.h
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void * KerAppCPointer;

@class KerApp;

@protocol KerAppDelegate <NSObject>

@optional

-(BOOL) KerApp:(KerApp *) app openURI:(NSString *) uri;

-(BOOL) KerApp:(KerApp *) app openPageViewController:(NSString *) path animated:(BOOL) animated query:(NSDictionary<NSString *,NSString *> *) query;

-(BOOL) KerApp:(KerApp *) app openPageWindow:(NSString *) path animated:(BOOL) animated query:(NSDictionary<NSString *,NSString *> *) query;

-(BOOL) KerApp:(KerApp *) app openApp:(NSString *) url animated:(BOOL) animated query:(NSDictionary<NSString *,NSString *> *) query;

-(BOOL) KerApp:(KerApp *) app openViewController:(UIViewController *) viewController animated:(BOOL) animated;

@end

@interface KerApp : NSObject

+(void) openlib;

@property(nonatomic,strong) UIViewController * rootViewController;
@property(nonatomic,strong,readonly) NSString * basePath;
@property(nonatomic,assign,readonly) KerAppCPointer CPointer;
@property(nonatomic,weak) id<KerAppDelegate> delegate;

-(instancetype) initWithBasePath:(NSString *) basePath;

-(void) run:(NSDictionary<NSString *,NSString *> *) query;

-(void) open:(NSString *) uri animated:(BOOL) animated;

-(void) back:(NSUInteger)delta animated:(BOOL) animated;

-(void) openPageViewController:(NSString *) path animated:(BOOL) animated query:(NSDictionary<NSString *,NSString *> *) query;

-(void) openPageWindow:(NSString *) path animated:(BOOL) animated query:(NSDictionary<NSString *,NSString *> *) query;

-(void) openApp:(NSString *) url animated:(BOOL) animated query:(NSDictionary<NSString *,NSString *> *) query;

-(void) openViewController:(UIViewController *) viewController animated:(BOOL) animated;

-(void) recycle;

+(NSString *) encodeURL:(NSString *) url;

+(NSString *) decodeURL:(NSString *) url;

+(NSString *) userAgent;

+(void) setUserAgent:(NSString *)v;

@end
