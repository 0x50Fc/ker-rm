//
//  KerWXObject+uploadFile.h
//  KerWX
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>

@interface KerWXUploadFileRes : NSObject

@property(nonatomic,strong) NSString * data;
@property(nonatomic,assign) int statusCode;

@end

@protocol KerWXUploadTask <NSObject,KerJSExport>

-(void) onHeadersReceived:(KerJSObject *) callback;

-(void) offHeadersReceived:(KerJSObject *) callback;

-(void) onProgressUpdate:(KerJSObject *) callback;

-(void) offProgressUpdate:(KerJSObject *) callback;

-(void) abort;

@end

@protocol KerWXUploadFileObject <NSObject>

@property(nonatomic,strong,readonly) NSString * url;
@property(nonatomic,strong,readonly) NSString * filePath;
@property(nonatomic,strong,readonly) NSString * name;
@property(nonatomic,strong,readonly) id formData;
@property(nonatomic,strong,readonly) id header;

-(void) success:(KerWXUploadFileRes *) res;
-(void) fail:(NSString *) errmsg;
-(void) complete;

@end

@interface KerWXObject (uploadFile)

-(id<KerWXUploadTask>) uploadFile:(KerJSObject *) object;

@end
