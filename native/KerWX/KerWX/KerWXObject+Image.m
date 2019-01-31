//
//  KerWXObject+Image.m
//  KerWX
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+Image.h"
#import <KerWXObject+File.h>
#include <objc/runtime.h>

NSString * ker_orientation(UIImageOrientation orientation){
    switch (orientation) {
        case UIImageOrientationUp:
            return @"up";
        case UIImageOrientationDown:
            return @"down";
        case UIImageOrientationLeft:
            return @"left";
        case UIImageOrientationRight:
            return @"right";
        case UIImageOrientationUpMirrored:
            return @"up-mirrored";
        case UIImageOrientationDownMirrored:
            return @"down-mirrored";
        case UIImageOrientationLeftMirrored:
            return @"left-mirrored";
        case UIImageOrientationRightMirrored:
            return @"right-mirrored";
    }
    return @"none";
}

@implementation KerWXGetImageInfoRes

-(instancetype) initWithUIImage:(UIImage *) image path:(NSString *)path{
    if (self = [super init]) {
        self.width = image.size.width;
        self.height = image.size.height;
        self.path = path;
        self.orientation = ker_orientation(image.imageOrientation);
        self.type = [[path componentsSeparatedByString:@"."] lastObject];
    }
    return self;
}

@end

@implementation KerWXChooseImageRes

@end

@implementation KerWXObject (Image)

-(void) chooseImage:(KerJSObject *) jsObject {
    
    id<KerWXChooseImageObject> object = [jsObject implementProtocol:@protocol(KerWXChooseImageObject)];
    
    UIImagePickerController * pickerController = [[UIImagePickerController alloc] init];
    
    objc_setAssociatedObject(pickerController, "_KerWXObjectChooseImageObject", object, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    pickerController.delegate = self;
    
    pickerController.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;

    id sourceType = [object sourceType];
    
    if([sourceType isKindOfClass:[NSArray class]] && [(NSArray *) sourceType count] > 0) {
        sourceType = sourceType[0];
    }
    
    if([sourceType isKindOfClass:[NSString class]] && [sourceType isEqualToString:@"album"]) {
        pickerController.sourceType = UIImagePickerControllerSourceTypeCamera;
    }
    
    UIViewController * topController = [[[UIApplication sharedApplication] keyWindow] rootViewController];
    
    while(topController && [topController presentingViewController]) {
        topController = [topController presentingViewController];
    }
    
    [topController presentViewController:pickerController animated:YES completion:nil];

}

-(void) imagePickerControllerDidCancel:(UIImagePickerController *)picker {
    [picker dismissViewControllerAnimated:YES completion:nil];
}

-(void) imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary<UIImagePickerControllerInfoKey,id> *)info {
    
    id<KerWXChooseImageObject> object  = objc_getAssociatedObject(picker, "_KerWXObjectChooseImageObject");
    
    if(object) {
        
        UIImage * image = [info valueForKey:UIImagePickerControllerOriginalImage];
        
        id sizeType = object.sizeType;
        
        if([sizeType isKindOfClass:[NSArray class]] && [(NSArray *) sizeType count] > 0) {
            sizeType = sizeType[0];
        }
        
        if(![sizeType isKindOfClass:[NSString class]]) {
             sizeType = @"original";
        }
        NSString * path  = [NSTemporaryDirectory() stringByAppendingFormat:@"KerWXObjectChooseImage.png"];
        
        NSString * uri = [KerUI resolveURI:path];
        
        if([sizeType isEqualToString:@"compressed"]) {
            CGSize size = image.size;
            CGFloat v = MIN(size.width,size.height);
            if(v > 640) {
                CGFloat r = 640 / v;
                CGFloat width = ceil(size.width * r);
                CGFloat height = ceil(size.height * r);
                UIGraphicsBeginImageContext(CGSizeMake(width, height));
                [image drawInRect:CGRectMake(0, 0, width, height)];
                image = UIGraphicsGetImageFromCurrentImageContext();
                UIGraphicsEndImageContext();
            }
        }
        
        NSData * data = UIImagePNGRepresentation(image);
        
        [data writeToFile:path atomically:YES];
        
        KerWXChooseImageRes * res = [[KerWXChooseImageRes alloc] init];
        
        res.tempFilePaths = @[uri];
        res.tempFiles = @[@{@"path":uri,@"size":@(data.length)}];
        
        [object success:res];
        [object complete];
        
    }
    
    [picker dismissViewControllerAnimated:YES completion:nil];
}

-(void) getImageInfo:(KerJSObject *) object{

    id<KerWXGetImageInfoObject> v = [object implementProtocol:@protocol(KerWXGetImageInfoObject)];

    if ([v.src hasPrefix:@"http://"] || [v.src hasPrefix:@"https://"]) {
        
        //网络图片
        NSString * tmpPath = [NSString stringWithFormat:@"%@%@.%@", NSTemporaryDirectory(), [NSString ker_uuidString], [v.src componentsSeparatedByString:@"."].lastObject];
        //NSLog(@"tmp path = %@", tmpPath);
        NSData * data = [NSData dataWithContentsOfURL:[NSURL URLWithString:v.src]];
        UIImage * image = [UIImage imageWithData:data];
        BOOL success = [data writeToFile:tmpPath atomically:YES];
        
        if (success) {
            
            KerWXGetImageInfoRes * res = [[KerWXGetImageInfoRes alloc] initWithUIImage:image path:tmpPath];
            [v success:res];
            [v complete:res];
            
        }else{
            
            WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"getImageInfo:fail, download url file fail"];
            [v fail:res];
            [v complete:res];
            
        }

    }else{
        
        //本地图片 路径问题以后处理
        UIImage * image = [UIImage imageWithContentsOfFile:v.src];
        KerWXGetImageInfoRes * res = [[KerWXGetImageInfoRes alloc] initWithUIImage:image path:v.src];
        [v success:res];
        [v complete:res];
        
    }

}

-(void) saveImageToPhotosAlbum:(KerJSObject *) object{
    
    id<KerWXSaveImageToPhotosAlbumObject> v = [object implementProtocol:@protocol(KerWXSaveImageToPhotosAlbumObject)];
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:v.filePath]) {
        
        UIImage * image = [UIImage imageWithContentsOfFile:v.filePath];
        UIImageWriteToSavedPhotosAlbum(image, self, @selector(image:didFinishSavingWithError:contextInfo:), (__bridge_retained void*)object);
        
    }else {
        
        //文件不存在
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"saveImageToPhotosAlbum:fail et file data fail"];
        [v fail:res];
        [v complete:res];
        
    }

}

-(void)image:(UIImage *)image didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo {

    id<KerWXSaveImageToPhotosAlbumObject> v = [(__bridge id) contextInfo implementProtocol:@protocol(KerWXSaveImageToPhotosAlbumObject)];
    //NSLog(@"%@", v.filePath);
    if(error){
        
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:[NSString stringWithFormat:@"saveImageToPhotosAlbum:fail error %@", error.description]];
        [v fail:res];
        [v complete:res];
        
    }else{
        
        WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"saveImageToPhotosAlbum:ok"];
        [v success:res];
        [v complete:res];
        
    }
}

@end




