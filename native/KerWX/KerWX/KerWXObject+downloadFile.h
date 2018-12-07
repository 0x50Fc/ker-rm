//
//  KerWXObject+downloadFile.h
//  KerWX
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWXObject.h>

@interface KerWXDownloadFileRes : NSObject

@property(nonatomic,strong) NSString * tempFilePath;
@property(nonatomic,assign) int statusCode;

@end

@protocol KerWXDownloadTask <NSObject,KerJSExport>

-(void) onHeadersReceived:(KerJSObject *) callback;

-(void) offHeadersReceived:(KerJSObject *) callback;

-(void) onProgressUpdate:(KerJSObject *) callback;

-(void) offProgressUpdate:(KerJSObject *) callback;

-(void) abort;

@end

@protocol KerWXDownloadFileObject <NSObject>

@property(nonatomic,strong,readonly) NSString * url;
@property(nonatomic,strong,readonly) NSString * filePath;
@property(nonatomic,strong,readonly) id header;

-(void) success:(KerWXDownloadFileRes *) res;
-(void) fail:(NSString *) errmsg;
-(void) complete;

@end

@interface KerWXObject (downloadFile)

-(id<KerWXDownloadTask>) downloadFile:(KerJSObject *) object;

@end
