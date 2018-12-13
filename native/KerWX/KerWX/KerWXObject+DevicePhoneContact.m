//
//  KerWXObject+DevicePhoneContact.m
//  KerWX
//
//  Created by zuowu on 2018/12/13.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+DevicePhoneContact.h"
#include <objc/runtime.h>

#define WXPhoneContactKey "WXPhoneContactKey"

@interface WXPhoneContact : NSObject <CNContactPickerDelegate, CNContactViewControllerDelegate>

@property (nonatomic, strong) KerJSObject * addPhoneContactObject;

@end

@implementation WXPhoneContact : NSObject

@synthesize addPhoneContactObject = _addPhoneContactObject;

-(UIViewController *)phoneContactViewController{
    return [[UIApplication sharedApplication] keyWindow].rootViewController;
}

-(void) addPhoneContact:(KerJSObject *) object{
    
    self.addPhoneContactObject = object;
    
    CNContactStore *contactStore = [[CNContactStore alloc] init];
    [contactStore requestAccessForEntityType:CNEntityTypeContacts completionHandler:^(BOOL granted, NSError * _Nullable error) {

        if (granted) {
            [self createContact];
        } else {
            
            id<WXAddPhoneContactObject> v = [object implementProtocol:@protocol(WXAddPhoneContactObject)];
            WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"addPhoneContact: request contact access fail cancel"];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });
            
        }
    }];

}

-(void)createContact{
    
    UIAlertController * actionSheet = [UIAlertController alertControllerWithTitle:nil message:nil preferredStyle:UIAlertControllerStyleActionSheet];
    
    UIAlertAction *action1 = [UIAlertAction actionWithTitle:@"创建新联系人" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        
        CNMutableContact * contact = [[CNMutableContact alloc] initWithKerWXAddPhoneContactObject:self.addPhoneContactObject];
        CNContactViewController * vc = [CNContactViewController viewControllerForNewContact:contact];
        vc.delegate = self;
        UINavigationController * nav = [[UINavigationController alloc] initWithRootViewController:vc];
        [self.phoneContactViewController presentViewController:nav animated:YES completion:nil];
        
    }];
    
    UIAlertAction *action2 = [UIAlertAction actionWithTitle:@"添加到现有联系人" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        
        CNContactPickerViewController * pickerVC = [[CNContactPickerViewController alloc] init];
        pickerVC.delegate = self;
        [self.phoneContactViewController presentViewController:pickerVC animated:YES completion:nil];
        
    }];
    
    UIAlertAction *action3 = [UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        
        if (self.addPhoneContactObject) {
            WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"addPhoneContact:fail cancel"];
            id<WXAddPhoneContactObject> v = [self.addPhoneContactObject implementProtocol:@protocol(WXAddPhoneContactObject)];
            [v fail:res];
            [v complete:res];
        }
        
    }];
    
    [actionSheet addAction:action1];
    [actionSheet addAction:action2];
    [actionSheet addAction:action3];
    
    [self.phoneContactViewController presentViewController:actionSheet animated:YES completion:nil];

}

#pragma mark -- CNContactViewControllerDelegate

- (BOOL)contactViewController:(CNContactViewController *)viewController shouldPerformDefaultActionForContactProperty:(CNContactProperty *)property{
    return NO;
}

- (void)contactViewController:(CNContactViewController *)viewController didCompleteWithContact:(nullable CNContact *)contact{
    
    if (self.addPhoneContactObject) {
        id<WXAddPhoneContactObject> v = [self.addPhoneContactObject implementProtocol:@protocol(WXAddPhoneContactObject)];
        if (contact == nil) {
            WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"addPhoneContact:fail cancel"];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });
        }else {
            WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"addPhoneContact:fail ok"];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v success:res];
                [v complete:res];
            });
        }
    }
    
    [self.phoneContactViewController dismissViewControllerAnimated:YES completion:nil];
    
}


#pragma mark --  CNContactPickerDelegate

