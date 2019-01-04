//
//  KerWXHttpBody.m
//  KerWX
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXHttpBody.h"
#import <Ker/Ker.h>

@interface KerWXHttpBodyItem : NSObject {
    
}

@property(nonatomic,strong) NSString * key;

@end

@interface KerWXHttpBodyItemValue : KerWXHttpBodyItem {
    
}

@property(nonatomic,strong) NSString * value;

@end

@interface KerWXHttpBodyItemData : KerWXHttpBodyItem {
    
}

@property(nonatomic,strong) NSData * data;
@property(nonatomic,strong) NSString * type;
@property(nonatomic,strong) NSString * name;

@end

@implementation KerWXHttpBodyItem
@end

@implementation KerWXHttpBodyItemValue
@end

@implementation KerWXHttpBodyItemData
@end

@interface KerWXHttpBody() {
    
}

@property(nonatomic,strong) NSMutableArray * items;

@end

static NSString * KerWXHttpBodyToken = @"8jej23fkdxxd" ;
static NSString * KerWXHttpBodyTokenBegin = @"--8jej23fkdxxd";
static NSString * KerWXHttpBodyTokenEnd = @"--8jej23fkdxxd--";
static NSString * KerWXHttpBodyMutilpartType = @"multipart/form-data; boundary=8jej23fkdxxd";
static NSString * KerWXHttpBodyUrlencodedType = @"application/x-www-form-urlencoded";

@implementation KerWXHttpBody

@synthesize contentType = _contentType;
@synthesize items = _items;
@synthesize data = _data;

-(instancetype) init {
    if((self = [super init])) {
        _contentType = KerWXHttpBodyUrlencodedType;
    }
    return self;
}

-(NSMutableArray *) items {
    if(_items == nil) {
        _items = [NSMutableArray arrayWithCapacity:4];
    }
    return _items;
}

-(void) add:(NSString *)key value:(NSString *)value {
    KerWXHttpBodyItemValue * i = [[KerWXHttpBodyItemValue alloc] init];
    i.key = key;
    i.value = value;
    [self.items addObject:i];
}

-(void) add:(NSString *)key data:(NSData *)data type:(NSString *)type name:(NSString *)name {
    KerWXHttpBodyItemData * i = [[KerWXHttpBodyItemData alloc] init];
    i.key =key;
    i.data = data;
    i.type = type;
    i.name = name;
    [self.items addObject:i];
    _contentType = KerWXHttpBodyMutilpartType;
}

-(NSData *) data {
    if(_data == nil) {
        
        NSMutableData * mdata = [NSMutableData dataWithCapacity:64];
        
        if([self.contentType isEqualToString:KerWXHttpBodyMutilpartType]) {
            for(KerWXHttpBodyItem * i in _items) {
                if([i isKindOfClass:[KerWXHttpBodyItemValue class]]) {
                    KerWXHttpBodyItemValue * v = (KerWXHttpBodyItemValue *) i;
                    [mdata appendData:[KerWXHttpBodyTokenBegin dataUsingEncoding:NSUTF8StringEncoding]];
                    [mdata appendBytes:"\r\n" length:2];
                    [mdata appendData:[[NSString stringWithFormat:@"Content-Disposition: form-data; name=\"%@\"",i.key] dataUsingEncoding:NSUTF8StringEncoding]];
                    [mdata appendBytes:"\r\n\r\n" length:4];
                    [mdata appendData:[v.value dataUsingEncoding:NSUTF8StringEncoding]];
                    [mdata appendBytes:"\r\n" length:2];
                } else if([i isKindOfClass:[KerWXHttpBodyItemData class]]) {
                    
                    KerWXHttpBodyItemData * v = (KerWXHttpBodyItemData *) i;
                    
                    [mdata appendData:[KerWXHttpBodyTokenBegin dataUsingEncoding:NSUTF8StringEncoding]];
                    [mdata appendBytes:"\r\n" length:2];
                    [mdata appendData:[[NSString stringWithFormat:@"Content-Disposition: form-data; name=\"%@\"",i.key] dataUsingEncoding:NSUTF8StringEncoding]];
                    
                    if([v.name length]) {
                        [mdata appendData:[[NSString stringWithFormat:@"; filename=\"%@\"",v.name] dataUsingEncoding:NSUTF8StringEncoding]];
                    }
                    
                    [mdata appendBytes:"\r\n" length:2];
                    [mdata appendData:[[NSString stringWithFormat:@"Content-Type: %@\r\n",v.type] dataUsingEncoding:NSUTF8StringEncoding]];
                    [mdata appendData:[@"Content-Transfer-Encoding: binary\r\n\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
                    
                    [mdata appendData:v.data];
                    [mdata appendBytes:"\r\n" length:2];
                    
                }
            }
            [mdata appendData:[KerWXHttpBodyTokenEnd dataUsingEncoding:NSUTF8StringEncoding]];
        } else {
            for(KerWXHttpBodyItem * i in _items) {
                if([i isKindOfClass:[KerWXHttpBodyItemValue class]]) {
                    KerWXHttpBodyItemValue * v = (KerWXHttpBodyItemValue *) i;
                    if([mdata length] !=0 ){
                        [mdata appendBytes:"&" length:1];
                    }
                    [mdata appendData:[v.key dataUsingEncoding:NSUTF8StringEncoding]];
                    [mdata appendBytes:"=" length:1];
                    [mdata appendData:[[KerUI encodeURL:v.value] dataUsingEncoding:NSUTF8StringEncoding]];
                }
            }
        }
        
        _data = mdata;
    }
    return _data;
}

@end

