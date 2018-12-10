//
//  KerWXObject+Location.h
//  KerWX
//
//  Created by hailong11 on 2018/11/15.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWXObject.h>
#import <Ker/Ker.h>


@interface WXGetLocationRes : NSObject

@property(nonatomic,assign) double latitude;
@property(nonatomic,assign) double longitude;
@property(nonatomic,assign) double speed;
@property(nonatomic,assign) double accuracy;
@property(nonatomic,assign) double altitude;
@property(nonatomic,assign) double verticalAccuracy;
@property(nonatomic,assign) double horizontalAccuracy;

@end

@protocol WXGetLocationObject <NSObject>

@property(nonatomic,strong) NSString * type;
@property(nonatomic,assign,readonly) BOOL altitude;

-(void) success:(WXGetLocationRes *) res errmsg:(NSString *) errmsg;
-(void) fail:(NSString *) errmsg;
-(void) complete;

@end



@interface KerWXObject (Location)

-(void) getLocation:(KerJSObject *) object;

@end
