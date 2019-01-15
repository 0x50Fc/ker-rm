//
//  KerWXObject+File.h
//  KerWX
//
//  Created by zuowu on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>

@interface WXGetFileInfoRes : WXCallbackRes
@property (nonatomic, assign) unsigned long long size;
@property (nonatomic, copy) NSString * digest;
@end

@protocol WXGetFileInfoObject <WXCallbackFunction>
@property (nonatomic, copy) NSString * filePath;
@property (nonatomic, copy) NSString * digestAlgorithm;
@end

@interface KerWXObject (File)

-(void) getFileInfo:(KerJSObject *) object;

@end




