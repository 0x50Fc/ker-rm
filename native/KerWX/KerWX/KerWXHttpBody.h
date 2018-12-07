//
//  KerWXHttpBody.h
//  KerWX
//
//  Created by hailong11 on 2018/12/6.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface KerWXHttpBody : NSObject

@property(nonatomic,strong,readonly) NSString * contentType;
@property(nonatomic,strong,readonly) NSData * data;

-(void) add:(NSString *) key value:(NSString *) value;
-(void) add:(NSString *) key data:(NSData *) data type:(NSString *) type name:(NSString *) name;


@end
