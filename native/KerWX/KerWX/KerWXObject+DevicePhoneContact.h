//
//  KerWXObject+DevicePhoneContact.h
//  KerWX
//
//  Created by zuowu on 2018/12/13.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>
#import <Contacts/Contacts.h>
#import <ContactsUI/ContactsUI.h>

@protocol WXAddPhoneContactObject <WXCallbackFunction>

//基本信息
@property (nonatomic, copy) NSString * firstName;
@property (nonatomic, copy) NSString * nickName;
@property (nonatomic, copy) NSString * lastName;
@property (nonatomic, copy) NSString * middleName;
@property (nonatomic, copy) NSString * title;
@property (nonatomic, copy) NSString * remark;
@property (nonatomic, copy) NSString * photoFilePath;
@property (nonatomic, copy) NSString * organization;
@property (nonatomic, copy) NSString * email;
@property (nonatomic, copy) NSString * url;

//电话
@property (nonatomic, copy) NSString * mobilePhoneNumber;
@property (nonatomic, copy) NSString * workFaxNumber;
@property (nonatomic, copy) NSString * workPhoneNumber;
@property (nonatomic, copy) NSString * hostNumber;
@property (nonatomic, copy) NSString * homeFaxNumber;
@property (nonatomic, copy) NSString * homePhoneNumber;

//社交媒体
@property (nonatomic, copy) NSString * weChatNumber;
@property (nonatomic, copy) NSString * sinaWeiboNumber;
@property (nonatomic, copy) NSString * sinaWeiboName;

//地址 联系 工作 住宅
@property (nonatomic, copy) NSString * addressState;
@property (nonatomic, copy) NSString * addressCity;
@property (nonatomic, copy) NSString * addressStreet;
@property (nonatomic, copy) NSString * addressPostalCode;

@property (nonatomic, copy) NSString * workAddressCountry;
@property (nonatomic, copy) NSString * workAddressState;
@property (nonatomic, copy) NSString * workAddressCity;
@property (nonatomic, copy) NSString * workAddressStreet;
@property (nonatomic, copy) NSString * workAddressPostalCode;

@property (nonatomic, copy) NSString * homeAddressCountry;
@property (nonatomic, copy) NSString * homeAddressState;
@property (nonatomic, copy) NSString * homeAddressCity;
@property (nonatomic, copy) NSString * homeAddressStreet;
@property (nonatomic, copy) NSString * homeAddressPostalCode;

@end


@interface KerWXObject (DevicePhoneContact)

-(void) addPhoneContact:(KerJSObject *) kerJSObject;

@end


@interface CNMutableContact (DevicePhoneContact)

-(instancetype)initWithKerWXAddPhoneContactObject:(KerJSObject *) kerJSObject;
-(instancetype)ker_updateWithMutableContact:(CNMutableContact *) contact;

@end