- (void)contactPickerDidCancel:(CNContactPickerViewController *)picker{
    
    [self.phoneContactViewController dismissViewControllerAnimated:YES completion:^{
        if (self.addPhoneContactObject) {
            id<WXAddPhoneContactObject> v = [self.addPhoneContactObject implementProtocol:@protocol(WXAddPhoneContactObject)];
            WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"addPhoneContact:fail cancel"];
            dispatch_async(dispatch_get_main_queue(), ^{
                [v fail:res];
                [v complete:res];
            });
        }
    }];
    
}

- (void)contactPicker:(CNContactPickerViewController *)picker didSelectContact:(CNContact *)contact{
    
    [self.phoneContactViewController dismissViewControllerAnimated:YES completion:^{
        CNMutableContact * aa = [contact mutableCopy];
        CNMutableContact * bb= [[CNMutableContact alloc] initWithKerWXAddPhoneContactObject:self.addPhoneContactObject];
        [aa ker_updateWithMutableContact:bb];
        CNContactViewController * vc = [CNContactViewController  viewControllerForNewContact:aa];
        vc.delegate = self;
        UINavigationController * nav = [[UINavigationController alloc] initWithRootViewController:vc];
        [self.phoneContactViewController presentViewController:nav animated:YES completion:nil];
    }];

}

@end

@implementation KerWXObject (DevicePhoneContact)

