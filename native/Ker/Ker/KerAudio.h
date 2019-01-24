//
//  KerAudio.h
//  Ker
//
//  Created by 张海龙 on 2019/1/24.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Ker/KerObject.h>

@interface KerAudioStartRecordRes  : NSObject

@property(nonatomic,strong) NSString * tempFilePath;

@end

@protocol KerAudioStartRecordObject <NSObject>

-(void) success:(KerAudioStartRecordRes *) res;

-(void) fail:(NSString *) errmsg;

-(void) complete;

@end


@interface KerAudio : NSObject<KerJSExport>

+(void) openlib;

-(void) startRecord:(KerJSObject *) jsObject;

-(void) stopRecord;

@end

