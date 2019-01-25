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

@protocol KerWXChooseImageObject <NSObject>

@property(nonatomic,assign,readonly) int count;
@property(nonatomic,strong,readonly) id sizeType;
@property(nonatomic,strong,readonly) id sourceType;

-(void) success:(KerWXChooseImageRes *) res;
-(void) fail:(NSString *) errmsg;
-(void) complete;

@end

@interface KerWXObject (Image)<UIImagePickerControllerDelegate,UINavigationControllerDelegate>

-(void) chooseImage:(KerJSObject *) object;

@end