-(WXPhoneContact *)WXPhoneContact{
    
    WXPhoneContact * v = objc_getAssociatedObject(self, WXPhoneContactKey);
    if(v == nil) {
        v = [[WXPhoneContact alloc]init];
        objc_setAssociatedObject(self, WXPhoneContactKey, v, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return v;
}

-(void) addPhoneContact:(KerJSObject *) object{
    [self.WXPhoneContact addPhoneContact:object];
}

@end



@implementation CNMutableContact (DevicePhoneContact)

-(instancetype)initWithKerWXAddPhoneContactObject:(KerJSObject *) kerJSObject {
    
    if (self = [super init]) {
        id<WXAddPhoneContactObject> object = [kerJSObject implementProtocol:@protocol(WXAddPhoneContactObject)];
        //名称 备注
        self.givenName = object.firstName;
        self.nickname = object.nickName;
        self.familyName = object.lastName;
        self.middleName = object.middleName;
        self.note = object.remark;
        self.jobTitle = object.title;
        self.organizationName = object.organization;
        
        //url
        if (object.url) {
            CNLabeledValue * homeurl = [CNLabeledValue labeledValueWithLabel:CNLabelURLAddressHomePage value:object.url];
            self.urlAddresses = @[homeurl];
        }

        //邮箱
        if (object.email) {
            CNLabeledValue * homeEmail = [CNLabeledValue labeledValueWithLabel:CNLabelHome value:object.email];
            self.emailAddresses = @[homeEmail];
        }
        
        
        //头像路径
        self.imageData = UIImagePNGRepresentation([UIImage imageWithContentsOfFile:object.photoFilePath]);
        
        //社交账号
        CNSocialProfile * wxProfile = [[CNSocialProfile alloc] initWithUrlString:nil username:nil userIdentifier:object.weChatNumber service:@"WeChat"];
        CNLabeledValue * wxProfileLabelValue = [CNLabeledValue labeledValueWithLabel:@"微信" value:wxProfile];
        
        CNSocialProfile * wbProfile = [[CNSocialProfile alloc] initWithUrlString:@"http://www.weibo.com" username:object.sinaWeiboName userIdentifier:object.sinaWeiboNumber service:CNSocialProfileServiceSinaWeibo];
        CNLabeledValue * wbProfileLabelValue = [CNLabeledValue labeledValueWithLabel:@"微博" value:wbProfile];
        self.socialProfiles = @[wxProfileLabelValue, wbProfileLabelValue];
        
        //电话号码
        NSMutableArray * numberArr = [[NSMutableArray alloc] init];
        
        if (object.mobilePhoneNumber) {
            [numberArr addObject:[CNLabeledValue labeledValueWithLabel:CNLabelPhoneNumberMobile value:[CNPhoneNumber phoneNumberWithStringValue:object.mobilePhoneNumber]]];
        }
        if (object.workFaxNumber) {
            [numberArr addObject:[CNLabeledValue labeledValueWithLabel:CNLabelPhoneNumberWorkFax value:[CNPhoneNumber phoneNumberWithStringValue:object.workFaxNumber]]];
        }
        if (object.workPhoneNumber) {
            [numberArr addObject:[CNLabeledValue labeledValueWithLabel:CNLabelWork value:[CNPhoneNumber phoneNumberWithStringValue:object.workPhoneNumber]]];
        }
        if (object.hostNumber) {
            [numberArr addObject:[CNLabeledValue labeledValueWithLabel:@"公司电话" value:[CNPhoneNumber phoneNumberWithStringValue:object.hostNumber]]];
        }
        if (object.homeFaxNumber) {
            [numberArr addObject:[CNLabeledValue labeledValueWithLabel:CNLabelPhoneNumberHomeFax value:[CNPhoneNumber phoneNumberWithStringValue:object.homeFaxNumber]]];
        }
        if (object.homePhoneNumber) {
            [numberArr addObject:[CNLabeledValue labeledValueWithLabel:CNLabelPhoneNumberMain value:[CNPhoneNumber phoneNumberWithStringValue:object.homePhoneNumber]]];
        }
        
        self.phoneNumbers = numberArr;
        
        //地址
        CNMutablePostalAddress * postalAddress = [[CNMutablePostalAddress alloc] init];
        postalAddress.state = object.addressState;
        postalAddress.city = object.addressCity;
        postalAddress.street = object.addressStreet;
        postalAddress.postalCode = object.addressPostalCode;
        CNLabeledValue * addressLabelValue = [CNLabeledValue labeledValueWithLabel:@"联系地址" value:postalAddress];
        
        //工作地址
        CNMutablePostalAddress * workAddress = [[CNMutablePostalAddress alloc] init];
        workAddress.state = object.workAddressState;
        workAddress.country = object.workAddressCountry;
        workAddress.city = object.workAddressCity;
        workAddress.street = object.workAddressStreet;
        workAddress.postalCode = object.workAddressPostalCode;
        CNLabeledValue * workAddressLabelValue = [CNLabeledValue labeledValueWithLabel:CNLabelWork value:workAddress];
        
        //住宅地址
        CNMutablePostalAddress * homeAddress = [[CNMutablePostalAddress alloc] init];
        homeAddress.state = object.homeAddressState;
        homeAddress.country = object.homeAddressCountry;
        homeAddress.city = object.homeAddressCity;
        homeAddress.street = object.homeAddressStreet;
        homeAddress.postalCode = object.homeAddressPostalCode;
        CNLabeledValue * homeAddressLabelValue = [CNLabeledValue labeledValueWithLabel:CNLabelHome value:homeAddress];
        
        //地址
        self.postalAddresses = @[addressLabelValue, workAddressLabelValue, homeAddressLabelValue];
        
    }
    return self;
}

-(instancetype)ker_updateWithMutableContact:(CNMutableContact *)contact {
    if (contact != nil) {
        //名称 备注
        if (contact.givenName) {
            self.givenName = contact.givenName;
        }
        if (contact.nickname) {
            self.nickname = contact.nickname;
        }
        if (contact.familyName) {
            self.familyName = contact.familyName;
        }
        if (contact.note) {
            self.note = contact.note;
        }
        if (contact.jobTitle) {
            self.jobTitle = contact.jobTitle;
        }
        if (contact.organizationName) {
            self.organizationName = contact.organizationName;
        }
        
        //url
        self.urlAddresses = [self.urlAddresses arrayByAddingObjectsFromArray:contact.urlAddresses];
        
        //邮箱
        self.emailAddresses = [self.emailAddresses arrayByAddingObjectsFromArray:contact.emailAddresses];
        
        //头像路径
        if (contact.imageData) { self.imageData = contact.imageData; }
        
        //社交账号
        self.socialProfiles = [self.socialProfiles arrayByAddingObjectsFromArray:contact.socialProfiles];
        
        //电话号码
        self.phoneNumbers = [self.phoneNumbers arrayByAddingObjectsFromArray:contact.phoneNumbers];
        
        //地址
        self.postalAddresses = [self.postalAddresses arrayByAddingObjectsFromArray:contact.postalAddresses];
    }
    return self;
}

@end
