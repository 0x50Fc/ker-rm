//
//  KerWXObject+File.h
//  KerWX
//
//  Created by zuowu on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>

@interface WXSaveFileRes : WXCallbackRes
@property (nonatomic, copy) NSString * savedFilePath;
@end

@interface WXGetFileInfoRes : WXCallbackRes
@property (nonatomic, assign) unsigned long long size;
@property (nonatomic, copy) NSString * digest;
@end

@interface WXGetSavedFileListRes : WXCallbackRes
@property (nonatomic, copy) NSArray<NSDictionary *> * fileList;
@end


@protocol WXSaveFileObject <WXCallbackFunction>
@property (nonatomic, copy) NSString * tempFilePath;
@end

@protocol WXGetFileInfoObject <WXCallbackFunction>
@property (nonatomic, copy) NSString * filePath;
@property (nonatomic, copy) NSString * digestAlgorithm;
@end

@protocol WXGetSavedFileInfoObject <WXCallbackFunction>
@property (nonatomic, copy) NSString * filePath;
@end

@protocol WXRemoveSavedFileObject <WXCallbackFunction>
@property (nonatomic, copy) NSString * filePath;
@end

@interface KerWXObject (File)

-(void) saveFile: (KerJSObject *) object;
-(void) getFileInfo: (KerJSObject *) object;
-(void) getSavedFileList: (KerJSObject *) object;
-(void) getSavedFileInfo: (KerJSObject *) object;
-(void) removeSavedFile: (KerJSObject *) object;

@end

@interface NSString (File)
+(NSString *)ker_uuidString;
@end




