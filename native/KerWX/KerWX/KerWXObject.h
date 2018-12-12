//
//  KerWXObject.h
//  KerWX
//
//  Created by hailong11 on 2018/11/15.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Ker/Ker.h>

@interface KerWXObject : NSObject<KerJSExport>

+(void) openlib;

@property(nonatomic,strong,readonly) NSString * basePath;
@property(nonatomic,strong,readonly) NSString * dataPath;

-(instancetype) initWithBasePath:(NSString *) basePath dataPath:(NSString *) dataPath;

@end

@protocol WXCallbackFunction <NSObject>
-(void) success:(id) res;
-(void) fail:(id) res;
-(void) complete:(id)res;
@end

@interface WXCallbackRes : NSObject
@property (nonatomic, copy) NSString * errMsg;
-(instancetype) initWithErrMsg:(NSString *)errMsg;
@end





