//
//  KerWXObject+Http.h
//  KerWX
//
//  Created by hailong11 on 2018/11/16.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWXObject.h>

@interface KerWXRequestRes : NSObject

@property(nonatomic,strong) id data;
@property(nonatomic,assign) int statusCode;
@property(nonatomic,strong) NSDictionary * header;

@end

@protocol KerWXRequestTask <NSObject,KerJSExport>

-(void) onHeadersReceived:(KerJSObject *) callback;

-(void) offHeadersReceived:(KerJSObject *) callback;

-(void) abort;

@end

@protocol KerWXRequestObject <NSObject>

@property(nonatomic,strong,readonly) NSString * url;
@property(nonatomic,strong,readonly) id data;
@property(nonatomic,strong,readonly) id header;
@property(nonatomic,strong,readonly) NSString * method;
@property(nonatomic,strong,readonly) NSString * dataType;
@property(nonatomic,strong,readonly) NSString * responseType;

-(void) success:(KerWXRequestRes *) res;
-(void) fail:(NSString *) errmsg;
-(void) complete;

@end

@interface KerWXObject (Http)

+(NSURLSession *) defaultSession;

+(void) setDefaultSession:(NSURLSession *) session;

-(id<KerWXRequestTask>) request:(KerJSObject *) object;

@end
