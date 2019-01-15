//
//  KerWXObject+File.m
//  KerWX
//
//  Created by zuowu on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+File.h"
#include <CommonCrypto/CommonDigest.h>

@implementation WXGetFileInfoRes

@end

static NSString * md5OfPath(NSString * path){
    
    NSFileManager * fileManager = [NSFileManager defaultManager];
    if ([fileManager fileExistsAtPath:path]) {
        NSData * data = [NSData dataWithContentsOfFile:path];
        unsigned char digest[CC_MD5_DIGEST_LENGTH];
        CC_MD5( data.bytes, (CC_LONG)data.length, digest );
        NSMutableString * output = [NSMutableString stringWithCapacity:CC_MD5_DIGEST_LENGTH * 2];
        for(int i = 0; i < CC_MD5_DIGEST_LENGTH; i++){
            [output appendFormat:@"%02x", digest[i]];
        }
        return output;
    }
    
    return @"";
    
}

static NSString * sha1OfPath(NSString * path){
    
    NSFileManager * fileManager = [NSFileManager defaultManager];
    if ([fileManager fileExistsAtPath:path]) {
        NSData *data = [NSData dataWithContentsOfFile:path];
        unsigned char digest[CC_SHA1_DIGEST_LENGTH];
        CC_SHA1( data.bytes, (CC_LONG)data.length, digest );
        NSMutableString *output = [NSMutableString stringWithCapacity:CC_SHA1_DIGEST_LENGTH * 2];
        for( int i = 0; i < CC_SHA1_DIGEST_LENGTH; i++ ){
            [output appendFormat:@"%02x", digest[i]];
        }
        
        return output;
    }
    
    return @"";
    
}

static unsigned long long fileSizeOfPath(NSString * path){
    NSFileManager * manager = [NSFileManager defaultManager];
    if ([manager fileExistsAtPath:path]) {
        return [[manager attributesOfItemAtPath:path error:nil] fileSize];
    }
    return 0;
}

@implementation KerWXObject (File)

-(void) getFileInfo:(KerJSObject *) object {
    
    id<WXGetFileInfoObject> v = [object implementProtocol:@protocol(WXGetFileInfoObject)];
    
    //文件不存在
    NSFileManager * manager = [NSFileManager defaultManager];
    if (![manager fileExistsAtPath:v.filePath]) {
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"getFileInfo:fail invalid filePath"];
        [v fail:res];
        [v complete:res];
        return;
    }
    
    //路径参数不对
    if (![v.filePath isKindOfClass:[NSString class]]) {
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"getFileInfo:fail parameter error: parameter.filePath should be String"];
        [v fail:res];
        [v complete:res];
        return;
    }
    
    WXGetFileInfoRes * res = [[WXGetFileInfoRes alloc] init];
    res.size = fileSizeOfPath(v.filePath);

    if ([v.digestAlgorithm isEqualToString:@""] || [v.digestAlgorithm isEqualToString:@"md5"] || v.digestAlgorithm == nil) {
        res.digest = md5OfPath(v.filePath);
    }else if ([v.digestAlgorithm isEqualToString:@"sha1"]){
        res.digest = sha1OfPath(v.filePath);
    }else {
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"getFileInfo:fail invalid digestAlgorithm"];
        [v fail:res];
        [v complete:res];
        return;
    }
    
    [v success:res];
    [v complete:res];

}

@end
