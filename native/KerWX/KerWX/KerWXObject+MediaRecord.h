//
//  KerWXObject+MediaRecord.h
//  KerWX
//
//  Created by zuowu on 2018/12/29.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>



@protocol KerWXRecorderManagerStartObject <NSObject>
@property (nonatomic, assign) double duration;         //录音时长 最多10分钟 600000
@property (nonatomic, assign) double sampleRate;       //采样率
@property (nonatomic, assign) int numberOfChannels;    //通道数
@property (nonatomic, assign) double encodeBitRate;    //编码码率
@property (nonatomic, copy) NSString * format;          //格式
@property (nonatomic, assign) double frameSize;        //帧大小
@property (nonatomic, copy) NSString * audioSource;     //输入源
@end

@protocol KerWXRecorderManager <NSObject, KerJSExport>

-(void) start:(KerJSObject *) object;

@end



@interface KerWXObject (MediaRecord)

-(void) startRecord:(KerJSObject *) object;
-(void) stopRecord:(KerJSObject *) object;

-(id<KerWXRecorderManager>) getRecorderManager:(KerJSObject *) object;

@end


