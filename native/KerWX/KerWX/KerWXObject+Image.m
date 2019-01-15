//
//  KerWXObject+Image.m
//  KerWX
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+Image.h"

#include <objc/runtime.h>

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

@end



@implementation NSString (Image)

+(NSString *)ker_uuidString{
    
    CFUUIDRef uuid_ref = CFUUIDCreate(NULL);
    CFStringRef uuid_string_ref= CFUUIDCreateString(NULL, uuid_ref);
    NSString * uuid = [NSString stringWithString:(__bridge NSString *)uuid_string_ref];
    CFRelease(uuid_ref);
    CFRelease(uuid_string_ref);
    return [[uuid lowercaseString] stringByReplacingOccurrencesOfString:@"-" withString:@""];

}

@end
