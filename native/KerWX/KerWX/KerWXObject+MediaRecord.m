//
//  KerWXObject+MediaRecord.m
//  KerWX
//
//  Created by zuowu on 2018/12/29.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+MediaRecord.h"
#import <AVFoundation/AVFoundation.h>

#define INPUT_BUS 1

@interface KerWXRecorderManager : NSObject <KerWXRecorderManager> {
    AudioUnit _audioUnit;
    AudioBufferList * _buffList;
}

@end

@implementation KerWXRecorderManager


-(void) dealloc {
    NSLog(@"[KerWXRecorderManager] [dealloc]");
}

-(void) start:(KerJSObject *)object{
    NSLog(@"KerWXRecorderManager star");
    
    id<KerWXRecorderManagerStartObject> starObject = [object implementProtocol:@protocol(KerWXRecorderManagerStartObject)];
    
    AudioUnitInitialize(_audioUnit);
    
    //initAudioSession
    NSError *error;
    AVAudioSession * audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord error:&error];
    [audioSession setPreferredSampleRate:44100 error:&error];
    [audioSession setPreferredInputNumberOfChannels:1 error:&error];
    [audioSession setPreferredIOBufferDuration:5 error:&error];
    
    
    //initBuffer
    UInt32 flag = 0;
    AudioUnitSetProperty(_audioUnit,
                         kAudioUnitProperty_ShouldAllocateBuffer,
                         kAudioUnitScope_Output,
                         INPUT_BUS,
                         &flag,
                         sizeof(flag));
    
    _buffList = (AudioBufferList*)malloc(sizeof(AudioBufferList));
    _buffList->mNumberBuffers = 1;
    _buffList->mBuffers[0].mNumberChannels = 1;
    _buffList->mBuffers[0].mDataByteSize = 2048 * sizeof(short);
    _buffList->mBuffers[0].mData = (short *)malloc(sizeof(short) * 2048);
    
    
    //initAudioComponent
    AudioComponentDescription audioDesc;
    audioDesc.componentType = kAudioUnitType_Output;
    audioDesc.componentSubType = kAudioUnitSubType_RemoteIO;
    audioDesc.componentManufacturer = kAudioUnitManufacturer_Apple;
    audioDesc.componentFlags = 0;
    audioDesc.componentFlagsMask = 0;
    
    AudioComponent inputComponent = AudioComponentFindNext(NULL, &audioDesc);
    AudioComponentInstanceNew(inputComponent, &_audioUnit);
    
    
    //initFormat
    /**
     AudioStreamBasicDescription:
     mSampleRate;       采样率, eg. 44100
     mFormatID;         格式, eg. kAudioFormatLinearPCM
     mFormatFlags;      标签格式, eg. kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked
     mBytesPerPacket;   每个Packet的Bytes数量, eg. 2
     mFramesPerPacket;  每个Packet的帧数量, eg. 1
     mBytesPerFrame;    (mBitsPerChannel / 8 * mChannelsPerFrame) 每帧的Byte数, eg. 2
     mChannelsPerFrame; 1:单声道；2:立体声, eg. 1
     mBitsPerChannel;   语音每采样点占用位数[8/16/24/32], eg. 16
     mReserved;         保留
     */
    
    AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate = 44100;
    audioFormat.mFormatID = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    audioFormat.mFramesPerPacket = 1;
    audioFormat.mChannelsPerFrame = 1;
    audioFormat.mBitsPerChannel = 16;
    audioFormat.mBytesPerPacket = 2;
    audioFormat.mBytesPerFrame = 2;
    
    if (noErr != AudioUnitSetProperty(_audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, INPUT_BUS, &audioFormat, sizeof(audioFormat))) {
        // send err
    }
    
    
    //initRecordeCallback
    AURenderCallbackStruct recordCallback;
    recordCallback.inputProc = RecordCallback;
    recordCallback.inputProcRefCon = (__bridge void *)self;
    if (noErr != AudioUnitSetProperty(_audioUnit, kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Global, INPUT_BUS, &recordCallback, sizeof(recordCallback))) {
        //send err
    }
}

static OSStatus RecordCallback(void *inRefCon,
                               AudioUnitRenderActionFlags *ioActionFlags,
                               const AudioTimeStamp *inTimeStamp,
                               UInt32 inBusNumber,
                               UInt32 inNumberFrames,
                               AudioBufferList *ioData)
{
    KerWXRecorderManager * THIS = (__bridge KerWXRecorderManager*)inRefCon;
    
    AudioUnitRender(THIS->_audioUnit, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, THIS->_buffList);
    
    short *data = (short *)THIS->_buffList->mBuffers[0].mData;
    NSLog(@"%d", data[0]);
    NSLog(@"%d", THIS->_buffList->mBuffers[0].mDataByteSize);
    
    //     NSLog(@"%ld", ioData->mBuffers[0].mDataByteSize);
    
    //    NSInteger time = [[NSDate date] timeIntervalSince1970] * 1000;
    //    NSLog(@"%ld call back", time);
    //    NSLog(@"%ld",sizeof(ioData->mBuffers->mData));
    //    ioData->mBuffers->mDataByteSize;
    
    
    //    sizeof(ioData->mBuffers->mData);
    
    return noErr;
}

@end


@implementation KerWXObject (MediaRecord)

-(void) startRecord:(KerJSObject *) object{
    
}

-(void) stopRecord:(KerJSObject *) object{
    
}

-(id<KerWXRecorderManager>) getRecorderManager: (KerJSObject *) object{
    
    KerWXRecorderManager * manager = [[KerWXRecorderManager alloc] init];
    [manager start:object];
    return manager;
}

@end
