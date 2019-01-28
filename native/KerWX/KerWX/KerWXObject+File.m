//
//  KerWXObject+File.m
//  KerWX
//
//  Created by zuowu on 2019/1/15.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+File.h"
#import <CommonCrypto/CommonDigest.h>
#import <objc/runtime.h>
#import <QuickLook/QuickLook.h>

#define WXKerFileOpenDocumentManagerKey "WXKerFileOpenDocumentManagerKey"

@implementation WXSaveFileRes
-(instancetype)initWithErrMsg:(NSString *)errMsg path:(NSString *)path{
    if (self = [super init]) {
        self.errMsg = errMsg;
        self.savedFilePath = path;
    }
    return self;
}
@end

@implementation WXGetFileInfoRes
@end

@implementation WXGetSavedFileListRes
-(instancetype)initWithErrMsg:(NSString *)errMsg fileList:(NSArray *)list{
    if (self = [super init]) {
        self.errMsg = errMsg;
        self.fileList = list;
    }
    return self;
}
@end

static NSString * ker_Md5OfPath(NSString * path){
    
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

static NSString * ker_Sha1OfPath(NSString * path){
    
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



BOOL ker_FileDirectoryExist(NSString * path){
    
    BOOL isDir = NO;
    BOOL isDirExist = [[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDir];
    
    if (isDirExist && isDir) {
        return YES;
    }else {
        return NO;
    }
    
}

NSString * ker_CreateFileDirectory(NSString * path){
    
    if (!ker_FileDirectoryExist(path)) {
        
        NSError * err;
        BOOL bCreateDir = [[NSFileManager defaultManager] createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:&err];
        
        if (err) {
            //错误
            return [NSString stringWithFormat:@"saveFile: fail create directoyr %@ Error, description %@", path, err.description];
        }
        if (!bCreateDir) {
            //创建文件夹错误
            return [NSString stringWithFormat:@"saveFile: fail create directoyr %@ Fail", path];
        }
    }
    
    //文件夹已经存在
    return nil;
}


@interface WXKerFileOpenDocumentManager : NSObject <QLPreviewControllerDelegate,QLPreviewControllerDataSource>

@property (nonatomic, strong) KerJSObject * openObject;

@end

@implementation WXKerFileOpenDocumentManager

-(void) openDocument: (KerJSObject *) object{
    
    self.openObject = object;
    id<WXOpenDocumentObject> v = [object implementProtocol:@protocol(WXOpenDocumentObject)];

    if (![[NSFileManager defaultManager] fileExistsAtPath:v.filePath]) {

        //文件不存在
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"openDocument:fail file not exist"];
        [v fail:res];
        [v complete:res];

    }else {
    
        //打开预览
        QLPreviewController * viewController = [[QLPreviewController alloc] init];
        viewController.delegate = self;
        viewController.dataSource = self;
        viewController.currentPreviewItemIndex = 0;
        
        dispatch_async(dispatch_get_main_queue(), ^{
            UIViewController * topViewController = [[UIApplication sharedApplication] keyWindow].rootViewController;
            if ([topViewController isKindOfClass:[UINavigationController class]]) {
                UINavigationController * nav = (UINavigationController *)topViewController;
                [nav pushViewController:viewController animated:YES];
            }
        });
        
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"openDocument:ok"];
        [v fail:res];
        [v complete:res];
        
    }

}

- (NSInteger)numberOfPreviewItemsInPreviewController:(nonnull QLPreviewController *)controller {
    return 1;
}

- (nonnull id<QLPreviewItem>)previewController:(nonnull QLPreviewController *)controller previewItemAtIndex:(NSInteger)index {
    
//    NSString * testPath = [NSTemporaryDirectory() stringByAppendingString:@"/2019_PDF.pdf"];
//    return [NSURL fileURLWithPath:testPath];
    
    id<WXOpenDocumentObject> v = [self.openObject implementProtocol:@protocol(WXOpenDocumentObject)];
    return [NSURL fileURLWithPath:v.filePath];
}



@end

@implementation KerWXObject (File)

-(WXKerFileOpenDocumentManager *)WXKerFileOpenDocumentManager{
    WXKerFileOpenDocumentManager * manager = objc_getAssociatedObject(self, WXKerFileOpenDocumentManagerKey);
    if (manager == nil) {
        manager = [[WXKerFileOpenDocumentManager alloc]init];
        objc_setAssociatedObject(self, WXKerFileOpenDocumentManagerKey, manager, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return manager;
}

-(void) saveFile: (KerJSObject *) object{
    
    id<WXSaveFileObject> v = [object implementProtocol:@protocol(WXSaveFileObject)];
    
    //Doucments
    NSString * storePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask,YES).firstObject stringByAppendingString:@"/store"];
    NSString * result = ker_CreateFileDirectory(storePath);
    
    if (!result) {
        
        //成功
        NSString * fileName = [[v.tempFilePath componentsSeparatedByString:@"/"] lastObject];
        NSString * savePath = [[storePath stringByAppendingString:@"/"]stringByAppendingString:fileName];
        //NSLog(@"save path = %@", savePath);
    
        NSError * err;
        BOOL success = [[NSFileManager defaultManager] copyItemAtPath:v.tempFilePath toPath:savePath error:&err];
        
        if (err) {
            NSString * msg = [NSString stringWithFormat:@"saveFile:fail copy to store directory fail, %@", err.description];
            WXSaveFileRes * res = [[WXSaveFileRes alloc] initWithErrMsg:msg path:@""];
            [v fail:res];
            [v complete:res];
            return;
        }
        
        if (!success) {
            NSString * msg = [NSString stringWithFormat:@"saveFile:fail copy to store directory fail"];
            WXSaveFileRes * res = [[WXSaveFileRes alloc] initWithErrMsg:msg path:@""];
            [v fail:res];
            [v complete:res];
            return;
        }
        
        WXSaveFileRes * res = [[WXSaveFileRes alloc] initWithErrMsg:@"saveFile:ok" path:savePath];
        [v success:res];
        [v complete:res];

    }else{
        
        //创建失败
        NSLog(@"创建目录失败");
        WXSaveFileRes * res = [[WXSaveFileRes alloc] initWithErrMsg:result path:@""];
        [v fail:res];
        [v complete:res];
        
    }
}

-(void) getFileInfo: (KerJSObject *) object {
    
    id<WXGetFileInfoObject> v = [object implementProtocol:@protocol(WXGetFileInfoObject)];
    
    //文件不存在
    if (![[NSFileManager defaultManager] fileExistsAtPath:v.filePath]) {
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
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:v.filePath]) {
        res.size = [[[NSFileManager defaultManager] attributesOfItemAtPath:v.filePath error:nil] fileSize];
    }else {
        res.size = 0;
    }

    if ([v.digestAlgorithm isEqualToString:@""] || [v.digestAlgorithm isEqualToString:@"md5"] || v.digestAlgorithm == nil) {
        res.digest = ker_Md5OfPath(v.filePath);
    }else if ([v.digestAlgorithm isEqualToString:@"sha1"]){
        res.digest = ker_Sha1OfPath(v.filePath);
    }else {
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"getFileInfo:fail invalid digestAlgorithm"];
        [v fail:res];
        [v complete:res];
        return;
    }
    
    [v success:res];
    [v complete:res];

}

