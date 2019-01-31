//
//  KerWXObject+Image.h
//  KerWX
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>


@interface KerWXChooseImageRes : NSObject

@property(nonatomic,strong) NSArray * tempFilePaths;
@property(nonatomic,strong) NSArray * tempFiles;

@end

@interface KerWXGetImageInfoRes : WXCallbackRes

@property (nonatomic, assign) double width;
@property (nonatomic, assign) double height;
@property (nonatomic, copy) NSString * path;
@property (nonatomic, copy) NSString * orientation;
@property (nonatomic, copy) NSString * type;

@end

@protocol KerWXChooseImageObject <NSObject>

@property(nonatomic,assign,readonly) int count;
@property(nonatomic,strong,readonly) id sizeType;
@property(nonatomic,strong,readonly) id sourceType;

-(void) success:(KerWXChooseImageRes *) res;
-(void) fail:(NSString *) errmsg;
-(void) complete;

@end

@protocol KerWXGetImageInfoObject <WXCallbackFunction>

@property (nonatomic, copy, readonly) NSString * src;

@end

@protocol KerWXSaveImageToPhotosAlbumObject <WXCallbackFunction>

@property (nonatomic, copy, readonly) NSString * filePath;

@end

@interface KerWXObject (Image)<UIImagePickerControllerDelegate,UINavigationControllerDelegate>

-(void) chooseImage:(KerJSObject *) object;
-(void) getImageInfo:(KerJSObject *) object;
-(void) saveImageToPhotosAlbum:(KerJSObject *) object;

@end