-(void) getSavedFileList: (KerJSObject *) object {
    
    id<WXGetFileInfoObject> v = [object implementProtocol:@protocol(WXGetFileInfoObject)];
    NSString * storePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask,YES).firstObject stringByAppendingString:@"/store"];
    
    if (ker_FileDirectoryExist(storePath)) {
        
        NSDirectoryEnumerator * direnum = [[NSFileManager defaultManager] enumeratorAtPath:storePath];
        NSMutableArray * arr = [[NSMutableArray alloc] init];
        
        for (NSString * fileName in direnum) {
            
            if ([fileName isEqualToString:@".DS_Store"]) {
                continue;
            }
            
            NSString * path = [[storePath stringByAppendingString:@"/"] stringByAppendingString:fileName];
            NSError * err;
            NSDictionary * fileAttributes = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:&err];
            
            if (err) {
                WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:[NSString stringWithFormat:@"getSavedFileList:fail %@", err.description]];
                [v fail:res];
                [v complete:res];
                return;
            }
            
            NSDictionary * dic = @{
                                   @"filePath":path,
                                   @"size": [fileAttributes objectForKey:NSFileSize],
                                   @"createTime":[NSNumber numberWithInteger:[[fileAttributes objectForKey:NSFileCreationDate] timeIntervalSince1970]]
                                   };
            [arr addObject:dic];
        }
        
        WXGetSavedFileListRes * res = [[WXGetSavedFileListRes alloc] initWithErrMsg:@"getSavedFileList:ok" fileList:arr];
        [v success:res];
        [v complete:res];

    }else{
        
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"getSavedFileList:fail directory is not exist"];
        [v fail:res];
        [v complete:res];
        
    }

}

-(void) getSavedFileInfo: (KerJSObject *) object{
    
    id<WXKerFileObject> v = [object implementProtocol:@protocol(WXKerFileObject)];
    NSString * storePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask,YES).firstObject stringByAppendingString:@"/store"];
    //NSLog(@"storePath = %@", storePath);
    //NSLog(@"filePath = %@", v.filePath);
    if ([v.filePath hasPrefix:storePath]) {
        
        if ([[NSFileManager defaultManager] fileExistsAtPath:v.filePath]){
            
            NSError * err;
            NSDictionary * fileAttributes = [[NSFileManager defaultManager] attributesOfItemAtPath:v.filePath error:&err];
            
            NSDictionary * dic = @{
                                   @"errMsg":@"getSavedFileInfo ok",
                                   @"size": [fileAttributes objectForKey:NSFileSize],
                                   @"createTime":[NSNumber numberWithInteger:[[fileAttributes objectForKey:NSFileCreationDate] timeIntervalSince1970]]
                                   };
            [v success:dic];
            [v complete:dic];
            
        }else {
            
            //文件不存在
            WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"getSavedFileInfo:fail file doesn't exist"];
            [v fail:res];
            [v complete:res];
        }
        
    }else {
        
        //不是 sotre文件夹
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"getSavedFileInfo:fail not a store filePath"];
        [v fail:res];
        [v complete:res];
    }
}

-(void) removeSavedFile: (KerJSObject *) object{
    
    id<WXKerFileObject> v = [object implementProtocol:@protocol(WXKerFileObject)];
    NSString * storePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask,YES).firstObject stringByAppendingString:@"/store"];
    if ([v.filePath hasPrefix:storePath]) {
        
        if ([[NSFileManager defaultManager] fileExistsAtPath:v.filePath]){
            
            NSError * err;
            BOOL success = [[NSFileManager defaultManager] removeItemAtPath:v.filePath error:&err];
            
            if (err) {
                
                WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:[NSString stringWithFormat:@"removeSavedFile:fail err %@", err.description]];
                [v fail:res];
                [v complete:res];
                
            } else if (!success) {
                
                WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"removeSavedFile:fail"];
                [v fail:res];
                [v complete:res];
                
            } else {
                
                WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"removeSavedFile:ok"];
                [v success:res];
                [v complete:res];
                
            }
            
        }else{
            
            //文件不存在
            WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"removeSavedFile:fail file doesn't exist"];
            [v fail:res];
            [v complete:res];
            
        }
        
    }else{
        
        //不是 sotre文件夹
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"removeSavedFile:fail not a store filePath"];
        [v fail:res];
        [v complete:res];
    }
    
}

-(void) openDocument: (KerJSObject *) object{
    
    [self.WXKerFileOpenDocumentManager openDocument:object];

}

@end





@implementation NSString (File)

+(NSString *)ker_uuidString{
    
    CFUUIDRef uuid_ref = CFUUIDCreate(NULL);
    CFStringRef uuid_string_ref= CFUUIDCreateString(NULL, uuid_ref);
    NSString * uuid = [NSString stringWithString:(__bridge NSString *)uuid_string_ref];
    CFRelease(uuid_ref);
    CFRelease(uuid_string_ref);
    return [[uuid lowercaseString] stringByReplacingOccurrencesOfString:@"-" withString:@""];
    
}

@end
